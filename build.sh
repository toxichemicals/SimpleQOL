#!/bin/bash

OUTPUT_NAME="QOL"
DIRS=""
FILES=""
DOCS_FILE="Alldocs.mside"

usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Options:"
    echo "  -dirs=DIR1,DIR2    Include directories"
    echo "  -files=F1,F2       Include specific files"
    echo "  -o=NAME            Output name (default: QOL)"
    exit 0
}

# Argument Parsing
for i in "$@"; do
    case $i in
        -dirs=*) DIRS="${i#*=}"; DIRS=${DIRS//,/ } ;;
        -files=*) FILES="${i#*=}"; FILES=${FILES//,/ } ;;
        -o=*) OUTPUT_NAME="${i#*=}" ;;
        -h|--help) usage ;;
    esac
done

if [[ -z "$DIRS" && -z "$FILES" ]]; then
    echo "Error: Provide -dirs or -files."; exit 1
fi

FINAL_FILE="${OUTPUT_NAME}.h"
GUARD=$(echo "${OUTPUT_NAME}_H" | tr '[:lower:]' '[:upper:]' | tr '-' '_')
TEMP_ALL="raw_combine.tmp"

# Cleanup trap
trap 'rm -f "$TEMP_ALL"' EXIT

# Initialize files
: > "$TEMP_ALL"
: > "$DOCS_FILE"

# 1. Amalgamation + .mside collection
for d in $DIRS; do
    if [ -d "$d" ]; then
        # Use a find command to avoid shell expansion issues
        for f in "$d"/*.h; do
            if [ -f "$f" ]; then
                printf "\n/* Source: %s */\n" "$f" >> "$TEMP_ALL"
                cat "$f" >> "$TEMP_ALL"
                printf "\n" >> "$TEMP_ALL"
            fi
        done
        # Collect Mark-Sideways files
        if [ -f "$d/docs.mside" ]; then
            printf "\n@@@ Module: %s @@@\n\n" "$d" >> "$DOCS_FILE"
            cat "$d/docs.mside" >> "$DOCS_FILE"
            printf "\n\n---\n" >> "$DOCS_FILE"
        fi
    fi
done

for f in $FILES; do
    if [ -f "$f" ]; then
        printf "\n/* Source: %s */\n" "$f" >> "$TEMP_ALL"
        cat "$f" >> "$TEMP_ALL"
    fi
done

# 2. Reconstruct Header
{
    echo "#ifndef ${GUARD}"
    echo "#define ${GUARD}"
    echo ""
    echo "/* --- System Feature Macros --- */"
    echo "#ifndef _GNU_SOURCE"
    echo "#define _GNU_SOURCE"
    echo "#endif"
    echo "#include <signal.h>"
    echo "#include <pthread.h>"
    echo ""
    echo "/* --- QOL Global Configuration --- */"
    echo "#ifndef MAX_HTTP_BUF"
    echo "#define MAX_HTTP_BUF 65536"
    echo "#endif"
    echo ""

    # Run awk to clean up duplicate includes and guards
    awk '
    BEGIN { b=0; }
    /^#if/ { b++; print; next }
    /^#endif/ { 
        if(b>0) { b--; print "#endif"; } 
        next; 
    }
    /^#include/ { 
        if(b>0) { print; } 
        else { if(!s[$0]){print; s[$0]=1;} } 
        next; 
    }
    /^#ifndef .*_H/ || /^#define .*_H/ { next; }
    { print }
    ' "$TEMP_ALL"

    echo ""
    echo "#endif /* ${GUARD} */"
} > "$FINAL_FILE"

# 3. Fire the Sideways Generator
if [ -f "./docsgen.sh" ]; then
    bash ./docsgen.sh
    if [ -d "docs" ]; then
        cp docs/docs.html ./index.html
    fi
    echo "Build success. Amalgamation: $FINAL_FILE, Docs: index.html"
fi
