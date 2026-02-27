#!/bin/bash
Types_h="simple_types.h"
budp_h="simple_budp.first"
output="simple_budp.h"

# 1. Start Header Guard
echo "#ifndef SIMPLE_BUDP_UNIFIED_H" > $output
echo "#define SIMPLE_BUDP_UNIFIED_H" >> $output

# 2. Add SimpleTypes
cat "$Types_h" >> $output

# 3. Add the Multiplication Macro (Ensures suint256_mul exists)
echo "DEF_MUL(256, 8)" >> $output

# 4. Add BUDP Logic
cat "$budp_h" >> $output

# 5. Close Header Guard
echo "#endif // SIMPLE_BUDP_UNIFIED_H" >> $output

chmod +x $output
echo "Generated $output successfully."
