#!/bin/bash
Types_h="simple_types.fst"
budp_h="simple_budp.fst"
tacl_c="tweetnacl.c"
output="simple_budp.h"
tmp_file="combined.tmp"

# 1. Start Header Guard
echo "#ifndef SIMPLE_BUDP_UNIFIED_H" > $output
echo "#define SIMPLE_BUDP_UNIFIED_H" >> $output

# 2. Add TweetNaCl implementation wrapper
# The implementation will only be included if this macro is defined
echo "#ifdef BUDP_IMPLEMENTATION" >> $output
cat "$tacl_c" >> $output
echo "#endif // BUDP_IMPLEMENTATION" >> $output

# 3. Combine your library files into the temp buffer
cat "$Types_h" "$budp_h" > "$tmp_file"

# 4. Global Literal Replacement
sed -i 's/suint/budp_suint/g' "$tmp_file"
sed -i 's/sint/budp_sint/g' "$tmp_file"
sed -i 's/sfloat/budp_sfloat/g' "$tmp_file"

# 5. Prefix Macros and internal helpers
sed -i -E 's/\bDEF_(STRUCTS|SHIFT|PRINTERS|ADD|MUL)\b/budp_DEF_\1/g' "$tmp_file"
sed -i -E 's/\b(_internal_|_nf_|_f_)/_budp\1/g' "$tmp_file"
sed -i -E 's/\bslib(nf)?print\b/budp_\1print/g' "$tmp_file"

# 6. Fix accidental double-prefixing
sed -i 's/budp_budp_/budp_/g' "$tmp_file"

# 7. Append implementation logic to final output
# This ensures declarations exist even without BUDP_IMPLEMENTATION
echo "#ifdef BUDP_IMPLEMENTATION" >> $output
cat "$tmp_file" >> $output
echo "#endif // BUDP_IMPLEMENTATION" >> $output

rm "$tmp_file"

# 8. Close Guard
echo "#endif // SIMPLE_BUDP_UNIFIED_H" >> $output

chmod +x $output
echo "Generated $output with embedded implementation."
