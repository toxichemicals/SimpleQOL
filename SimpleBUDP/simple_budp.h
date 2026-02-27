#ifndef SIMPLE_BUDP_UNIFIED_H
#define SIMPLE_BUDP_UNIFIED_H
#ifndef SIMPLE_TYPES_H
#define SIMPLE_TYPES_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

// --- Base Types ---
typedef uint8_t u8;   typedef uint16_t u16; typedef uint32_t u32; typedef uint64_t u64;
typedef int8_t s8;    typedef int16_t s16;  typedef int32_t s32;  typedef int64_t s64;

// --- Structural Definitions ---
#define budp_DEF_STRUCTS(BITS, LIMBS) \
    typedef struct { u32 limbs[LIMBS]; } budp_suint##BITS; \
    typedef struct { s32 limbs[LIMBS]; } budp_sint##BITS; \
    typedef struct { budp_suint##BITS mantissa; s32 exponent; } budp_sfloat##BITS;

budp_DEF_STRUCTS(32, 1)    budp_DEF_STRUCTS(64, 2)    budp_DEF_STRUCTS(128, 4)
budp_DEF_STRUCTS(256, 8)   budp_DEF_STRUCTS(512, 16)  budp_DEF_STRUCTS(1024, 32)
budp_DEF_STRUCTS(2048, 64) budp_DEF_STRUCTS(4096, 128) budp_DEF_STRUCTS(8192, 256)
budp_DEF_STRUCTS(12288, 384)

// --- Internal Helpers ---
static inline void _budp_internal_raw_hex(u32* limbs, int count) {
    printf("0x");
    int started = 0;
    for (int i = count - 1; i >= 0; i--) {
        if (limbs[i] != 0 || started || i == 0) {
            if (started) printf("%08X", limbs[i]);
            else printf("%X", limbs[i]);
            started = 1;
        }
    }
}

static inline void _budp_internal_dec_ascii(u32* limbs, int count) {
    u32 temp[count];
    memcpy(temp, limbs, count * sizeof(u32));
    char digits[5000]; 
    int pos = 0, digit_count = 0;
    while (1) {
        u64 remainder = 0;
        int all_zero = 1;
        for (int i = count - 1; i >= 0; i--) {
            u64 val = temp[i] + (remainder << 32);
            temp[i] = (u32)(val / 10);
            remainder = val % 10;
            if (temp[i] > 0) all_zero = 0;
        }
        if (digit_count > 0 && digit_count % 3 == 0) digits[pos++] = ',';
        digits[pos++] = (char)('0' + remainder);
        digit_count++;
        if (all_zero) break;
    }
    for (int i = pos - 1; i >= 0; i--) putchar(digits[i]);
}

// --- Shift Operations ---
#define budp_DEF_SHIFT(BITS, COUNT) \
    static inline void budp_suint##BITS##_shl(budp_suint##BITS *v, int shift) { \
        if (shift <= 0) return; \
        int limb_shift = shift / 32; \
        int bit_shift = shift % 32; \
        if (limb_shift >= COUNT) { memset(v->limbs, 0, sizeof(v->limbs)); return; } \
        for (int i = COUNT - 1; i >= 0; i--) { \
            if (i >= limb_shift) { \
                v->limbs[i] = v->limbs[i - limb_shift] << bit_shift; \
                if (bit_shift > 0 && i - limb_shift - 1 >= 0) \
                    v->limbs[i] |= v->limbs[i - limb_shift - 1] >> (32 - bit_shift); \
            } else v->limbs[i] = 0; \
        } \
    }

budp_DEF_SHIFT(32, 1)    budp_DEF_SHIFT(64, 2)    budp_DEF_SHIFT(128, 4)
budp_DEF_SHIFT(256, 8)   budp_DEF_SHIFT(512, 16)  budp_DEF_SHIFT(1024, 32)
budp_DEF_SHIFT(2048, 64) budp_DEF_SHIFT(4096, 128) budp_DEF_SHIFT(8192, 256)
budp_DEF_SHIFT(12288, 384)

// --- Printers ---
#define budp_DEF_PRINTERS(BITS, COUNT) \
    static inline void _budp_nf_u##BITS(budp_suint##BITS v) { _budp_internal_raw_hex(v.limbs, COUNT); } \
    static inline void _budp_f_u##BITS(budp_suint##BITS v)  { _budp_internal_dec_ascii(v.limbs, COUNT); } \
    static inline void _budp_nf_s##BITS(budp_sint##BITS v)  { printf("(S)"); _budp_internal_raw_hex((u32*)v.limbs, COUNT); } \
    static inline void _budp_f_s##BITS(budp_sint##BITS v)   { if(v.limbs[COUNT-1] < 0) printf("-"); _budp_internal_dec_ascii((u32*)v.limbs, COUNT); } \
    static inline void _budp_nf_flt##BITS(budp_sfloat##BITS v) { _budp_internal_raw_hex(v.mantissa.limbs, COUNT); printf(" E%d", v.exponent); } \
    static inline void _budp_f_flt##BITS(budp_sfloat##BITS v)  { _budp_internal_dec_ascii(v.mantissa.limbs, COUNT); printf(" * 2^%d", v.exponent); }

budp_DEF_PRINTERS(32, 1)    budp_DEF_PRINTERS(64, 2)    budp_DEF_PRINTERS(128, 4)
budp_DEF_PRINTERS(256, 8)   budp_DEF_PRINTERS(512, 16)  budp_DEF_PRINTERS(1024, 32)
budp_DEF_PRINTERS(2048, 64) budp_DEF_PRINTERS(4096, 128) budp_DEF_PRINTERS(8192, 256)
budp_DEF_PRINTERS(12288, 384)

#define budp_nfprint(val) _Generic((val), \
    budp_suint32: _budp_nf_u32, budp_suint64: _budp_nf_u64, budp_suint128: _budp_nf_u128, budp_suint256: _budp_nf_u256, budp_suint512: _budp_nf_u512, \
    budp_suint1024: _budp_nf_u1024, budp_suint2048: _budp_nf_u2048, budp_suint4096: _budp_nf_u4096, budp_suint8192: _budp_nf_u8192, budp_suint12288: _budp_nf_u12288, \
    budp_sint32: _budp_nf_s32, budp_sint64: _budp_nf_s64, budp_sint128: _budp_nf_s128, budp_sint256: _budp_nf_s256, budp_sint512: _budp_nf_s512, \
    budp_sint1024: _budp_nf_s1024, budp_sint2048: _budp_nf_s2048, budp_sint4096: _budp_nf_s4096, budp_sint8192: _budp_nf_s8192, budp_sint12288: _budp_nf_s12288, \
    budp_sfloat32: _budp_nf_flt32, budp_sfloat64: _budp_nf_flt64, budp_sfloat128: _budp_nf_flt128, budp_sfloat256: _budp_nf_flt256, budp_sfloat512: _budp_nf_flt512, \
    budp_sfloat1024: _budp_nf_flt1024, budp_sfloat2048: _budp_nf_flt2048, budp_sfloat4096: _budp_nf_flt4096, budp_sfloat8192: _budp_nf_flt8192, budp_sfloat12288: _budp_nf_flt12288 \
)(val)

#define budp_print(val) _Generic((val), \
    budp_suint32: _budp_f_u32, budp_suint64: _budp_f_u64, budp_suint128: _budp_f_u128, budp_suint256: _budp_f_u256, budp_suint512: _budp_f_u512, \
    budp_suint1024: _budp_f_u1024, budp_suint2048: _budp_f_u2048, budp_suint4096: _budp_f_u4096, budp_suint8192: _budp_f_u8192, budp_suint12288: _budp_f_u12288, \
    budp_sint32: _budp_f_s32, budp_sint64: _budp_f_s64, budp_sint128: _budp_f_s128, budp_sint256: _budp_f_s256, budp_sint512: _budp_f_s512, \
    budp_sint1024: _budp_f_s1024, budp_sint2048: _budp_f_s2048, budp_sint4096: _budp_f_s4096, budp_sint8192: _budp_f_s8192, budp_sint12288: _budp_f_s12288, \
    budp_sfloat32: _budp_f_flt32, budp_sfloat64: _budp_f_flt64, budp_sfloat128: _budp_f_flt128, budp_sfloat256: _budp_f_flt256, budp_sfloat512: _budp_f_flt512, \
    budp_sfloat1024: _budp_f_flt1024, budp_sfloat2048: _budp_f_flt2048, budp_sfloat4096: _budp_f_flt4096, budp_sfloat8192: _budp_f_flt8192, budp_sfloat12288: _budp_f_flt12288 \
)(val)
// --- Addition ---
#define budp_DEF_ADD(BITS, COUNT) \
    static inline u32 budp_suint##BITS##_add(budp_suint##BITS *res, budp_suint##BITS a, budp_suint##BITS b) { \
        u64 carry = 0; \
        for (int i = 0; i < COUNT; i++) { \
            u64 sum = (u64)a.limbs[i] + b.limbs[i] + carry; \
            res->limbs[i] = (u32)sum; \
            carry = sum >> 32; \
        } \
        return (u32)carry; \
    }

// --- Multiplication ---
#define budp_DEF_MUL(BITS, COUNT) \
    static inline void budp_suint##BITS##_mul(budp_suint##BITS *res, budp_suint##BITS a, budp_suint##BITS b) { \
        budp_suint##BITS out = {0}; \
        for (int i = 0; i < COUNT; i++) { \
            if (a.limbs[i] == 0) continue; \
            u64 carry = 0; \
            for (int j = 0; i + j < COUNT; j++) { \
                u64 cur = out.limbs[i + j] + (u64)a.limbs[i] * b.limbs[j] + carry; \
                out.limbs[i + j] = (u32)cur; \
                carry = cur >> 32; \
            } \
        } \
        *res = out; \
    }

budp_DEF_ADD(12288, 384)
budp_DEF_MUL(12288, 384)
// Power function: res = base ^ exp
static inline void budp_suint12288_pow(budp_suint12288 *res, u32 base_val, u32 exp) {
    budp_suint12288 b = {0}; b.limbs[0] = base_val;
    budp_suint12288 result = {0}; result.limbs[0] = 1;
    
    for (u32 i = 0; i < exp; i++) {
        budp_suint12288_mul(&result, result, b);
    }
    *res = result;
}

// Tetration: base ^^ height
static inline void budp_suint12288_tetrate(budp_suint12288 *res, u32 base, u32 height) {
    if (height == 0) { res->limbs[0] = 1; return; }
    if (height == 1) { res->limbs[0] = base; return; }
    
    // Starting with base^^2
    budp_suint12288 current_val;
    budp_suint12288_pow(&current_val, base, base);
    
    for (u32 i = 2; i < height; i++) {
        // This is the "Horrid" part. We use the previous result 
        // as the exponent for the next layer.
        // NOTE: We only take the first limb as the exponent here 
        // because budp_suint12288_pow expects a u32 exponent.
        budp_suint12288_pow(&current_val, base, current_val.limbs[0]);
    }
    *res = current_val;
}
#endif
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
    budp_suint256 my_priv_id;      
    budp_suint256 shared_secret;
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
#endif // SIMPLE_BUDP_UNIFIED_H
