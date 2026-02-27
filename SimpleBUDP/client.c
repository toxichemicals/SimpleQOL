#include "simple_budp.h"

int main() {
    binit("CLIENT_SECRET_SEED");
    bopen(0, 0); 
    enableheartbeat(0);
    
    UseE(0);      
    bforceE(0);   

    printf("--- BUDP CLIENT START ---\n");
    bconnect(0, "127.0.0.1", 8080);

    int frames = 0;
    int connected = 0;

    while (1) {
        char* msg = bupdate(0);

        if (msg) {
            if (strcmp(msg, "CONNECTED") == 0) {
                printf("[!] Handshake complete. Tunnel is secure.\n");
                connected = 1;
                bsend(0, "Hello Server!");
            } 
            else if (strcmp(msg, "DEAD") == 0) {
                printf("[!] Session ended.\n");
                break;
            }
            else {
                printf("Server Replied: %s\n", msg);
            }
        }

        // Logic fix: If not connected yet, retry connection every 2 seconds (2000 ms)
        if (!connected && ++frames % 2000 == 0) {
            printf("Retrying connection...\n");
            bconnect(0, "127.0.0.1", 8080);
        }

        // Send keep-alive every 5 seconds if connected
        if (connected && ++frames % 5000 == 0) {
            bsend(0, "Still here!");
        }

        b_sleep(1); // Ensure this is 1ms
    }
    return 0;
}
