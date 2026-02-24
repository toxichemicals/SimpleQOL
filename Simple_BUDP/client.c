#include "simple_budp.h"

int main() {
    // 1. Setup
    binit("CLIENT_SECRET_SEED");
    bopen(0, 0); // Open random port
    enableheartbeat(0);
    
    // 2. Choose Mode (Comment/Uncomment as needed)
    UseE(0);      // We want encryption
    bforceE(0);   // We require it

    printf("--- BUDP CLIENT START ---\n");
    printf("Connecting to 127.0.0.1:8080...\n");
    bconnect(0, "127.0.0.1", 8080);

    int frames = 0;
    while (1) {
        char* msg = bupdate(0);

        if (msg) {
            if (strcmp(msg, "CONNECTED") == 0) {
                printf("[!] Handshake complete. Tunnel is secure.\n");
                bsend(0, "Hello Server!");
            } 
            else if (strncmp(msg, "LOCAL_ERROR", 11) == 0) {
                printf("[X] Connection Blocked: %s\n", msg);
                break;
            }
            else if (strcmp(msg, "DEAD") == 0) {
                printf("[!] Session ended.\n");
                break;
            }
            else {
                printf("Server Replied: %s\n", msg);
            }
        }

        // Send a message every 5 seconds just to test flow
        if (bcconn(0) && ++frames % 5000 == 0) {
            bsend(0, "Still here!");
        }

        b_sleep(1);
    }

    return 0;
}