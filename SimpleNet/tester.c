#include "simple_net.h"

int main() {
    // 1. Test net_init()
    net_init();
    printf("Network initialized.\n");

    // 2. Test openudp() and opentcp()
    // Opening a UDP socket on port 5000 (Slot 0)
    if (openudp("127.0.0.1", 5000, 0)) {
        printf("UDP socket opened on port 5000 (ID 0).\n");
    }

    // Opening a TCP listener on port 6000 (Slot 1)
    if (opentcp("127.0.0.1", 6000, 1)) {
        printf("TCP listener opened on port 6000 (ID 1).\n");
    }

    // 3. Test connecttcp()
    // Try to connect Slot 2 to the listener on Slot 1
    if (connecttcp(2, "127.0.0.1", 6000)) {
        printf("TCP client (ID 2) connecting to port 6000...\n");
    }

    // 4. Test isconnected()
    if (isconnected(2)) {
        printf("Slot 2 is connected.\n");
    }

    // 5. Test senddata()
    // UDP send (ID 0 sends to itself)
    senddata(0, "127.0.0.1", 5000, "Hello UDP");
    
    // TCP send (Client ID 2 sends to Listener ID 1)
    senddata(2, "127.0.0.1", 6000, "Hello TCP");

    // 6. Test receivefrom() and getclientinfo()
    // Give the OS a moment to process the local packets
#ifdef _WIN32
    Sleep(100);
#else
    usleep(100000);
#endif

    char* udp_data = receivefrom(0);
    if (udp_data) {
        ClientInfo info = getclientinfo(0);
        printf("UDP Received: %s from %s:%d\n", udp_data, info.host, info.port);
    }

    // 7. Test receivefrompro()
    // This will accept the connection on ID 1 and read the data
    Packet p = receivefrompro(1);
    if (p.data) {
        printf("TCP Packet Received: %s from %s:%d\n", p.data, p.host, p.port);
    }

    // 8. Test closesocket_id()
    closesocket_id(0);
    closesocket_id(1);
    closesocket_id(2);
    printf("All sockets closed. Test complete.\n");

    return 0;
}
