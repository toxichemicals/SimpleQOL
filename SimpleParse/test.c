#include "simple_parse.h"
#include <stdio.h>

int main() {
    const char* source_code = 
        "// My Kool Script\n"
        "move_player(10, 20);\n"
        "spawn_enemy(slime);\n"
        "exit_game();";

    printf("--- Testing Simple_Parser ---\n");

    // Test parsecon
    Parsed p = parsecon(source_code, "spawn_enemy(%%)");
    if (p.byte != -1) {
        printf("Found: %s\n", p.str);      // "spawn_enemy()"
        printf("Content: %s\n", p.con);    // "slime"
        printf("Line: %d\n", p.line);      // 3
        printf("Full content: %s\n", p.fullstr);
    }

    // Test parseex
    int x, y;
    if (parseex("move_player(10, 20);", "move_player(%d, %d);", &x, &y)) {
        printf("X: %d, Y: %d\n", x, y);
    }

    return 0;
}
