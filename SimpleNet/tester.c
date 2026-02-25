#include "simple_net.h"
#include <stdio.h>

#ifdef _WIN32
    #include <windows.h>
    #define sleep_ms(x) Sleep(x)
    unsigned __stdcall server_thread(void* arg);
    unsigned __stdcall client_thread(void* arg);
#else
    #include <pthread.h>
    #include <unistd.h>
    #define sleep_ms(x) usleep((x) * 1000)
    void* server_thread(void* arg);
    void* client_thread(void* arg);
#endif

#define SERVER_ID 0
#define CLIENT_START_ID 1
#define NUM_CLIENTS 5
#define TEST_PORT 8080

int main() {
    net_init(); // Initialize global mutex and networking
    printf("--- Starting Thread Safety Test (Max IDs: %d) ---\n", MAX_IDS);

    // 1. Start Server Socket
    if (!openudp("127.0.0.1", TEST_PORT, SERVER_ID)) {
        printf("Failed to open server socket.\n");
        return 1;
    }
    printf("Server listening on port %d (ID: %d)\n", TEST_PORT, SERVER_ID);

    // 2. Launch Server Thread
#ifdef _WIN32
    _beginthreadex(NULL, 0, server_thread, NULL, 0, NULL);
#else
    pthread_t serv_tid;
    pthread_create(&serv_tid, NULL, server_thread, NULL);
#endif

    // 3. Launch Multiple Client Threads
#ifdef _WIN32
    HANDLE clients[NUM_CLIENTS];
    for (int i = 0; i < NUM_CLIENTS; i++) {
        int* id = malloc(sizeof(int));
        *id = CLIENT_START_ID + i;
        clients[i] = (HANDLE)_beginthreadex(NULL, 0, client_thread, id, 0, NULL);
    }
    WaitForMultipleObjects(NUM_CLIENTS, clients, TRUE, INFINITE);
#else
    pthread_t clients[NUM_CLIENTS];
    for (long i = 0; i < NUM_CLIENTS; i++) {
        pthread_create(&clients[i], NULL, client_thread, (void*)(i + CLIENT_START_ID));
    }
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(clients[i], NULL);
    }
#endif

    printf("Test complete. Cleaning up...\n");
    closesocket_id(SERVER_ID);
    return 0;
}

/* --- Server Logic --- */
#ifdef _WIN32
unsigned __stdcall server_thread(void* arg) {
#else
void* server_thread(void* arg) {
#endif
    char buf[MAX_NET_BUF];
    printf("[Server] Thread started.\n");
    
    int messages_received = 0;
    while (messages_received < NUM_CLIENTS) {
        // Using the new thread-safe receive function
        int bytes = receivefrom_safe(SERVER_ID, buf, MAX_NET_BUF);
        if (bytes > 0) {
            ClientInfo info = getclientinfo(SERVER_ID);
            printf("[Server] Received: '%s' from %s:%d\n", buf, info.host, info.port);
            messages_received++;
        }
        sleep_ms(10);
    }
    return 0;
}

/* --- Client Logic --- */
#ifdef _WIN32
unsigned __stdcall client_thread(void* arg) {
    int id = *(int*)arg;
    free(arg);
#else
void* client_thread(void* arg) {
    int id = (int)(long)arg;
#endif
    char msg[64];
    sprintf(msg, "Hello from Client ID %d", id);
    
    sleep_ms(id * 50); // Stagger starts slightly
    
    // senddata handles its own locking internally
    if (senddata(id, "127.0.0.1", TEST_PORT, msg)) {
        printf("[Client %d] Sent message successfully.\n", id);
    } else {
        printf("[Client %d] Failed to send.\n", id);
    }

    closesocket_id(id);
    return 0;
}
