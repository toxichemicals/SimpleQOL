#!/bin/bash

# Default values
OUTPUT_NAME="QOL"
DIRS=""
FILES=""

# Function to display help
usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Amalgamates multiple C/C++ header files into a single header."
    echo ""
    echo "Options:"
    echo "  -dirs=DIR1,DIR2    Comma-separated list of directories to include"
    echo "  -files=F1,F2       Comma-separated list of specific files to include"
    echo "  -o=NAME            Output filename base (default: QOL)"
    echo "  -h, --help         Display this help message"
    echo ""
    echo "Example:"
    echo "  $0 -dirs=src/include,src/utils -o=MyLibrary"
    exit 0
}

# Parse Arguments
for i in "$@"; do
    case $i in
        -dirs=*)
            DIRS="${i#*=}"
            DIRS=${DIRS//,/ }
            ;;
        -files=*)
            FILES="${i#*=}"
            FILES=${FILES//,/ }
            ;;
        -o=*)
            OUTPUT_NAME="${i#*=}"
            ;;
        -h|--help)
            usage
            ;;
        *)
            echo "Unknown option: $i"
            usage
            ;;
    esac
done

# Validation: Ensure we actually have something to process
if [[ -z "$DIRS" && -z "$FILES" ]]; then
    echo "Error: You must provide at least one directory (-dirs) or file (-files)."
    echo "Run '$0 --help' for more information."
    exit 1
fi

FINAL_FILE="${OUTPUT_NAME}.h"
GUARD=$(echo "${OUTPUT_NAME}_H" | tr '[:lower:]' '[:upper:]' | tr '-' '_')
TEMP_ALL="raw_combine.tmp"

# Ensure temp file is cleaned up on exit or crash
trap 'rm -f "$TEMP_ALL"' EXIT

> "$TEMP_ALL"

# 1. Merge everything with forced newlines
for d in $DIRS; do
    if [ -d "$d" ]; then
        for f in "$d"/*.h; do
            if [ -f "$f" ]; then
                printf "\n/* Source: $f */\n" >> "$TEMP_ALL"
                cat "$f" >> "$TEMP_ALL"
                printf "\n" >> "$TEMP_ALL"
            fi
        done
    else
        echo "Warning: Directory '$d' not found. Skipping."
    fi
done

for f in $FILES; do
    if [ -f "$f" ]; then
        printf "\n/* Source: $f */\n" >> "$TEMP_ALL"
        cat "$f" >> "$TEMP_ALL"
        printf "\n" >> "$TEMP_ALL"
    else
        echo "Warning: File '$f' not found. Skipping."
    fi
done

# 2. Start the Final File
echo "#ifndef ${GUARD}" > "$FINAL_FILE"
echo "#define ${GUARD}" >> "$FINAL_FILE"
echo "" >> "$FINAL_FILE"

# 3. Global Constants & Safety Checks
echo "/* --- QOL Global Configuration --- */" >> "$FINAL_FILE"
echo "#ifndef MAX_HTTP_BUF" >> "$FINAL_FILE"
echo "#define MAX_HTTP_BUF 65536" >> "$FINAL_FILE"
echo "#endif" >> "$FINAL_FILE"
echo "" >> "$FINAL_FILE"

# 4. Process with AWK
awk '
BEGIN { in_block = 0; }

/^#if/ { in_block++; print; next }
/^#endif/ {
    if (in_block > 0) {
        in_block--;
        print;
    }
    next;
}

/^#include/ {
    if (in_block > 0) {
        print;
    } else {
        if (!seen[$0]) {
            print;
            seen[$0] = 1;
        }
    }
    next;
}

/^#ifndef .*_H/ || /^#define .*_H/ { next; }

{ print }
' "$TEMP_ALL" >> "$FINAL_FILE"

echo "" >> "$FINAL_FILE"
echo "#endif /* ${GUARD} */" >> "$FINAL_FILE"

echo "Successfully generated: ${FINAL_FILE}."
