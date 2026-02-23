#!/bin/bash

SOURCE_DOCS="Alldocs.mside"
OUTPUT_DIR="docs"
TAILWIND_FILE="$OUTPUT_DIR/tailwind.js"
TAILWIND_URL="https://cdn.tailwindcss.com"
TITLE="QOL Library | Mark-Sideways"

if [ ! -f "$SOURCE_DOCS" ]; then
    echo "Error: $SOURCE_DOCS not found. Run build.sh first."
    exit 1
fi

mkdir -p "$OUTPUT_DIR"

if [ ! -f "$TAILWIND_FILE" ]; then
    curl -s "$TAILWIND_URL" -o "$TAILWIND_FILE" || wget -q "$TAILWIND_URL" -O "$TAILWIND_FILE"
fi

echo "Rendering Mark-Sideways (.mside)..."

{
    echo "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'>"
    echo "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    echo "<title>$TITLE</title>"
    echo "<script>$(cat "$TAILWIND_FILE")</script>"
    echo "<style>
        @import url('https://fonts.googleapis.com/css2?family=Fira+Code:wght@400;600&family=Inter:wght@400;700&display=swap');
        body { font-family: 'Inter', sans-serif; background-color: #0f172a; }
        .mside-tree { font-family: 'Fira Code', monospace; line-height: 1.4; }
        .mside-code { font-family: 'Fira Code', monospace; background: rgba(0,0,0,0.4); }
    </style>"
    echo "</head><body class='text-slate-300 p-8'>"
    echo "<div class='max-w-4xl mx-auto'>"
    echo "<header class='mb-12 border-b border-slate-700 pb-6'>"
    echo "<h1 class='text-4xl font-bold text-white tracking-tight'>$TITLE</h1>"
    echo "<p class='text-slate-500 mt-2 font-mono text-sm'>System Documentation // Generated $(date +%Y-%m-%d)</p>"
    echo "</header>"

    # Process line by line
    # We use sed for the complex regex substitutions first
    sed -E "
        s/@@@ (.*) @@@/<div class='bg-emerald-500\/10 border border-emerald-500\/20 text-emerald-400 px-4 py-2 rounded-md font-bold my-8 uppercase tracking-widest text-sm inline-block'>\1<\/div>/g;
        s/^# (.*)/<h1 class='text-3xl font-bold text-white mt-12 mb-4'>\1<\/h1>/g;
        s/^## (.*)/<h2 class='text-2xl font-semibold text-sky-400 mt-8 mb-3'>\1<\/h2>/g;
        s/^### (.*)/<h3 class='text-xl font-medium text-slate-200 mt-6 mb-2'>\1<\/h3>/g;
        s/\\\$\\\$\\\$ (.*) \\\$\\\$\\\$/<span class='bg-red-500\/20 border border-red-500\/50 text-red-400 px-2 py-1 rounded font-bold'>\1<\/span>/g;
        s/\\\$\\\$ (.*) \\\$\\\$/<span class='text-amber-400 border-b border-amber-400\/30 font-semibold'>\1<\/span>/g;
        s/\\\$ ([^$]+) \\\$/<code class='bg-slate-800 text-pink-400 px-1 rounded'>\1<\/code>/g;
        s/&& ([^&]+) &&/<span class='bg-yellow-400\/10 text-yellow-200 px-1 underline decoration-yellow-500\/50'>\1<\/span>/g;
        s/\\\\\\\\([^\\\\]+)\\\\\\\\/<span class='underline decoration-slate-500'>\1<\/span>/g;
        s/\/\/([^\/]+)\/\//<span class='italic text-slate-400'>\1<\/em>/g;
        s/\*\*([^*]+)\*\*/<strong class='text-white font-bold'>\1<\/strong>/g;
        s/\^ ([^^]+) \^/<p class='text-xs text-slate-500 mt-1 italic'>\1<\/p>/g;
        s/-->/<span class='text-orange-400 font-bold ml-2'>\&rdsh;<\/span>/g;
        s/->/<span class='text-orange-500 font-bold'>\&rarr;<\/span>/g;
    " "$SOURCE_DOCS" | awk '
    BEGIN { in_code=0; }
    
    # Handle Tree Lines (preserve leading spaces)
    /^\|/ {
        print "<div class=\"mside-tree text-slate-500 whitespace-pre\">" $0 "</div>";
        next;
    }

    # Handle Code Blocks
    /^\|\|/ {
        if (!in_code) { 
            print "<div class=\"mside-code border border-slate-700 rounded-lg p-6 my-6 text-sm text-sky-300 shadow-2xl\"><pre>"; 
            in_code=1; 
        } else { 
            print "</pre></div>"; 
            in_code=0; 
        }
        next;
    }

    # Content inside/outside code
    {
        if (in_code) {
            # Escape HTML tags inside code blocks
            gsub(/</, "&lt;"); gsub(/>/, "&gt;");
            print $0;
        } else {
            if (length($0) > 0 && $0 !~ /<h/ && $0 !~ /<div/ && $0 !~ /---/) {
                print "<p class=\"mb-4\">" $0 "</p>";
            } else {
                print $0;
            }
        }
    }
    '
    
    echo "</div><footer class='mt-20 border-t border-slate-800 p-8 text-center text-slate-600 text-xs tracking-widest uppercase'>"
    echo "Built with Mark-Sideways & Tailwind CSS"
    echo "</footer></body></html>"
} > "$OUTPUT_DIR/docs.html"

echo "Success: ./$OUTPUT_DIR/docs.html"
