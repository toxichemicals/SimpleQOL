#ifndef SIMPLE_TOOLS_H
#define SIMPLE_TOOLS_H

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
#define MAX_HTTP_BUF 65536
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
