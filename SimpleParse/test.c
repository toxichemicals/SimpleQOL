#include "simple_parse.h"
#include <stdio.h>

void print_parsed(const char* label, Parsed p) {
    printf("[%s]\n", label);
    printf("  .byte:    %d\n", p.byte);
    printf("  .lbyte:   %d\n", p.lbyte);
    printf("  .line:    %d\n", p.line);
    printf("  .fullstr: \"%s\"\n", p.fullstr);
    printf("  .str:     \"%s\"\n", p.str);
    printf("  .con:     \"%s\"\n", p.con);
    printf("\n");
}

int main() {
    const char* script = 
        "// Init\n"                // Line 1
        "    myfunc(data);\n"      // Line 2 (4 spaces)
        "exit();";                 // Line 3

    printf("--- Simple_Parser Exhaustive Struct Test ---\n\n");

    // 1. parse()
    printf("[parse]\n  Result: %d\n\n", parse(script, "myfunc"));

    // 2. parseex()
    int val = 0;
    int items = parseex("code(500);", "code(%d);", &val);
    printf("[parseex]\n  Items: %d, Value: %d\n\n", items, val);

    // 3. advparse()
    Parsed p_adv = advparse(script, "myfunc");
    print_parsed("advparse Result", p_adv);

    // 4. parsecon()
    Parsed p_con = parsecon(script, "myfunc(%%)");
    print_parsed("parsecon Result", p_con);

    // 5. parseline()
    char buf[256];
    int ok = parseline(script, 2, buf, 256);
    printf("[parseline]\n  OK: %d, Content: \"%s\"\n\n", ok, buf);

    // 6. parselineadv()
    Parsed p_la = parselineadv(script, 2);
    print_parsed("parselineadv Result", p_la);

    // 7. parselinecon()
    Parsed p_lc = parselinecon(script, 2, "myfunc(%%)");
    print_parsed("parselinecon Result", p_lc);

    return 0;
}
