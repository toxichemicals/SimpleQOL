#ifndef SIMPLE_FS_H
#define SIMPLE_FS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* --- Platform Paths & Directory Headers --- */
#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #define PATH_SEP '\\'
    #define ALT_SEP '/'
    #define mkdir(path, mode) _mkdir(path)
#else
    #include <sys/stat.h>
    #include <dirent.h>
    #include <unistd.h>
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
    return (float)seed / (float)0xFFFFFFFF;
}

static inline long fs_size(const char* filename) {
    char clean_path[256]; strncpy(clean_path, filename, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);
    return size;
}

static inline char* fs_read(const char* filename) {
    char clean_path[256]; strncpy(clean_path, filename, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
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
    char clean_path[256]; strncpy(clean_path, filename, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "wb");
    if (!f) return 0;
    fprintf(f, "%s", data);
    fclose(f);
    return 1;
}

static inline int fs_append(const char* filename, const char* data) {
    char clean_path[256]; strncpy(clean_path, filename, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "ab");
    if (!f) return 0;
    fprintf(f, "%s", data);
    fclose(f);
    return 1;
}

static inline int fs_delete(const char* filename) {
    char clean_path[256]; strncpy(clean_path, filename, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
    return remove(clean_path) == 0;
}

static inline int fs_exists(const char* filename) {
    char clean_path[256]; strncpy(clean_path, filename, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "r");
    if (f) { fclose(f); return 1; }
    return 0;
}

/* --- Directory Functions --- */

static inline int fs_mkdir(const char* path) {
    if (!path) return 0;
    char clean_path[256]; strncpy(clean_path, path, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
    return mkdir(clean_path, 0777) == 0;
}

static inline int fs_dirremove(const char* path) {
    if (!path) return 0;
    char clean_path[256]; strncpy(clean_path, path, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
#ifdef _WIN32
    WIN32_FIND_DATAA findData;
    char search_path[280];
    sprintf(search_path, "%s\\*", clean_path);
    HANDLE hFind = FindFirstFileA(search_path, &findData);
    if (hFind == INVALID_HANDLE_VALUE) return 0;
    do {
        if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
            char full_path[512];
            sprintf(full_path, "%s\\%s", clean_path, findData.cFileName);
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) fs_dirremove(full_path);
            else DeleteFileA(full_path);
        }
    } while (FindNextFileA(hFind, &findData));
    FindClose(hFind);
    return RemoveDirectoryA(clean_path) != 0;
#else
    DIR* d = opendir(clean_path);
    if (!d) return 0;
    struct dirent* p;
    while ((p = readdir(d))) {
        if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) continue;
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", clean_path, p->d_name);
        struct stat statbuf;
        if (!stat(full_path, &statbuf)) {
            if (S_ISDIR(statbuf.st_mode)) fs_dirremove(full_path);
            else unlink(full_path);
        }
    }
    closedir(d);
    return rmdir(clean_path) == 0;
#endif
}

/**
 * Returns a piped string "name:type|name:type". 
 * CALLER MUST FREE RESULT.
 */
static inline char* fs_listdir(const char* path) {
    if (!path) return NULL;
    char clean_path[256]; strncpy(clean_path, path, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);

    size_t cap = 1024;
    char* result = (char*)malloc(cap);
    if (!result) return NULL;
    result[0] = '\0';

#ifdef _WIN32
    WIN32_FIND_DATAA findData;
    char search_path[280];
    sprintf(search_path, "%s\\*", clean_path);
    HANDLE hFind = FindFirstFileA(search_path, &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
                int is_dir = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
                size_t needed = strlen(result) + strlen(findData.cFileName) + 10;
                if (needed >= cap) { cap *= 2; result = (char*)realloc(result, cap); }
                if (result[0] != '\0') strcat(result, "|");
                strcat(result, findData.cFileName);
                strcat(result, is_dir ? ":dir" : ":file");
            }
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }
#else
    DIR* d = opendir(clean_path);
    if (d) {
        struct dirent* p;
        while ((p = readdir(d))) {
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) continue;
            char full_path[512];
            snprintf(full_path, sizeof(full_path), "%s/%s", clean_path, p->d_name);
            struct stat st;
            int is_dir = (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode));
            size_t needed = strlen(result) + strlen(p->d_name) + 10;
            if (needed >= cap) { cap *= 2; result = (char*)realloc(result, cap); }
            if (result[0] != '\0') strcat(result, "|");
            strcat(result, p->d_name);
            strcat(result, is_dir ? ":dir" : ":file");
        }
        closedir(d);
    }
#endif
    return result;
}

/* --- Utility Functions --- */

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
    char clean_path[256]; strncpy(clean_path, filename, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "rb");
    if (!f) return NULL;
    char* buffer = (char*)malloc(endbyte + 1);
    if (!buffer) { fclose(f); return NULL; }
    int bytesRead = (int)fread(buffer, 1, endbyte, f);
    buffer[bytesRead] = 0;
    fclose(f);
    return buffer;
}

static inline char* fs_readamount(const char* filename, int startbyte, int endbyte) {
    char clean_path[256]; strncpy(clean_path, filename, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "rb");
    if (!f) return NULL;
    fseek(f, startbyte, SEEK_SET);
    int amount = endbyte - startbyte;
    if (amount <= 0) { fclose(f); return NULL; }
    char* buffer = (char*)malloc(amount + 1);
    if (!buffer) { fclose(f); return NULL; }
    int bytesRead = (int)fread(buffer, 1, amount, f);
    buffer[bytesRead] = 0;
    fclose(f);
    return buffer;
}

#endif
