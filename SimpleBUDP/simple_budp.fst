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
    #include <windows.h>
    #include <bcrypt.h>
    #pragma comment(lib, "bcrypt.lib")
    typedef SOCKET net_fd;
    #define b_sleep(s) Sleep((s) * 1000)
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <fcntl.h>
    #include <netdb.h>
    typedef int net_fd;
    #define b_sleep(s) sleep(s)
#endif

// --- TweetNaCl External Declarations ---
extern int crypto_box_keypair(unsigned char *y, unsigned char *x);
extern int crypto_scalarmult(unsigned char *q, const unsigned char *n, const unsigned char *p);
extern int crypto_stream_xor(unsigned char *c, const unsigned char *m, unsigned long long dlen, const unsigned char *n, const unsigned char *k);

#define MAX_B_BUF 65536
#define SLICE_SIZE 1000
#define MAX_B_SESSIONS 150

// Reliability & Handshake Constants
#define BUDP_DATA   0
#define BUDP_SYN    1
#define BUDP_SYNACK 2
#define BUDP_ACK    3
#define BUDP_NACK   4
#define BUDP_FIN   5
typedef enum { B_IDLE, B_CONNECTING, B_WAITING_ACK, B_READY, B_DEAD } BudpState;

#pragma pack(push, 1)
typedef struct {
    uint8_t type;
    uint32_t seq;
    uint16_t slice_idx;
    uint16_t slice_total;
    uint32_t checksum;
    uint32_t data_len;
    char data[SLICE_SIZE];
} BudpPacket;
#pragma pack(pop)

typedef struct {
    net_fd fd;
    BudpState state;
    struct sockaddr_in remote_addr;
    int is_server;
    unsigned char my_priv_key[32];
    unsigned char my_pub_key[32];
    unsigned char peer_pub_key[32];
    unsigned char shared_secret[32];
    int dh_established;
    uint32_t seq_out;
    uint32_t seq_in_expected;
    char assembly_buf[MAX_B_BUF];
    
    // --- Advanced ARQ & Reliability ---
    uint64_t slice_ack_mask;      // Selective Repeat bitmask (up to 64 slices)
    uint32_t last_nack_seq;       // Last processed sequence for NACK
    char reorder_buf[MAX_B_BUF * 4]; // Buffer for out-of-order packets
    
    // --- Timing & Termination ---
    uint64_t last_send_time;
    uint32_t srtt;
    uint32_t rto;
    int is_closing;
} BudpSession;
static BudpSession* _b_sessions = NULL;

void randombytes(unsigned char *buf, unsigned long long len) {
#ifdef _WIN32
    BCryptGenRandom(NULL, buf, (ULONG)len, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
#else
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd != -1) { read(fd, buf, len); close(fd); }
#endif
}

void _b_setup_dh(int id) {
    crypto_box_keypair(_b_sessions[id].my_pub_key, _b_sessions[id].my_priv_key);
}

void _b_compute_shared(int id) {
    crypto_scalarmult(_b_sessions[id].shared_secret, 
                      _b_sessions[id].my_priv_key, 
                      _b_sessions[id].peer_pub_key);
    _b_sessions[id].dh_established = 1;
}

static uint32_t _b_crc32(const void* data, size_t n_bytes) {
    uint32_t crc = 0xFFFFFFFF;
    const uint8_t* p = (const uint8_t*)data;
    while (n_bytes--) {
        crc ^= *p++;
        for (int i = 0; i < 8; i++)
            crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
    }
    return ~crc;
}

void binit(const char* seed) {
    if (_b_sessions) return;
#ifdef _WIN32
    WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
    _b_sessions = (BudpSession*)calloc(MAX_B_SESSIONS, sizeof(BudpSession));
    for(int i=0; i<MAX_B_SESSIONS; i++) { _b_sessions[i].fd = -1; _b_sessions[i].state = B_IDLE; }
}

int bopen(int id, int port) {
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
    return 1;
}

void _bsend_packet(int id, uint8_t type, const char* data, uint32_t len, uint32_t seq, uint16_t idx, uint16_t total, int encrypt) {
    BudpPacket pkt = {0};
    pkt.type = type; pkt.seq = seq; pkt.slice_idx = idx; pkt.slice_total = total; pkt.data_len = len;
    if (data) memcpy(pkt.data, data, len);
    
    if (encrypt && _b_sessions[id].dh_established) {
        unsigned char nonce[24] = {0};
        memcpy(nonce, &pkt.seq, sizeof(pkt.seq));
        memcpy(nonce + 4, &pkt.slice_idx, sizeof(pkt.slice_idx));
        crypto_stream_xor((unsigned char*)pkt.data, (unsigned char*)pkt.data, pkt.data_len, nonce, _b_sessions[id].shared_secret);
    }
    
    pkt.checksum = _b_crc32(pkt.data, pkt.data_len);
    sendto(_b_sessions[id].fd, (char*)&pkt, sizeof(BudpPacket), 0, (struct sockaddr*)&_b_sessions[id].remote_addr, sizeof(struct sockaddr_in));
}

void bsend(int id, char* data) {
    uint32_t len = (uint32_t)strlen(data);
    uint16_t total = (len / SLICE_SIZE) + 1;
    uint32_t current_seq = _b_sessions[id].seq_out++;
    for (uint16_t i = 0; i < total; i++) {
        uint32_t off = i * SLICE_SIZE;
        uint32_t d_len = (len - off > SLICE_SIZE) ? SLICE_SIZE : (len - off);
        _bsend_packet(id, BUDP_DATA, data + off, d_len, current_seq, i, total, 1);
    }
}

void bconnect(int id, const char* host, int port) {
    struct hostent *hp = gethostbyname(host);
    if (!hp) return;
    _b_sessions[id].remote_addr.sin_family = AF_INET;
    _b_sessions[id].remote_addr.sin_port = htons(port);
    memcpy(&_b_sessions[id].remote_addr.sin_addr, hp->h_addr_list[0], hp->h_length);
    _b_sessions[id].state = B_CONNECTING;
    _b_setup_dh(id);
    _bsend_packet(id, BUDP_SYN, (char*)_b_sessions[id].my_pub_key, 32, 0, 0, 1, 0);
}
void bclose(int id) {
    if (id < 0 || _b_sessions[id].fd == -1) return;
#ifdef _WIN32
    closesocket(_b_sessions[id].fd);
#else
    close(_b_sessions[id].fd);
#endif
    _b_sessions[id].fd = -1;
    _b_sessions[id].state = B_DEAD; // Optional: mark as permanently closed
}
char* bupdate(int id) {
    if (id < 0 || _b_sessions[id].fd == -1) return NULL;
    BudpPacket in;
    struct sockaddr_in src;
    socklen_t slen = sizeof(src);
    int res = recvfrom(_b_sessions[id].fd, (char*)&in, sizeof(BudpPacket), 0, (struct sockaddr*)&src, &slen);
    if (res < (int)sizeof(BudpPacket) || _b_crc32(in.data, in.data_len) != in.checksum) return NULL;

    if (in.type == BUDP_DATA) _bsend_packet(id, BUDP_ACK, NULL, 0, in.seq, in.slice_idx, 1, 0);

    switch(in.type) {
    case BUDP_SYN:
    if (_b_sessions[id].is_server) {
        // Validate against known client alias (e.g., "trusted_client")
        if (verify_peer_key("trusted_client", (unsigned char*)in.data)) {
            _b_sessions[id].remote_addr = src; 
            memcpy(_b_sessions[id].peer_pub_key, in.data, 32);
            _b_setup_dh(id);
            _b_sessions[id].state = B_WAITING_ACK;
            _bsend_packet(id, BUDP_SYNACK, (char*)_b_sessions[id].my_pub_key, 32, 0, 0, 1, 0);
        } else {
            // Identity mismatch: Reject and do not transition state
            return (char*)"UNAUTHORIZED_PEER";
        }
    } break;
        case BUDP_SYNACK:
    if (!_b_sessions[id].is_server && _b_sessions[id].state == B_CONNECTING) {
        // Validate against known server alias
        if (verify_peer_key("trusted_server", (unsigned char*)in.data)) {
            memcpy(_b_sessions[id].peer_pub_key, in.data, 32);
            _b_compute_shared(id);
            _b_sessions[id].state = B_READY;
            _bsend_packet(id, BUDP_ACK, "READY", 5, 0, 0, 1, 0);
            return (char*)"CONNECTED";
        } else {
            return (char*)"UNAUTHORIZED_PEER";
        }
    } break;
        case BUDP_ACK:
            if (_b_sessions[id].is_server && _b_sessions[id].state == B_WAITING_ACK) {
                _b_compute_shared(id);
                _b_sessions[id].state = B_READY;
                return (char*)"CONNECTED";
            } break;
                case BUDP_DATA:
        if (_b_sessions[id].state != B_READY) break;
        if (_b_sessions[id].dh_established) {
            unsigned char nonce[24] = {0};
            memcpy(nonce, &in.seq, sizeof(in.seq));
            memcpy(nonce + 4, &in.slice_idx, sizeof(in.slice_idx));
            crypto_stream_xor((unsigned char*)in.data, (unsigned char*)in.data, in.data_len, nonce, _b_sessions[id].shared_secret);
        }
        memcpy(_b_sessions[id].assembly_buf + (in.slice_idx * SLICE_SIZE), in.data, in.data_len);
        if (in.slice_idx == in.slice_total - 1) {
            _b_sessions[id].a
}
    return NULL;
}
// Define a simple file structure for trusted keys: ~/.local/BUDP/peers/{alias}.pub
int verify_peer_key(const char* alias, const unsigned char* received_key) {
    unsigned char stored_key[32];
    char path[256];
    snprintf(path, sizeof(path), "%s/.local/BUDP/peers/%s.pub", getenv("HOME"), alias);
    
    FILE* f = fopen(path, "rb");
    if (!f) return 0; // Untrusted: No key on file
    
    fread(stored_key, 1, 32, f);
    fclose(f);
    
    // Cryptographically compare keys
    return (memcmp(received_key, stored_key, 32) == 0);
}
