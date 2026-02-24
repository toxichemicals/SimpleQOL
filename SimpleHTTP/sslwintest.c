#define HTTP_CURL // Enable Client features in simple_http.h
#include "simple_http.h"

int main() {
    printf("Connecting to toxichemicals.loseyourip.com...\n");

    // http_request returns a pointer to an internal static buffer
    char* response = http_request("https://toxichemicals.loseyourip.com");

    if (response != NULL && strlen(response) > 0) {
        printf("--- Content Start ---\n");
        printf("%s\n", response);
        printf("--- Content End ---\n");
        printf("\nSuccess!\n");
    } else {
        fprintf(stderr, "Failed to retrieve content or received empty response.\n");
        return 1;
    }

    return 0;
}
