#!/bin/bash
sed -i 's/\[cite[: ]*[^]]*\]//g' Alldocs.mside
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
        body { font-family: 'Inter', sans-serif; background-color: #0f172a; color: #cbd5e1; }
        .mside-tree { font-family: 'Fira Code', monospace; line-height: 1.5; color: #94a3b8; }
        .mside-code { font-family: 'Fira Code', monospace; background: #1e293b; border-left: 4px solid #38bdf8; }
        pre { margin: 0; white-space: pre; }
        .cite-tag { font-size: 0.7rem; color: #6366f1; vertical-align: super; margin-left: 2px; }
    </style>"
    echo "</head><body class='antialiased'>"
    echo "<div class='max-w-4xl mx-auto px-8 py-16'>"
    echo "<header class='mb-20 border-b border-slate-800 pb-10 text-center'>"
    echo "<h1 class='text-5xl font-extrabold text-white tracking-tighter mb-4'>$TITLE</h1>"
    echo "<p class='text-slate-500 font-mono text-xs uppercase tracking-[0.3em]'>System Documentation // $(date +%Y-%m-%d)</p>"
    echo "</header>"

    awk '
    function escape(str) {
        gsub(/&/, "\&amp;", str); gsub(/</, "\&lt;", str); gsub(/>/, "\&gt;", str);
        return str;
    }

    function process_inline(line) {
        # 1. Handle Headers (Block-level identification)
        if (line ~ /^### /) { sub(/^### /, "", line); return "<h3 class=\"text-xl font-bold text-slate-200 mt-10 mb-2\">" line "</h3>"; }
        if (line ~ /^## /) { sub(/^## /, "", line); return "<h2 class=\"text-2xl font-semibold text-sky-400 mt-12 mb-4\">" line "</h2>"; }
        if (line ~ /^# /) { sub(/^# /, "", line); return "<h1 class=\"text-4xl font-black text-white mt-20 mb-8 text-center border-b border-slate-800 pb-4\">" line "</h1>"; }

        # 2. Simple Replacements (No capture groups to avoid \1 issues)
        gsub(/@@@ /, "<div class=\"bg-emerald-500/10 border border-emerald-500/20 text-emerald-400 px-4 py-2 rounded font-bold my-8 inline-block uppercase tracking-widest text-xs\">", line);
        gsub(/ @@@/, "</div>", line);
        
        gsub(/\$\$\$ /, "<div class=\"bg-red-500/20 border border-red-500/50 text-red-400 px-6 py-4 rounded-lg font-black my-8 text-center uppercase tracking-widest italic\">", line);
        gsub(/ \$\$\$/, "</div>", line);

        gsub(/\$\$ /, "<span class=\"bg-amber-500/10 text-amber-400 border border-amber-500/30 px-2 py-1 rounded text-sm font-bold\">", line);
        gsub(/ \$\$/, "</span>", line);

        gsub(/&& /, "<span class=\"bg-yellow-400/10 text-yellow-200 px-1 underline decoration-yellow-500/50\">", line);
        gsub(/ &&/, "</span>", line);

        # 3. Handle Underlines (\\\\)
        gsub(/\\\\ /, "<span class=\"underline decoration-slate-600\">", line);
        gsub(/ \\\\/, "</span>", line);

        # 4. Handle Code Spans ($ ... $)
        gsub(/\$ /, "<code class=\"bg-slate-800 text-pink-400 px-1.5 py-0.5 rounded text-sm font-mono\">", line);
        gsub(/ \$/, "</code>", line);

        # 5. Handle Italics (// ... //)
        gsub(/\/\/ /, "<span class=\"italic text-slate-400\">", line);
        gsub(/ \/\//, "</span>", line);

        # 6. Arrows and Citations
        gsub(/-->/, "<span class=\"text-orange-400 font-bold ml-2\">\&rdsh;</span>", line);
        gsub(/->/, "<span class=\"text-orange-500 font-bold ml-1\">\&rarr;</span>", line);
        gsub(/\+)\]/, "<span class=\"cite-tag\">[\\1]</span>", line);
        
        # 7. Footnotes/Smalltext
        gsub(/\^ /, "<p class=\"text-xs text-slate-500 mt-1 italic tracking-tight\">", line);
        gsub(/ \^/, "</p>", line);

        return line;
    }

    BEGIN { in_code = 0; }

    # Code Blocks
    /^\|\|/ {
        if (!in_code) {
            print "<div class=\"mside-code rounded-xl p-6 my-8 text-sm text-sky-300 shadow-2xl\"><pre><code>";
            in_code = 1;
        } else {
            print "</code></pre></div>";
            in_code = 0;
        }
        next;
    }

    # Tree Structure
    /^\|/ {
        if (!in_code) {
            print "<div class=\"mside-tree whitespace-pre\">" process_inline($0) "</div>";
            next;
        }
    }

    {
        if (in_code) {
            print escape($0);
        } else {
            if ($0 ~ /^[[:space:]]*$/) { next; }
            if ($0 == "---") { print "<hr class=\"my-20 border-slate-800\"/>"; next; }
            
            content = process_inline($0);
            if (content ~ /^<(h1|h2|h3|div|p|hr)/) {
                print content;
            } else {
                print "<p class=\"mb-6 leading-relaxed text-slate-300\">" content "</p>";
            }
        }
    }
    ' "$SOURCE_DOCS"

    echo "</div><footer class='mt-32 border-t border-slate-900 p-16 text-center text-slate-600 text-[10px] tracking-[0.5em] uppercase'>"
    echo "Built with Mark-Sideways & Tailwind CSS"
    echo "</footer></body></html>"
} > "$OUTPUT_DIR/docs.html"

echo "Success: ./$OUTPUT_DIR/docs.html"
