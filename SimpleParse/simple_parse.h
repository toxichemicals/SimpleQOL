#ifndef SIMPLE_PARSE_H
#define SIMPLE_PARSE_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

typedef struct {
    int byte;          // Global offset in file
    int lbyte;         // Offset relative to the start of the line (Column)
    int line;          // Line number
    char fullstr[256]; // Match: "spawn_player(hero);"
    char str[256];     // Signature: "spawn_player()"
    char con[256];     // Content: "hero"
} Parsed;

/* --- Internal Helpers --- */

static inline int _sp_count_lines(const char* start, const char* end) {
    int lines = 1;
    for (const char* p = start; p < end; p++) if (*p == '\n') lines++;
    return lines;
}

static inline const char* _sp_get_line_ptr(const char* haystack, int line_num) {
    if (line_num <= 1) return haystack;
    const char* p = haystack;
    int current_line = 1;
    while (*p) {
        if (*p == '\n') {
            current_line++;
            if (current_line == line_num) return p + 1;
        }
        p++;
    }
    return NULL;
}

static inline const char* _sp_find_line_start(const char* haystack, const char* pos) {
    const char* p = pos;
    while (p > haystack && *(p - 1) != '\n') p--;
    return p;
}

/* --- Core Logic --- */

static inline int parse(const char* haystack, const char* needle) {
    return (haystack && needle && strstr(haystack, needle) != NULL);
}

static inline int parseex(const char* input, const char* format, ...) {
    if (!input || !format) return 0;
    va_list args;
    va_start(args, format);
    int items = vsscanf(input, format, args);
    va_end(args);
    return items;
}

static inline Parsed advparse(const char* haystack, const char* needle) {
    Parsed p = { -1, -1, -1, {0}, {0}, {0} };
    if (!haystack || !needle) return p;
    const char* pos = strstr(haystack, needle);
    if (pos) {
        p.byte = (int)(pos - haystack);
        p.line = _sp_count_lines(haystack, pos);
        p.lbyte = (int)(pos - _sp_find_line_start(haystack, pos));
        strncpy(p.fullstr, needle, 255);
    }
    return p;
}

static inline Parsed parsecon(const char* haystack, const char* pattern) {
    Parsed p = { -1, -1, -1, {0}, {0}, {0} };
    char fmt[256]; strncpy(fmt, pattern, 255);
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
    p.lbyte = (int)(start_ptr - _sp_find_line_start(haystack, start_ptr));

    int full_len = (int)((end_ptr + strlen(suffix)) - start_ptr);
    if (full_len > 255) full_len = 255;
    strncpy(p.fullstr, start_ptr, full_len); p.fullstr[full_len] = '\0';
    snprintf(p.str, 255, "%s%s", prefix, suffix);

    int con_len = (int)(end_ptr - val_start);
    if (con_len > 255) con_len = 255;
    strncpy(p.con, val_start, con_len); p.con[con_len] = '\0';

    return p;
}

/* --- Line Logic --- */

static inline int parseline(const char* haystack, int line_num, char* dest, int max_len) {
    const char* line_ptr = _sp_get_line_ptr(haystack, line_num);
    if (!line_ptr) return 0;
    int i = 0;
    while (line_ptr[i] && line_ptr[i] != '\n' && line_ptr[i] != '\r' && i < max_len - 1) {
        dest[i] = line_ptr[i];
        i++;
    }
    dest[i] = '\0';
    return 1;
}

static inline Parsed parselineadv(const char* haystack, int line_num) {
    Parsed p = { -1, 0, line_num, {0}, {0}, {0} };
    const char* line_ptr = _sp_get_line_ptr(haystack, line_num);
    if (line_ptr) {
        p.byte = (int)(line_ptr - haystack);
        parseline(haystack, line_num, p.fullstr, 256);
        strncpy(p.str, p.fullstr, 255);
    }
    return p;
}

static inline Parsed parselinecon(const char* haystack, int line_num, const char* pattern) {
    const char* line_ptr = _sp_get_line_ptr(haystack, line_num);
    if (!line_ptr) { Parsed empty = {-1,-1,-1}; return empty; }
    Parsed p = parsecon(line_ptr, pattern);
    if (p.byte != -1) {
        p.lbyte = p.byte; 
        p.byte += (int)(line_ptr - haystack); 
        p.line = line_num;
    }
    return p;
}

#endif
