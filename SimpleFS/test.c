#include "simple_fs.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    const char* test_dir = "test_data";
    const char* file1 = "test_data/notes.txt";
    const char* file2 = "test_data/data.bin";
    const char* sub_dir = "test_data/logs";

    printf("--- simple_fs.h Piped List Test ---\n\n");

    // 1. Setup structure
    fs_mkdir(test_dir);
    fs_mkdir(sub_dir);
    fs_write(file1, "Some text notes");
    fs_write(file2, "Binary payload");

    // 2. Test fs_listdir (Heap Version)
    printf("Fetching directory contents...\n");
    char* list = fs_listdir(test_dir);
    
    if (list) {
        printf("Result String: \"%s\"\n", list);
        
        // Example of how you might process the pipes
        printf("\nParsed View:\n");
        char* token = strtok(list, "|");
        while (token != NULL) {
            printf("  -> %s\n", token);
            token = strtok(NULL, "|");
        }

        // IMPORTANT: Free the memory allocated by fs_listdir
        free(list);
    } else {
        printf("Error: Could not list directory.\n");
    }

    // 3. Cleanup
    printf("\nCleaning up everything...\n");
    if (fs_dirremove(test_dir)) {
        printf("Success: Directory tree removed.\n");
    }

    return 0;
}
