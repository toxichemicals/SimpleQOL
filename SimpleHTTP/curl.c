#define HTTP_CURL
#include "../QOL.h"
int main(void){
char *data = http_request("http://localhost:8080/");
printf("%s\n", data);
}