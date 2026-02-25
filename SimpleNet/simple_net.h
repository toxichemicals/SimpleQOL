#ifndef SIMPLE_NET_H
#define SIMPLE_NET_H

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
