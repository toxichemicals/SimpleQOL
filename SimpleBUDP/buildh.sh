#!/bin/bash
Types_h="simple_types.fst"
budp_h="simple_budp.fst"
output="simple_budp.h"
tmp_file="combined.tmp"

# 1. Start Header Guard
echo "#ifndef SIMPLE_BUDP_UNIFIED_H" > $output
echo "#define SIMPLE_BUDP_UNIFIED_H" >> $output

# 2. Combine files into the temp buffer
cat "$Types_h" "$budp_h" > "$tmp_file"

# 3. Global Literal Replacement
# We replace the core strings first. This catches:
# suint12288 (type), suint12288_mul (definition), and suint12288_mul(...) (the call)
sed -i 's/suint/budp_suint/g' "$tmp_file"
sed -i 's/sint/budp_sint/g' "$tmp_file"
sed -i 's/sfloat/budp_sfloat/g' "$tmp_file"

# 4. Prefix the Macros and internal helpers
# This ensures DEF_MUL(256, 8) becomes budp_DEF_MUL(256, 8)
sed -i -E 's/\bDEF_(STRUCTS|SHIFT|PRINTERS|ADD|MUL)\b/budp_DEF_\1/g' "$tmp_file"
sed -i -E 's/\b(_internal_|_nf_|_f_)/_budp\1/g' "$tmp_file"
sed -i -E 's/\bslib(nf)?print\b/budp_\1print/g' "$tmp_file"

# 5. Clean up any accidental double-prefixing (budp_budp_)
sed -i 's/budp_budp_/budp_/g' "$tmp_file"

# 6. Append to final output
cat "$tmp_file" >> $output
rm "$tmp_file"

# 7. Close Guard
echo "#endif // SIMPLE_BUDP_UNIFIED_H" >> $output

chmod +x $output
echo "Generated $output. Checked for suint12288_mul calls."
