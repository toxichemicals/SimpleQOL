#ifndef SIMPLE_THREADS_H
#define SIMPLE_THREADS_H

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
