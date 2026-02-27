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

#define MAX_B_BUF 65536
#define SLICE_SIZE 1000
#define MAX_B_SESSIONS 150

typedef enum { B_IDLE, B_CONNECTING, B_READY, B_DEAD } BudpState;

#pragma pack(push, 1)
typedef struct {
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
    int enc_requested;
    int enc_forced;
    suint256 my_priv_id;      
    suint256 shared_secret;
    uint32_t seq_out;
    uint32_t seq_in_expected;
    char assembly_buf[MAX_B_BUF];
    int hb_enabled;
} BudpSession;

static BudpSession* _b_sessions = NULL;

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
    uint64_t h = 5381; int c; const char* s = seed;
    while ((c = *s++)) h = ((h << 5) + h) + c;
    for(int i=0; i<MAX_B_SESSIONS; i++) {
        _b_sessions[i].my_priv_id.limbs[0] = (uint32_t)h;
        _b_sessions[i].my_priv_id.limbs[1] = (uint32_t)(h >> 32);
        _b_sessions[i].fd = -1;
        _b_sessions[i].state = B_IDLE;
    }
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
    _b_sessions[id].seq_out = 100;
    _b_sessions[id].seq_in_expected = 0;
    return 1;
}

void bsend(int id, char* data) {
    if (id < 0 || _b_sessions[id].fd == -1) return;
    uint32_t len = strlen(data);
    uint16_t total = (len / SLICE_SIZE) + 1;
    uint32_t current_seq = _b_sessions[id].seq_out++;

    for (uint16_t i = 0; i < total; i++) {
        BudpPacket pkt = {0};
        pkt.seq = current_seq;
        pkt.slice_idx = i;
        pkt.slice_total = total;
        uint32_t off = i * SLICE_SIZE;
        pkt.data_len = (len - off > SLICE_SIZE) ? SLICE_SIZE : (len - off);
        memcpy(pkt.data, data + off, pkt.data_len);
        pkt.checksum = _b_crc32(pkt.data, pkt.data_len);
        
        sendto(_b_sessions[id].fd, (char*)&pkt, sizeof(BudpPacket), 0, 
               (struct sockaddr*)&_b_sessions[id].remote_addr, sizeof(struct sockaddr_in));
    }
}

void bconnect(int id, const char* host, int port) {
    struct hostent *hp = gethostbyname(host);
    if (!hp) return;
    _b_sessions[id].remote_addr.sin_family = AF_INET;
    _b_sessions[id].remote_addr.sin_port = htons(port);
    memcpy(&_b_sessions[id].remote_addr.sin_addr, hp->h_addr_list[0], hp->h_length);
    _b_sessions[id].state = B_CONNECTING;
    bsend(id, "CONN");
}



char* bupdate(int id) {
    if (id < 0 || _b_sessions[id].fd == -1) return NULL;
    
    // Check multiple times to "drain" any packets that arrived during the 1s sleep
    for (int i = 0; i < 50; i++) {
        BudpPacket in;
        struct sockaddr_in src;
        socklen_t slen = sizeof(src);
        int res = recvfrom(_b_sessions[id].fd, (char*)&in, sizeof(BudpPacket), 0, (struct sockaddr*)&src, &slen);
        
        if (res < (int)sizeof(BudpPacket)) return NULL;
        if (_b_crc32(in.data, in.data_len) != in.checksum) continue;

        // Auto-sync sequence if we are just starting
        if (_b_sessions[id].state == B_IDLE || _b_sessions[id].state == B_CONNECTING) {
            _b_sessions[id].seq_in_expected = in.seq;
        }

        if (in.seq == _b_sessions[id].seq_in_expected) {
            memcpy(_b_sessions[id].assembly_buf + (in.slice_idx * SLICE_SIZE), in.data, in.data_len);
            
            if (in.slice_idx == in.slice_total - 1) {
                _b_sessions[id].assembly_buf[in.slice_idx * SLICE_SIZE + in.data_len] = 0;
                _b_sessions[id].seq_in_expected++;

                if (strcmp(_b_sessions[id].assembly_buf, "CONN") == 0 && _b_sessions[id].is_server) {
                    _b_sessions[id].remote_addr = src; // Capture the client's actual return address
                    _b_sessions[id].state = B_READY;
                    bsend(id, "CONNECTED");
                    return (char*)"CONNECTED";
                }
                
                if (strcmp(_b_sessions[id].assembly_buf, "CONNECTED") == 0) {
                    _b_sessions[id].state = B_READY;
                    return (char*)"CONNECTED";
                }
                
                return _b_sessions[id].assembly_buf;
            }
        }
    }
    return NULL;
}

uint64_t bget_my_public_id(int id) {
    return (uint64_t)_b_sessions[id].my_priv_id.limbs[0] | ((uint64_t)_b_sessions[id].my_priv_id.limbs[1] << 32);
}

void UseE(int id) { _b_sessions[id].enc_requested = 1; }
void NoE(int id) { _b_sessions[id].enc_requested = 0; }
void bforceE(int id) { _b_sessions[id].enc_forced = 1; }
void enableheartbeat(int id) { _b_sessions[id].hb_enabled = 1; }
int bcconn(int id) { return _b_sessions[id].state == B_READY; }
void breset(int id) { _b_sessions[id].state = B_IDLE; _b_sessions[id].seq_in_expected = 0; }

void bclose(int id) {
    if (id < 0 || _b_sessions[id].fd == -1) return;
#ifdef _WIN32
    closesocket(_b_sessions[id].fd);
#else
    close(_b_sessions[id].fd);
#endif
    _b_sessions[id].fd = -1;
}
