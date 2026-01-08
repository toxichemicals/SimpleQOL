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
