#ifndef SIMPLE_HTTP_H
#define SIMPLE_HTTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Platform Detection & Cross-Platform Sleep --- */
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET http_fd;
    #define usleep(us) Sleep((us)/1000)
    #define sleep(s) Sleep((s)*1000)
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <fcntl.h>
    typedef int http_fd;
    #define INVALID_SOCKET -1
#endif

/* --- Modular SSL (Server Termination) --- */
#ifdef HTTP_SSL
    #include <openssl/ssl.h>
    #include <openssl/err.h>
    static SSL_CTX* _http_ssl_srv_ctx = NULL;
    static int _is_https_srv = 0;
#endif

/* --- Modular libcurl (Client Support) --- */
#ifdef HTTP_CURL
    #include <curl/curl.h>
    static size_t _curl_write_cb(void *ptr, size_t size, size_t nmemb, void *userdata) {
        size_t total = size * nmemb;
        strncat((char*)userdata, (char*)ptr, total);
        return total;
    }
#endif

#ifndef MAX_HTTP_BUF
#define MAX_HTTP_BUF 65536
#endif

typedef struct {
    char method[10];
    char path[256];
    int client_fd;
    int active;
    int verbose;
#ifdef HTTP_SSL
    SSL* ssl_handle;
#endif
} HttpState;

static HttpState _http_req = {0};
static http_fd _http_server_fd = INVALID_SOCKET;
static char _http_buffer[MAX_HTTP_BUF];

/* --- Internal Init --- */
static inline void _http_init_net() {
#ifdef _WIN32
    static int init = 0; if(!init) { WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa); init=1; }
#endif
}

/* --- [NEW] The libcurl Client --- */
static inline char* http_request(const char* url) {
    memset(_http_buffer, 0, MAX_HTTP_BUF);
#ifdef HTTP_CURL
    CURL *curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _curl_write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, _http_buffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Optional
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        return _http_buffer;
    }
    return "Error: Curl Init Failed";
#else
    return "Error: Define HTTP_CURL to use client functions.";
#endif
}

/* --- [NEW] SSL Server Termination --- */
static inline void http_host_ssl(int port, const char* cert, const char* key) {
#ifdef HTTP_SSL
    _http_init_net();
    SSL_library_init();
    _http_ssl_srv_ctx = SSL_CTX_new(TLS_server_method());
    if (SSL_CTX_use_certificate_chain_file(_http_ssl_srv_ctx, cert) <= 0 ||
        SSL_CTX_use_PrivateKey_file(_http_ssl_srv_ctx, key, SSL_FILETYPE_PEM) <= 0) return;
    _is_https_srv = 1;
    _http_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {.sin_family = AF_INET, .sin_addr.s_addr = INADDR_ANY, .sin_port = htons(port)};
    bind(_http_server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(_http_server_fd, 10);
#endif
}

/* --- Standard Server Logic --- */
static inline void http_host(int port) {
    _http_init_net();
    _http_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {.sin_family = AF_INET, .sin_addr.s_addr = INADDR_ANY, .sin_port = htons(port)};
    bind(_http_server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(_http_server_fd, 10);
}

static inline int http_reqready() {
    if (_http_req.active) return 1;
    struct sockaddr_in caddr;
    socklen_t clen = sizeof(caddr);
    int c_fd = accept(_http_server_fd, (struct sockaddr*)&caddr, &clen);
    if (c_fd >= 0) {
        _http_req.client_fd = c_fd;
#ifdef HTTP_SSL
        if (_is_https_srv) {
            _http_req.ssl_handle = SSL_new(_http_ssl_srv_ctx);
            SSL_set_fd(_http_req.ssl_handle, c_fd);
            SSL_accept(_http_req.ssl_handle);
            SSL_read(_http_req.ssl_handle, _http_buffer, MAX_HTTP_BUF-1);
        } else {
#endif
            recv(c_fd, _http_buffer, MAX_HTTP_BUF-1, 0);
#ifdef HTTP_SSL
        }
#endif
        sscanf(_http_buffer, "%s %s", _http_req.method, _http_req.path);
        _http_req.active = 1;
        return 1;
    }
    return 0;
}

static inline void http_reply(const char* html) {
    char h[512];
    sprintf(h, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n", (int)strlen(html));
#ifdef HTTP_SSL
    if (_http_req.ssl_handle) {
        SSL_write(_http_req.ssl_handle, h, (int)strlen(h));
        SSL_write(_http_req.ssl_handle, html, (int)strlen(html));
        SSL_free(_http_req.ssl_handle);
        _http_req.ssl_handle = NULL;
    } else {
#endif
        send(_http_req.client_fd, h, (int)strlen(h), 0);
        send(_http_req.client_fd, html, (int)strlen(html), 0);
#ifdef HTTP_SSL
    }
#endif
#ifdef _WIN32
    closesocket(_http_req.client_fd);
#else
    close(_http_req.client_fd);
#endif
    _http_req.active = 0;
}

static inline int http_is(const char* p) { return (_http_req.active && strcmp(_http_req.path, p) == 0); }

static inline char* http_param(const char* key) {
    static char v[256];
    char s[256]; sprintf(s, "%s=", key);
    char* st = strstr(_http_buffer, s);
    if (!st) return NULL;
    st += strlen(s);
    char* e = strpbrk(st, " &\r\n");
    int len = e ? (int)(e - st) : (int)strlen(st);
    if (len >= 256) len = 255;
    strncpy(v, st, len); v[len] = 0;
    return v;
}

#endif