#ifndef SIMPLE_BUDP_H
#define SIMPLE_BUDP_H

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