#ifndef SIMPLE_FS_H
#define SIMPLE_FS_H

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
