#ifndef SIMPLE_PARSE_H
#define SIMPLE_PARSE_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

typedef struct {
    int byte;          
    int line;          
    char fullstr[256]; 
    char str[256];     
    char con[256];     
} Parsed;

/* Internal: Only uses standard string.h to keep it modular */
static inline int _sp_count_lines(const char* start, const char* end) {
    int lines = 1;
    for (const char* p = start; p < end; p++) {
        if (*p == '\n') lines++;
    }
    return lines;
}

static inline int parse(const char* haystack, const char* needle) {
    return (haystack && needle && strstr(haystack, needle) != NULL);
}

static inline Parsed advparse(const char* haystack, const char* needle) {
    Parsed p = { -1, -1, {0}, {0}, {0} };
    if (!haystack || !needle) return p;

    const char* pos = strstr(haystack, needle);
    if (pos) {
        p.byte = (int)(pos - haystack);
        p.line = _sp_count_lines(haystack, pos);
        strncpy(p.fullstr, needle, 255);
    }
    return p;
}

static inline int parseex(const char* input, const char* format, ...) {
    if (!input || !format) return 0;
    va_list args;
    va_start(args, format);
    int items = vsscanf(input, format, args);
    va_end(args);
    return items;
}

static inline Parsed parsecon(const char* haystack, const char* pattern) {
    Parsed p = { -1, -1, {0}, {0}, {0} };
    char fmt[256]; 
    strncpy(fmt, pattern, 255);
    char* split = strstr(fmt, "%%");
    if (!split) return p;

    *split = '\0';
    char* prefix = fmt;       
    char* suffix = split + 2; 

    const char* start_ptr = strstr(haystack, prefix);
    if (!start_ptr) return p;

    const char* val_start = start_ptr + strlen(prefix);
    const char* end_ptr = strstr(val_start, suffix);
    if (!end_ptr) return p;

    p.byte = (int)(start_ptr - haystack);
    p.line = _sp_count_lines(haystack, start_ptr);

    int full_len = (int)((end_ptr + strlen(suffix)) - start_ptr);
    if (full_len > 255) full_len = 255;
    strncpy(p.fullstr, start_ptr, full_len);
    p.fullstr[full_len] = '\0';

    snprintf(p.str, 255, "%s%s", prefix, suffix);

    int con_len = (int)(end_ptr - val_start);
    if (con_len > 255) con_len = 255;
    strncpy(p.con, val_start, con_len);
    p.con[con_len] = '\0';

    return p;
}

#endif
