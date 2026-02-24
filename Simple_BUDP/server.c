#include "simple_budp.h"

int main() {
    // 1. Setup identity and networking
    binit("SERVER_SECRET_SEED");
    if (!bopen(0, 8080)) {
        printf("Error: Could not bind to port 8080.\n");
        return 1;
    }

    // 2. Configure features
    enableheartbeat(0);
    // NoE(0); // Uncomment to test global plaintext mode
    
    printf("--- BUDP SERVER START ---\n");
    printf("Public ID: %llu\n", bget_my_public_id(0));

    while (1) {
        char* msg = bupdate(0);

        if (msg) {
            if (strcmp(msg, "CONNECTED") == 0) {
                printf("[!] Client connected successfully.\n");
            } 
            else if (strcmp(msg, "DEAD") == 0) {
                printf("[!] Connection lost (Timeout).\n");
                breset(0);
            } 
            else {
                printf("Client Sent: %s\n", msg);
                bsend(0, "Message Received.");
            }
        }
        b_sleep(1);
    }
    return 0;
}