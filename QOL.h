#ifndef QOL_H
#define QOL_H

/* --- QOL Global Configuration --- */
#ifndef MAX_HTTP_BUF
#define MAX_HTTP_BUF 65536
#endif


/* Source: SimpleFS/simple_fs.h */
#ifndef SIMPLE_FS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
/* --- Platform Paths --- */
#ifdef _WIN32
    #define PATH_SEP '\\'
    #define ALT_SEP '/'
#else
    #define PATH_SEP '/'
    #define ALT_SEP '\\'
#endif

/* --- Internal Path Normalizer --- */
static inline void _fs_normalize(char* path) {
    if (!path) return;
    for (int i = 0; path[i]; i++) {
        if (path[i] == ALT_SEP) path[i] = PATH_SEP;
    }
}

/* --- Core Functions --- */

static inline int char_to_int(const void *ptr, int len) {
    uint32_t hash = 2166136261u;
    const unsigned char *p = (const unsigned char *)ptr;

    for (int i = 0; i < len; i++) {
        hash ^= p[i];
        hash *= 16777619u;
    }

    return (int)hash;
}

static inline float char_to_float(const void *ptr, int len) {
    unsigned int seed = (unsigned int)char_to_int(ptr, len);
    // 0xFFFFFFFF is the max value of uint32_t
    return (float)seed / (float)0xFFFFFFFF;
}

static inline long fs_size(const char* filename) {
    char clean_path[256]; strncpy(clean_path, filename, 255); _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);
    return size;
}

static inline char* fs_read(const char* filename) {
    char clean_path[256]; strncpy(clean_path, filename, 255); _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buffer = (char*)malloc(size + 1);
    if (!buffer) { fclose(f); return NULL; }
    fread(buffer, 1, size, f);
    buffer[size] = 0;
    fclose(f);
    return buffer;
}

static inline int fs_write(const char* filename, const char* data) {
    char clean_path[256]; strncpy(clean_path, filename, 255); _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "wb");
    if (!f) return 0;
    fprintf(f, "%s", data);
    fclose(f);
    return 1;
}

static inline int fs_append(const char* filename, const char* data) {
    char clean_path[256]; strncpy(clean_path, filename, 255); _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "ab");
    if (!f) return 0;
    fprintf(f, "%s", data);
    fclose(f);
    return 1;
}

static inline int fs_delete(const char* filename) {
    char clean_path[256]; strncpy(clean_path, filename, 255); _fs_normalize(clean_path);
    return remove(clean_path) == 0;
}

static inline int fs_exists(const char* filename) {
    char clean_path[256]; strncpy(clean_path, filename, 255); _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "r");
    if (f) { fclose(f); return 1; }
    return 0;
}

static inline int fs_count(const char* data, char target) {
    int count = 0;
    for (int i = 0; data[i]; i++) if (data[i] == target) count++;
    return count;
}

static inline char* fs_getline(char* data, int line_num) {
    static char line_buf[512];
    char* cur = data;
    for (int i = 0; i < line_num; i++) {
        cur = strchr(cur, '\n');
        if (!cur) return NULL;
        cur++;
    }
    char* next = strchr(cur, '\n');
    int len = next ? (int)(next - cur) : (int)strlen(cur);
    if (len > 511) len = 511;
    strncpy(line_buf, cur, len);
    line_buf[len] = 0;
    return line_buf;
}

static inline char* fs_readto(const char* filename, int endbyte) {
    char clean_path[256]; strncpy(clean_path, filename, 255); _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "rb");
    if (!f) return NULL;
    char* buffer = (char*)malloc(endbyte + 1);
    if (!buffer) { fclose(f); return NULL; }
    int bytesRead = fread(buffer, 1, endbyte, f);
    buffer[bytesRead] = 0;
    fclose(f);
    return buffer;
}

static inline char* fs_readamount(const char* filename, int startbyte, int endbyte) {
    char clean_path[256]; strncpy(clean_path, filename, 255); _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "rb");
    if (!f) return NULL;
    fseek(f, startbyte, SEEK_SET);
    int amount = endbyte - startbyte;
    if (amount <= 0) { fclose(f); return NULL; }
    char* buffer = (char*)malloc(amount + 1);
    if (!buffer) { fclose(f); return NULL; }
    int bytesRead = fread(buffer, 1, amount, f);
    buffer[bytesRead] = 0;
    fclose(f);
    return buffer;
}

#endif


/* Source: SimpleHTTP/simple_http.h */
#ifndef SIMPLE_HTTP_H

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

/* Source: SimpleNet/simple_net.h */
#ifndef SIMPLE_NET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Platform Shims & Headers --- */
#ifdef _WIN32
#define _WIN32_WINNT 0x0600
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET net_fd;
typedef int socklen_t;
#define net_close closesocket
#define net_errno WSAGetLastError()
#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0
#endif
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
typedef int net_fd;
#define net_close close
#define net_errno errno
#define INVALID_SOCKET -1
#endif

/* --- DX Definitions --- */
#define UDP 0
#define TCP 1
#define MAX_NET_BUF 65536

typedef struct {
    char host[128];
    int port;
} ClientInfo;

typedef struct {
    char* data;
    char host[128];
    int port;
} Packet;

/* --- Internal State --- */
static net_fd _net_fds[10] = {(net_fd)-1,(net_fd)-1,(net_fd)-1,(net_fd)-1,(net_fd)-1,(net_fd)-1,(net_fd)-1,(net_fd)-1,(net_fd)-1,(net_fd)-1};
static int _net_types[10] = {0,0,0,0,0,0,0,0,0,0};
static int _net_active[10] = {0,0,0,0,0,0,0,0,0,0};
static ClientInfo _last_client[10];
static char _net_static_buf[MAX_NET_BUF];

/* --- Initialization --- */
static inline void net_init() {
    static int init = 0;
    if (init) return;
    #ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    #endif
    init = 1;
}

/* --- Core Functions --- */

static inline void closesocket_id(int id) {
    if (id >= 0 && id <= 9 && _net_fds[id] != (net_fd)-1) {
        net_close(_net_fds[id]);
        _net_fds[id] = (net_fd)-1;
        _net_active[id] = 0;
    }
}

static inline int isconnected(int id) {
    if (id < 0 || id > 9 || _net_fds[id] == (net_fd)-1) return 0;
    if (_net_types[id] == UDP) return 1;
    if (_net_active[id] == 0) return 1;

    char temp;
    int res = recv(_net_fds[id], &temp, 1, MSG_PEEK | MSG_DONTWAIT);
    if (res == 0) return 0;
    #ifdef _WIN32
    if (res < 0 && net_errno != WSAEWOULDBLOCK) return 0;
    #else
    if (res < 0 && errno != EAGAIN && errno != EWOULDBLOCK) return 0;
    #endif
    return 1;
}

static inline int opensocket(char* interface, int port, int id, int type) {
    net_init();
    if (id < 0 || id > 9) return 0;
    closesocket_id(id);

    int sock_type = (type == TCP) ? SOCK_STREAM : SOCK_DGRAM;
    net_fd fd = socket(AF_INET, sock_type, 0);
    if (fd == (net_fd)-1) return 0;

    #ifdef _WIN32
    unsigned long m = 1; ioctlsocket(fd, FIONBIO, &m);
    #else
    fcntl(fd, F_SETFL, O_NONBLOCK);
    #endif

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = (interface == NULL) ? INADDR_ANY : inet_addr(interface);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        if (type == TCP || port != 0) { net_close(fd); return 0; }
    }

    if (type == TCP && port != 0) listen(fd, 5);

    _net_fds[id] = fd;
    _net_types[id] = type;
    _net_active[id] = 0;
    return 1;
}

static inline int openudp(char* interface, int port, int id) { return opensocket(interface, port, id, UDP); }
static inline int opentcp(char* interface, int port, int id) { return opensocket(interface, port, id, TCP); }

static inline int connecttcp(int id, char* host, int port) {
    net_init();
    if (id < 0 || id > 9) return 0;
    closesocket_id(id);

    net_fd fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == (net_fd)-1) return 0;

    struct hostent *he = gethostbyname(host);
    if (!he) { net_close(fd); return 0; }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr.s_addr, he->h_addr, he->h_length);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        #ifdef _WIN32
        if (net_errno != WSAEWOULDBLOCK) { net_close(fd); return 0; }
        #else
        if (errno != EINPROGRESS) { net_close(fd); return 0; }
        #endif
    }

    #ifdef _WIN32
    unsigned long m = 1; ioctlsocket(fd, FIONBIO, &m);
    #else
    fcntl(fd, F_SETFL, O_NONBLOCK);
    #endif

    _net_fds[id] = fd;
    _net_types[id] = TCP;
    _net_active[id] = 1;
    return 1;
}

static inline int senddata(int id, char* host, int port, char* data) {
    net_init();
    if (id < 0 || id > 9) return 0;

    if (_net_fds[id] == (net_fd)-1) {
        if (!openudp("0.0.0.0", 0, id)) return 0;
    }

    struct hostent *he = gethostbyname(host);
    if (!he) return 0;
    struct sockaddr_in dest = {0};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    memcpy(&dest.sin_addr.s_addr, he->h_addr, he->h_length);

    int res = -1;
    if (_net_types[id] == TCP) {
        res = send(_net_fds[id], data, (int)strlen(data), 0);
    } else {
        res = sendto(_net_fds[id], data, (int)strlen(data), 0, (struct sockaddr*)&dest, sizeof(dest));
    }
    return (res >= 0);
}

static inline char* receivefrom(int id) {
    if (id < 0 || id > 9 || _net_fds[id] == (net_fd)-1) return NULL;
    int bytes = -1;

    if (_net_types[id] == TCP) {
        if (_net_active[id] == 1) {
            bytes = recv(_net_fds[id], _net_static_buf, MAX_NET_BUF - 1, 0);
        } else {
            struct sockaddr_in caddr;
            socklen_t clen = sizeof(caddr);
            net_fd n_fd = accept(_net_fds[id], (struct sockaddr*)&caddr, &clen);
            if (n_fd != (net_fd)-1) {
                #ifdef _WIN32
                unsigned long m = 1; ioctlsocket(n_fd, FIONBIO, &m);
                #else
                fcntl(n_fd, F_SETFL, O_NONBLOCK);
                #endif
                net_close(_net_fds[id]);
                _net_fds[id] = n_fd;
                _net_active[id] = 1;
                strcpy(_last_client[id].host, inet_ntoa(caddr.sin_addr));
                _last_client[id].port = ntohs(caddr.sin_port);
                bytes = recv(_net_fds[id], _net_static_buf, MAX_NET_BUF - 1, 0);
            }
        }
    } else {
        struct sockaddr_in src;
        socklen_t src_len = sizeof(src);
        bytes = recvfrom(_net_fds[id], _net_static_buf, MAX_NET_BUF - 1, 0, (struct sockaddr*)&src, &src_len);
        if (bytes > 0) {
            strcpy(_last_client[id].host, inet_ntoa(src.sin_addr));
            _last_client[id].port = ntohs(src.sin_port);
        }
    }

    if (bytes > 0) {
        _net_static_buf[bytes] = 0;
        return _net_static_buf;
    }
    return NULL;
}

static inline Packet receivefrompro(int id) {
    Packet p = {NULL, "", 0};
    char* d = receivefrom(id);
    if (d) {
        p.data = d;
        strcpy(p.host, _last_client[id].host);
        p.port = _last_client[id].port;
    }
    return p;
}

static inline ClientInfo getclientinfo(int id) {
    if (id < 0 || id > 9) { ClientInfo e = {"", 0}; return e; }
    return _last_client[id];
}

#endif


/* Source: SimpleTools/simple_tools.h */
#ifndef SIMPLE_TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>

/* --- Cross-Platform Compatibility --- */

#ifdef _WIN32
    #include <windows.h>
    #define usleep(us) Sleep((us)/1000)
    #define sleep(s) Sleep((s)*1000)
#else
    #include <unistd.h>
#endif

#ifndef MAX_HTTP_BUF
#endif

/* --- Internal String Engine --- */

static inline void hexdmp(void* ptr, int buflen) {
    unsigned char* buf = (unsigned char*)ptr;
    for (int i = 0; i < buflen; i++) {
        // Print the hex value of the byte
        printf("%02x ", buf[i]);

        // Every 16 bytes, start a new line for readability
        if ((i + 1) % 16 == 0) printf("\n");
    }
    printf("\n");
}

static inline char* _str_replace_logic(const char* src, const char* find, const char* replace, int start_idx, int end_idx) {
    if (!src || !find || !replace) return NULL;
    size_t find_len = strlen(find);
    size_t replace_len = strlen(replace);
    size_t src_len = strlen(src);
    char* result = (char*)malloc(src_len * 10 + 1); 
    char* p_res = result;
    const char* p_src = src;
    int current_occurrence = 0;

    while (*p_src) {
        const char* pos = strstr(p_src, find);
        if (pos) {
            current_occurrence++;
            int should_replace = (current_occurrence >= start_idx && current_occurrence <= end_idx);
            size_t prefix_len = pos - p_src;
            memcpy(p_res, p_src, prefix_len);
            p_res += prefix_len;
            if (should_replace) {
                memcpy(p_res, replace, replace_len);
                p_res += replace_len;
            } else {
                memcpy(p_res, find, find_len);
                p_res += find_len;
            }
            p_src = pos + find_len;
        } else {
            strcpy(p_res, p_src);
            break;
        }
    }
    return result;
}

/* --- String Replacement API --- */

static inline char* str_replace_first(const char* s, const char* f, const char* r) { return _str_replace_logic(s, f, r, 1, 1); }
static inline char* str_replace_all(const char* s, const char* f, const char* r)   { return _str_replace_logic(s, f, r, 1, 999999); }
static inline char* str_replace_nth(const char* s, const char* f, const char* r, int n) { return _str_replace_logic(s, f, r, n, n); }
static inline char* str_replace_count(const char* s, const char* f, const char* r, int c) { return _str_replace_logic(s, f, r, 1, c); }
static inline char* str_replace_range(const char* s, const char* f, const char* r, int s_idx, int e_idx) { return _str_replace_logic(s, f, r, s_idx, e_idx); }

/* --- String Search & Logic --- */

static inline int str_starts(const char* s, const char* prefix) {
    if (!s || !prefix) return 0;
    return strncmp(s, prefix, strlen(prefix)) == 0;
}

static inline int str_ends(const char* s, const char* suffix) {
    if (!s || !suffix) return 0;
    size_t slen = strlen(s);
    size_t flen = strlen(suffix);
    return slen >= flen && strcmp(s + slen - flen, suffix) == 0;
}

static inline int str_contains(const char* s, const char* find) {
    return (s && find) ? strstr(s, find) != NULL : 0;
}

static inline int str_is_numeric(const char* s) {
    if (!s || *s == '\0') return 0;
    while (*s) { if (!isdigit(*s)) return 0; s++; }
    return 1;
}

/* --- String Formatting & Case --- */

static inline void str_lower(char* s) { for(; *s; ++s) *s = tolower(*s); }
static inline void str_upper(char* s) { for(; *s; ++s) *s = toupper(*s); }

static inline char* str_trim(char* s) {
    char* end;
    while(isspace((unsigned char)*s)) s++;
    if(*s == 0) return s;
    end = s + strlen(s) - 1;
    while(end > s && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return s;
}

static inline char* str_format(const char* fmt, ...) {
    char* res = (char*)malloc(MAX_HTTP_BUF);
    va_list args; va_start(args, fmt);
    vsprintf(res, fmt, args);
    va_end(args);
    return res;
}

static inline char* str_url_decode(const char* src) {
    if (!src) return NULL;
    char *res = (char*)malloc(strlen(src) + 1);
    char *p = res;
    while (*src) {
        if (*src == '%' && isxdigit(src[1]) && isxdigit(src[2])) {
            char hex[3] = { src[1], src[2], 0 };
            *p++ = (char)strtol(hex, NULL, 16);
            src += 3;
        } else if (*src == '+') {
            *p++ = ' '; src++;
        } else {
            *p++ = *src++;
        }
    }
    *p = '\0';
    return res;
}

/* --- Time Tools --- */

static inline char* time_now() {
    static char buf[32];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
    return buf;
}

static inline long time_unix() { return (long)time(NULL); }

/* --- JSON Builder --- */

static inline char* json_obj(const char* fmt, ...) {
    char* res = (char*)malloc(8192);
    strcpy(res, "{");
    va_list args; va_start(args, fmt);
    char* fmt_copy = strdup(fmt);
    char* token = strtok(fmt_copy, ", ");
    while (token) {
        char type = token[0];
        char* key = strchr(token, ':');
        if (key) {
            key++; strcat(res, "\""); strcat(res, key); strcat(res, "\":");
            if (type == 's') {
                char* val = va_arg(args, char*);
                strcat(res, "\""); strcat(res, val); strcat(res, "\"");
            } else if (type == 'i') {
                sprintf(res + strlen(res), "%d", va_arg(args, int));
            } else if (type == 'f') {
                sprintf(res + strlen(res), "%.2f", va_arg(args, double));
            } else if (type == 'b') {
                strcat(res, va_arg(args, int) ? "true" : "false");
            }
        }
        token = strtok(NULL, ", ");
        if (token) strcat(res, ", ");
    }
    strcat(res, "}");
    free(fmt_copy); va_end(args);
    return res;
}

#endif


#endif /* QOL_H */
