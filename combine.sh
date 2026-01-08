#!/bin/bash

OUTPUT_NAME="QOL"

# Parse Arguments
for i in "$@"; do
    case $i in
        -dirs=*) DIRS="${i#*=}"; DIRS=${DIRS//,/ };;
        -files=*) FILES="${i#*=}"; FILES=${FILES//,/ };;
        -o=*) OUTPUT_NAME="${i#*=}";;
    esac
done

FINAL_FILE="${OUTPUT_NAME}.h"
GUARD=$(echo "${OUTPUT_NAME}_H" | tr '[:lower:]' '[:upper:]')
TEMP_ALL="raw_combine.tmp"
> "$TEMP_ALL"

# 1. Merge everything with forced newlines to prevent #endif#ifndef glue
for d in $DIRS; do
    for expanded_dir in $d; do
        if [ -d "$expanded_dir" ]; then
            for f in "$expanded_dir"/*.h; do
                printf "\n/* Source: $f */\n" >> "$TEMP_ALL"
                cat "$f" >> "$TEMP_ALL"
                printf "\n" >> "$TEMP_ALL"
            done
        fi
    done
done
for f in $FILES; do
    if [ -f "$f" ]; then
        printf "\n/* Source: $f */\n" >> "$TEMP_ALL"
        cat "$f" >> "$TEMP_ALL"
        printf "\n" >> "$TEMP_ALL"
    fi
done

# 2. Start the Final File
echo "#ifndef ${GUARD}" > "$FINAL_FILE"
echo "#define ${GUARD}" >> "$FINAL_FILE"
echo "" >> "$FINAL_FILE"

# 3. Global Constants & Safety Checks
# We put these at the TOP so all modules can see them
echo "/* --- QOL Global Configuration --- */" >> "$FINAL_FILE"
echo "#ifndef MAX_HTTP_BUF" >> "$FINAL_FILE"
echo "#define MAX_HTTP_BUF 65536" >> "$FINAL_FILE"
echo "#endif" >> "$FINAL_FILE"
echo "" >> "$FINAL_FILE"

# 4. Process with AWK
awk '
BEGIN { in_block = 0; }

# Keep track of platform #if / #endif nesting
/^#if/ { in_block++; print; next }
/^#endif/ { 
    if (in_block > 0) {
        in_block--;
        print;
    }
    next;
}

# Deduplicate top-level includes
/^#include/ {
    if (in_block > 0) {
        print; # Always keep nested includes (like winsock)
    } else {
        if (!seen[$0]) {
            print;
            seen[$0] = 1;
        }
    }
    next;
}

# Strip only the start-of-file guards
/^#ifndef .*_H/ || /^#define .*_H/ { next; }

# Print everything else
{ print }
' "$TEMP_ALL" >> "$FINAL_FILE"

echo "" >> "$FINAL_FILE"
echo "#endif /* ${GUARD} */" >> "$FINAL_FILE"

rm "$TEMP_ALL"
echo "Successfully amalgamated into ${FINAL_FILE} with global buffer safety."
