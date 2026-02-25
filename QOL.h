#ifndef QOL_H
#define QOL_H

/* --- QOL Global Configuration --- */
#ifndef MAX_HTTP_BUF
#define MAX_HTTP_BUF 65536
#endif\n

/* Source: SimpleBUDP/simple_budp.h */
#ifndef SIMPLE_BUDP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/stat.h>

#ifdef _WIN32
    #define _WIN32_WINNT 0x0600
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <direct.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET net_fd;
    #define b_sleep(ms) Sleep(ms)
    #define b_mkdir(path) _mkdir(path)
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <fcntl.h>
    #include <netdb.h>
    #include <sys/time.h>
    #include <pwd.h>
    typedef int net_fd;
    #define net_close close
    #define b_sleep(ms) usleep(ms * 1000)
    #define b_mkdir(path) mkdir(path, 0700)
#endif

#define MAX_B_BUF 65536
#define B_SEP "::"
#define MAX_B_SESSIONS 150
#define DH_G 5
#define DH_P 4294967291ULL

typedef enum { B_IDLE, B_CONNECTING, B_READY, B_DEAD } BudpState;

typedef struct {
    net_fd fd;
    BudpState state;
    uint32_t seq_out;
    struct sockaddr_in remote_addr;
    char remote_ip[64];
    int is_server;
    
    int enc_supported;   
    int client_wants_e;  
    int force_e;         
    int force_noe;       
    int crypto_active;   

    uint64_t my_priv_id;      
    uint64_t session_priv;    
    uint64_t shared_secret;
    uint64_t expected_remote_pub; 

    long long last_rx_time, last_hb_tx;
    int hb_enabled;

    char session_buf[MAX_B_BUF]; 
} BudpSession;

static BudpSession* _b_sessions = NULL;

/* --- Persistence Utilities --- */
static void _b_get_base_path(char* out, const char* seed) {
#ifdef _WIN32
    sprintf(out, "%s\\BUDP\\%llu", getenv("APPDATA"), (unsigned long long)5381); // Simple hash for path
#else
    struct passwd *pw = getpwuid(getuid());
    sprintf(out, "%s/.local/BUDP/%s", pw->pw_dir, seed);
#endif
}

static uint64_t _b_check_pinned_server(const char* seed, const char* ip, uint64_t incoming_id) {
    char path[512], filepath[1024];
    _b_get_base_path(path, seed);
    
    // Recursive mkdir for deep paths
    char tmp[512];
    snprintf(tmp, sizeof(tmp), "%s", path);
    for(char *p = tmp + 1; *p; p++) if(*p == '/') { *p = 0; b_mkdir(tmp); *p = '/'; }
    b_mkdir(tmp);

    sprintf(filepath, "%s/%s.bin", path, ip);
    
    uint64_t pinned_id = 0;
    FILE* f = fopen(filepath, "rb");
    if (f) {
        fread(&pinned_id, sizeof(pinned_id), 1, f);
        fclose(f);
        return pinned_id; // Return existing ID for comparison
    } else {
        f = fopen(filepath, "wb");
        if (f) {
            fwrite(&incoming_id, sizeof(incoming_id), 1, f);
            fclose(f);
        }
        return incoming_id; // First time, so it's "pinned" now
    }
}

/* --- Math & Crypto --- */
static uint64_t _b_hash(const char* str) {
    uint64_t hash = 5381; int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;
    return hash;
}

static uint64_t _b_pow_mod(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t res = 1; base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1) res = (__uint128_t)res * base % mod;
        base = (__uint128_t)base * base % mod;
        exp /= 2;
    }
    return res;
}

static inline uint16_t _b_checksum(const char* data) {
    uint32_t sum = 0;
    const uint16_t* ptr = (const uint16_t*)data;
    int len = (int)strlen(data);
    while (len > 1) { sum += *ptr++; len -= 2; }
    if (len > 0) { sum += *(const uint8_t*)ptr; }
    while (sum >> 16) { sum = (sum & 0xFFFF) + (sum >> 16); }
    return (uint16_t)(~sum);
}

static inline void _b_crypt(int id, char* data, uint32_t seq) {
    if (!_b_sessions[id].crypto_active || !_b_sessions[id].shared_secret) return;
    uint64_t key = _b_sessions[id].shared_secret;
    for (int i = 0; i < (int)strlen(data); i++) {
        data[i] ^= ((key >> (i % 8)) & 0xFF) ^ (uint8_t)(seq & 0xFF);
    }
}

static inline void _b_raw_send(int id, char* cmd, int force_plain) {
    char pkt[MAX_B_BUF], temp[MAX_B_BUF];
    strcpy(temp, cmd);
    uint32_t this_seq = _b_sessions[id].seq_out++;
    if (!force_plain) _b_crypt(id, temp, this_seq);
    uint16_t ck = _b_checksum(temp);
    sprintf(pkt, "%u%s%04X%s%s", this_seq, B_SEP, ck, B_SEP, temp);
    sendto(_b_sessions[id].fd, pkt, (int)strlen(pkt), 0, (struct sockaddr*)&_b_sessions[id].remote_addr, sizeof(struct sockaddr_in));
}

/* --- Public API --- */
static char _current_seed[128] = "default";

static inline void binit(const char* identity_seed) {
    if (_b_sessions) return;
#ifdef _WIN32
    WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
    strncpy(_current_seed, identity_seed, 127);
    _b_sessions = (BudpSession*)calloc(MAX_B_SESSIONS, sizeof(BudpSession));
    uint64_t my_id = _b_hash(identity_seed);
    for(int i=0; i<MAX_B_SESSIONS; i++) {
        _b_sessions[i].fd = -1;
        _b_sessions[i].my_priv_id = my_id;
        _b_sessions[i].enc_supported = 1; 
    }
    srand((unsigned int)time(NULL));
}

static inline void breset(int id) {
    if (id < 0 || id >= MAX_B_SESSIONS) return;
    _b_sessions[id].state = B_IDLE;
    _b_sessions[id].crypto_active = 0;
    _b_sessions[id].shared_secret = 0;
}

static inline void NoE(int id) { _b_sessions[id].enc_supported = 0; }
static inline void UseE(int id) { _b_sessions[id].client_wants_e = 1; }
static inline void bforceE(int id) { _b_sessions[id].force_e = 1; _b_sessions[id].enc_supported = 1; }
static inline uint64_t bget_my_public_id(int id) { return _b_pow_mod(DH_G, _b_sessions[id].my_priv_id, DH_P); }
static inline int bcconn(int id) { return (id >= 0 && _b_sessions[id].state == B_READY); }
static inline void enableheartbeat(int id) { _b_sessions[id].hb_enabled = 1; }

static inline int bopen(int id, int port) {
    net_fd fd = socket(AF_INET, SOCK_DGRAM, 0);
#ifdef _WIN32
    unsigned long m = 1; ioctlsocket(fd, FIONBIO, &m);
#else
    fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    _b_sessions[id].fd = fd;
    _b_sessions[id].is_server = (port != 0);
    _b_sessions[id].seq_out = 100;
    return 1;
}

static inline void bconnect(int id, char* host, int port) {
    struct hostent *he = gethostbyname(host);
    _b_sessions[id].remote_addr.sin_family = AF_INET;
    _b_sessions[id].remote_addr.sin_port = htons(port);
    memcpy(&_b_sessions[id].remote_addr.sin_addr.s_addr, he->h_addr, he->h_length);
    strncpy(_b_sessions[id].remote_ip, host, 63);
    
    _b_sessions[id].state = B_CONNECTING;
    char msg[128]; sprintf(msg, "CONN::%llu", bget_my_public_id(id));
    _b_raw_send(id, msg, 1);
}

static inline char* bupdate(int id) {
    if (id < 0 || id >= MAX_B_SESSIONS || _b_sessions[id].fd == -1) return NULL;
    struct timeval tv; gettimeofday(&tv, NULL);
    long long now = (long long)tv.tv_sec * 1000 + (tv.tv_usec / 1000);

    struct sockaddr_in src; socklen_t slen = sizeof(src);
    int res = recvfrom(_b_sessions[id].fd, _b_sessions[id].session_buf, MAX_B_BUF-1, 0, (struct sockaddr*)&src, &slen);
    if (res <= 0) {
        if (bcconn(id) && _b_sessions[id].hb_enabled && (now - _b_sessions[id].last_rx_time > 10000)) {
            _b_sessions[id].state = B_DEAD; return (char*)"DEAD";
        }
        return NULL;
    }
    _b_sessions[id].session_buf[res] = 0;

    char* s1 = strstr(_b_sessions[id].session_buf, B_SEP); if(!s1) return NULL;
    uint32_t rx_seq = (uint32_t)atoi(_b_sessions[id].session_buf);
    char* s2 = strstr(s1 + 2, B_SEP); if(!s2) return NULL;
    char* cmd = s2 + 2;

    if ((uint16_t)strtol(s1 + 2, NULL, 16) != _b_checksum(cmd)) return NULL;
    if (_b_sessions[id].crypto_active) _b_crypt(id, cmd, rx_seq);
    _b_sessions[id].last_rx_time = now;

    if (_b_sessions[id].is_server) {
        if (strncmp(cmd, "CONN::", 6) == 0) {
            _b_sessions[id].remote_addr = src;
            char resp[128]; sprintf(resp, "AUTH::%llu::%s", bget_my_public_id(id), _b_sessions[id].enc_supported ? "ES" : "NoE");
            _b_raw_send(id, resp, 1);
            return NULL;
        }
        if (strncmp(cmd, "ConnE::", 7) == 0) {
            _b_sessions[id].session_priv = (rand() % 0xFFFF) + 2;
            _b_sessions[id].shared_secret = _b_pow_mod(strtoull(cmd + 7, NULL, 10), _b_sessions[id].session_priv, DH_P);
            _b_sessions[id].crypto_active = 1;
            char resp[128]; sprintf(resp, "ACCEPTED_E::%llu", _b_pow_mod(DH_G, _b_sessions[id].session_priv, DH_P));
            _b_raw_send(id, resp, 1);
            _b_sessions[id].state = B_READY;
            return (char*)"CONNECTED";
        }
    } else {
        if (strncmp(cmd, "AUTH::", 6) == 0) {
            uint64_t incoming_id = strtoull(cmd + 6, NULL, 10);
            
            // TOFU PINNING LOGIC
            uint64_t pinned = _b_check_pinned_server(_current_seed, _b_sessions[id].remote_ip, incoming_id);
            if (pinned != incoming_id) {
                _b_sessions[id].state = B_DEAD;
                return (char*)"LOCAL_ERROR: MITM DETECTED! Server ID mismatch.";
            }

            char* mode = strstr(cmd + 6, "::") + 2;
            if (strcmp(mode, "ES") == 0 && _b_sessions[id].client_wants_e) {
                _b_sessions[id].session_priv = (rand() % 0xFFFF) + 2;
                char resp[128]; sprintf(resp, "ConnE::%llu", _b_pow_mod(DH_G, _b_sessions[id].session_priv, DH_P));
                _b_raw_send(id, resp, 1);
            } else {
                if (_b_sessions[id].force_e) { _b_sessions[id].state = B_DEAD; return (char*)"LOCAL_ERROR: E Required."; }
                _b_raw_send(id, "ConnNoE", 1);
            }
            return NULL;
        }
        if (strncmp(cmd, "ACCEPTED_E::", 12) == 0) {
            _b_sessions[id].shared_secret = _b_pow_mod(strtoull(cmd + 12, NULL, 10), _b_sessions[id].session_priv, DH_P);
            _b_sessions[id].crypto_active = 1;
            _b_sessions[id].state = B_READY;
            return (char*)"CONNECTED";
        }
    }
    return cmd;
}
// Add this to your header
static inline void bclose(int id) {
    if (id < 0 || id >= MAX_B_SESSIONS || _b_sessions[id].fd == -1) return;
    #ifdef _WIN32
        closesocket(_b_sessions[id].fd);
    #else
        close(_b_sessions[id].fd);
    #endif
    _b_sessions[id].fd = -1;
    _b_sessions[id].state = B_IDLE;
    _b_sessions[id].crypto_active = 0;
    _b_sessions[id].shared_secret = 0;
}

// Add this to allow changing seeds/keys mid-run
static inline void brekey(int id, const char* new_seed) {
    _b_sessions[id].my_priv_id = _b_hash(new_seed);
    _b_sessions[id].crypto_active = 0; // Drop current encryption
    _b_sessions[id].shared_secret = 0;
    _b_sessions[id].state = B_IDLE;
}
static inline void bsend(int id, char* data) { if (bcconn(id)) _b_raw_send(id, data, 0); }

#endif

/* Source: SimpleConflictsFixer/simple_conflicts_fixer.h */
#ifndef SimpleConflictsFixer
#define SimpleConflictsFixer

#ifdef _WIN32
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0600
#define CloseWindow WinCloseWindow
#define ShowCursor WinShowCursor
#define Rectangle WinRectangle
#define DrawText WinDrawText
#include <winsock2.h>
#include <windows.h>
#include <objbase.h>
#undef CloseWindow
#undef ShowCursor
#undef Rectangle
#undef DrawText
#ifdef interface
#undef interface
#endif
#endif

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
#if defined(_WIN32) && defined(HTTP_SSL)
/* * FIX: MSYS2/UCRT Linker Compatibility 
 * Provides the vsnprintf symbol expected by some OpenSSL static builds
 * when using vanilla MinGW cross-compilers.
 */
#include <stdarg.h>
#ifdef __cplusplus
extern "C" {
#endif
int __cdecl __imp__vsnprintf(char *buffer, size_t count, const char *format, va_list argptr) {
    return vsnprintf(buffer, count, format, argptr);
}
#ifdef __cplusplus
}
#endif
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
#include <process.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET net_fd;
typedef int socklen_t;
typedef CRITICAL_SECTION net_mutex;
#define net_mutex_init(m) InitializeCriticalSection(m)
#define net_lock(m)      EnterCriticalSection(m)
#define net_unlock(m)    LeaveCriticalSection(m)
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
#include <pthread.h>
typedef int net_fd;
typedef pthread_mutex_t net_mutex;
#define net_mutex_init(m) pthread_mutex_init(m, NULL)
#define net_lock(m)      pthread_mutex_lock(m)
#define net_unlock(m)    pthread_mutex_unlock(m)
#define net_close close
#define net_errno errno
#define INVALID_SOCKET -1
#endif

/* --- Definitions --- */
#define UDP 0
#define TCP 1
#define MAX_IDS 150
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
static net_mutex _net_global_lock;
static net_fd _net_fds[MAX_IDS];
static int _net_types[MAX_IDS];
static int _net_active[MAX_IDS];
static ClientInfo _last_client[MAX_IDS];

/* --- Initialization --- */
static inline void net_init() {
    static int init = 0;
    if (init) return;

    net_mutex_init(&_net_global_lock);
    net_lock(&_net_global_lock);

    for(int i = 0; i < MAX_IDS; i++) {
        _net_fds[i] = (net_fd)-1;
        _net_types[i] = 0;
        _net_active[i] = 0;
    }

    #ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    #endif

    init = 1;
    net_unlock(&_net_global_lock);
}

/* --- Core Functions --- */

static inline void closesocket_id(int id) {
    if (id >= 0 && id < MAX_IDS) {
        net_lock(&_net_global_lock);
        if (_net_fds[id] != (net_fd)-1) {
            net_close(_net_fds[id]);
            _net_fds[id] = (net_fd)-1;
            _net_active[id] = 0;
        }
        net_unlock(&_net_global_lock);
    }
}

static inline int isconnected(int id) {
    int connected = 0;
    if (id < 0 || id >= MAX_IDS) return 0;

    net_lock(&_net_global_lock);
    if (_net_fds[id] == (net_fd)-1) {
        net_unlock(&_net_global_lock);
        return 0;
    }
    if (_net_types[id] == UDP || _net_active[id] == 0) {
        net_unlock(&_net_global_lock);
        return 1;
    }

    char temp;
    int res = recv(_net_fds[id], &temp, 1, MSG_PEEK | MSG_DONTWAIT);
    if (res == 0) connected = 0;
    else if (res < 0) {
        #ifdef _WIN32
        connected = (net_errno == WSAEWOULDBLOCK);
        #else
        connected = (errno == EAGAIN || errno == EWOULDBLOCK);
        #endif
    } else {
        connected = 1;
    }

    net_unlock(&_net_global_lock);
    return connected;
}

static inline int opensocket(char* interface, int port, int id, int type) {
    net_init();
    if (id < 0 || id >= MAX_IDS) return 0;
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

    net_lock(&_net_global_lock);
    _net_fds[id] = fd;
    _net_types[id] = type;
    _net_active[id] = 0;
    net_unlock(&_net_global_lock);
    return 1;
}

static inline int openudp(char* interface, int port, int id) { return opensocket(interface, port, id, UDP); }
static inline int opentcp(char* interface, int port, int id) { return opensocket(interface, port, id, TCP); }

static inline int connecttcp(int id, char* host, int port) {
    net_init();
    if (id < 0 || id >= MAX_IDS) return 0;
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

    net_lock(&_net_global_lock);
    _net_fds[id] = fd;
    _net_types[id] = TCP;
    _net_active[id] = 1;
    net_unlock(&_net_global_lock);
    return 1;
}

static inline int senddata(int id, char* host, int port, char* data) {
    net_init();
    if (id < 0 || id >= MAX_IDS) return 0;

    net_lock(&_net_global_lock);
    if (_net_fds[id] == (net_fd)-1) {
        net_unlock(&_net_global_lock);
        if (!openudp("0.0.0.0", 0, id)) return 0;
        net_lock(&_net_global_lock);
    }

    struct hostent *he = gethostbyname(host);
    if (!he) { net_unlock(&_net_global_lock); return 0; }

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
    net_unlock(&_net_global_lock);
    return (res >= 0);
}

/* Updated for thread safety: Pass in a buffer to avoid shared state corruption */
static inline int receivefrom_safe(int id, char* out_buf, int out_size) {
    if (id < 0 || id >= MAX_IDS || out_buf == NULL) return -1;

    net_lock(&_net_global_lock);
    net_fd fd = _net_fds[id];
    if (fd == (net_fd)-1) { net_unlock(&_net_global_lock); return -1; }

    int bytes = -1;
    if (_net_types[id] == TCP) {
        if (_net_active[id] == 1) {
            bytes = recv(fd, out_buf, out_size - 1, 0);
        } else {
            struct sockaddr_in caddr;
            socklen_t clen = sizeof(caddr);
            net_fd n_fd = accept(fd, (struct sockaddr*)&caddr, &clen);
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
                bytes = recv(_net_fds[id], out_buf, out_size - 1, 0);
            }
        }
    } else {
        struct sockaddr_in src;
        socklen_t src_len = sizeof(src);
        bytes = recvfrom(fd, out_buf, out_size - 1, 0, (struct sockaddr*)&src, &src_len);
        if (bytes > 0) {
            strcpy(_last_client[id].host, inet_ntoa(src.sin_addr));
            _last_client[id].port = ntohs(src.sin_port);
        }
    }

    if (bytes > 0) out_buf[bytes] = 0;
    net_unlock(&_net_global_lock);
    return bytes;
}

static inline ClientInfo getclientinfo(int id) {
    ClientInfo info = {"", 0};
    if (id >= 0 && id < MAX_IDS) {
        net_lock(&_net_global_lock);
        info = _last_client[id];
        net_unlock(&_net_global_lock);
    }
    return info;
}

#endif


/* Source: SimpleParse/simple_parse.h */
#ifndef SIMPLE_PARSE_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

typedef struct {
    int byte;          // Global offset in file
    int lbyte;         // Offset relative to the start of the line (Column)
    int line;          // Line number
    char fullstr[256]; // Match: "spawn_player(hero);"
    char str[256];     // Signature: "spawn_player()"
    char con[256];     // Content: "hero"
} Parsed;

/* --- Internal Helpers --- */

static inline int _sp_count_lines(const char* start, const char* end) {
    int lines = 1;
    for (const char* p = start; p < end; p++) if (*p == '\n') lines++;
    return lines;
}

static inline const char* _sp_get_line_ptr(const char* haystack, int line_num) {
    if (line_num <= 1) return haystack;
    const char* p = haystack;
    int current_line = 1;
    while (*p) {
        if (*p == '\n') {
            current_line++;
            if (current_line == line_num) return p + 1;
        }
        p++;
    }
    return NULL;
}

static inline const char* _sp_find_line_start(const char* haystack, const char* pos) {
    const char* p = pos;
    while (p > haystack && *(p - 1) != '\n') p--;
    return p;
}

/* --- Core Logic --- */

static inline int parse(const char* haystack, const char* needle) {
    return (haystack && needle && strstr(haystack, needle) != NULL);
}

static inline int parseex(const char* input, const char* format, ...) {
    if (!input || !format) return 0;
    va_list args;
    va_start(args, format);
    int items = vsscanf(input, format, args);
    va_end(args);
    return items;
}

static inline Parsed advparse(const char* haystack, const char* needle) {
    Parsed p = { -1, -1, -1, {0}, {0}, {0} };
    if (!haystack || !needle) return p;
    const char* pos = strstr(haystack, needle);
    if (pos) {
        p.byte = (int)(pos - haystack);
        p.line = _sp_count_lines(haystack, pos);
        p.lbyte = (int)(pos - _sp_find_line_start(haystack, pos));
        strncpy(p.fullstr, needle, 255);
    }
    return p;
}

static inline Parsed parsecon(const char* haystack, const char* pattern) {
    Parsed p = { -1, -1, -1, {0}, {0}, {0} };
    char fmt[256]; strncpy(fmt, pattern, 255);
    char* split = strstr(fmt, "%%");
    if (!split) return p;
    *split = '\0';
    char* prefix = fmt; 
    char* suffix = split + 2;

    const char* start_ptr = strstr(haystack, prefix);
    if (!start_ptr) return p;

    const char* val_start = start_ptr + strlen(prefix);
    const char* end_ptr = strstr(val_start, suffix);
    if (!end_ptr) return p;

    p.byte = (int)(start_ptr - haystack);
    p.line = _sp_count_lines(haystack, start_ptr);
    p.lbyte = (int)(start_ptr - _sp_find_line_start(haystack, start_ptr));

    int full_len = (int)((end_ptr + strlen(suffix)) - start_ptr);
    if (full_len > 255) full_len = 255;
    strncpy(p.fullstr, start_ptr, full_len); p.fullstr[full_len] = '\0';
    snprintf(p.str, 255, "%s%s", prefix, suffix);

    int con_len = (int)(end_ptr - val_start);
    if (con_len > 255) con_len = 255;
    strncpy(p.con, val_start, con_len); p.con[con_len] = '\0';

    return p;
}

/* --- Line Logic --- */

static inline int parseline(const char* haystack, int line_num, char* dest, int max_len) {
    const char* line_ptr = _sp_get_line_ptr(haystack, line_num);
    if (!line_ptr) return 0;
    int i = 0;
    while (line_ptr[i] && line_ptr[i] != '\n' && line_ptr[i] != '\r' && i < max_len - 1) {
        dest[i] = line_ptr[i];
        i++;
    }
    dest[i] = '\0';
    return 1;
}

static inline Parsed parselineadv(const char* haystack, int line_num) {
    Parsed p = { -1, 0, line_num, {0}, {0}, {0} };
    const char* line_ptr = _sp_get_line_ptr(haystack, line_num);
    if (line_ptr) {
        p.byte = (int)(line_ptr - haystack);
        parseline(haystack, line_num, p.fullstr, 256);
        strncpy(p.str, p.fullstr, 255);
    }
    return p;
}

static inline Parsed parselinecon(const char* haystack, int line_num, const char* pattern) {
    const char* line_ptr = _sp_get_line_ptr(haystack, line_num);
    if (!line_ptr) { Parsed empty = {-1,-1,-1}; return empty; }
    Parsed p = parsecon(line_ptr, pattern);
    if (p.byte != -1) {
        p.lbyte = p.byte; 
        p.byte += (int)(line_ptr - haystack); 
        p.line = line_num;
    }
    return p;
}

#endif


/* Source: SimpleThreads/simple_threads.h */
#ifndef SIMPLE_THREADS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    typedef HANDLE native_t;
#else
    #define _GNU_SOURCE
    #include <pthread.h>
    #include <unistd.h>
    typedef pthread_t native_t;
#endif

#define MAX_THREADS 256
static native_t _st_pool[MAX_THREADS] = {0};
static void* _st_results[MAX_THREADS] = {0};

typedef struct {
    void* user_fn;
    char* args_str;
    int id;
    int type; 
    int arg_count; 
} _st_pkt;

static char* _st_strdup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char* d = malloc(len);
    if (d) memcpy(d, s, len);
    return d;
}

#ifdef _WIN32
static DWORD WINAPI _st_entry(LPVOID p) {
#else
static void* _st_entry(void* p) {
#endif
    _st_pkt* pkt = (_st_pkt*)p;
    int i[4] = {0};
    char s_buf[256] = {0};

    if (pkt->args_str && pkt->args_str[0] == '\"') {
        sscanf(pkt->args_str, "\"%[^\"]\"", s_buf);
    } else if (pkt->args_str) {
        sscanf(pkt->args_str, "%d, %d, %d, %d", &i[0], &i[1], &i[2], &i[3]);
    }

    if (pkt->type == 1) {
        int r = 0;
        if (pkt->arg_count == 0) r = ((int (*)())pkt->user_fn)();
        else if (pkt->arg_count == 1) r = ((int (*)(int))pkt->user_fn)(i[0]);
        else r = ((int (*)(int, int))pkt->user_fn)(i[0], i[1]);
        int* b = malloc(sizeof(int)); *b = r; _st_results[pkt->id] = b;
    } else {
        void* r = NULL;
        if (pkt->arg_count == 0) r = ((void* (*)())pkt->user_fn)();
        else if (pkt->args_str && pkt->args_str[0] == '\"') r = ((void* (*)(char*))pkt->user_fn)(s_buf);
        else if (pkt->arg_count == 1) r = ((void* (*)(int))pkt->user_fn)(i[0]);
        else r = ((void* (*)(int, int))pkt->user_fn)(i[0], i[1]);
        _st_results[pkt->id] = r;
    }
    free(pkt->args_str); free(pkt); return 0;
}

static inline void _st_spawn(void* fn, char* args, int id, int type, int count) {
    if (id < 0 || id >= MAX_THREADS) return;
    _st_pkt* p = malloc(sizeof(_st_pkt));
    p->user_fn = fn; p->args_str = _st_strdup(args);
    p->id = id; p->type = type; p->arg_count = count;
#ifdef _WIN32
    _st_pool[id] = CreateThread(NULL, 0, _st_entry, p, 0, NULL);
#else
    pthread_create(&_st_pool[id], NULL, _st_entry, p);
#endif
}

#define spawnthread(fn, args, id) { \
    int cnt = 0; \
    if (strlen(args) > 0) { cnt = 1; for(int _j=0; args[_j]; _j++) if(args[_j]==',') cnt++; } \
    _st_spawn(fn, args, id, _Generic((fn), \
        int (*)(): 1, int (*)(int): 1, int (*)(int,int): 1, \
        int (*)(int,int,int): 1, int (*)(int,int,int,int): 1, \
        default: 0), cnt); \
}

static inline int isrunning(int id) {
    if (id < 0 || id >= MAX_THREADS || !_st_pool[id]) return 0;
#ifdef _WIN32
    return WaitForSingleObject(_st_pool[id], 0) == WAIT_TIMEOUT;
#else
    return pthread_kill(_st_pool[id], 0) == 0;
#endif
}

static inline void* getreturn(int id) {
    if (id < 0 || id >= MAX_THREADS || !_st_pool[id]) return NULL;
#ifdef _WIN32
    WaitForSingleObject(_st_pool[id], INFINITE); CloseHandle(_st_pool[id]);
#else
    pthread_join(_st_pool[id], NULL);
#endif
    _st_pool[id] = 0; return _st_results[id];
}

static inline void secondsleep(float s) { 
#ifdef _WIN32
    Sleep((int)(s * 1000));
#else
    usleep((int)(s * 1000000));
#endif
}

static inline void killthread(int id) {
    if (id >= 0 && id < MAX_THREADS && _st_pool[id]) {
#ifdef _WIN32
        TerminateThread(_st_pool[id], 0); CloseHandle(_st_pool[id]);
#else
        pthread_cancel(_st_pool[id]);
#endif
        _st_pool[id] = 0;
    }
}

static inline void bombthreads() { for(int i=0; i<MAX_THREADS; i++) killthread(i); }

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
