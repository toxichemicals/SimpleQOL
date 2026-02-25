#include "simple_threads.h"
int didcalc = 0;
int secondsleft = 5;
void my_worker(int x, int y) {
    while(1) {
        if(didcalc != 1){
        printf("Calculating: %d + %d = %d\n", x, y, x + y);
        didcalc = 1;
        } else printf("Waiting %d seconds..\n", secondsleft); secondsleft = secondsleft - 1;
        secondsleep(1);
    }
}

int returnfortytwo() {
    secondsleep(0.1);
    return 42;
}

char *returnhiifone(int x) {
    if(x == 1) return "Hi!";
    return "Not 1... :(\n";
} // Fixed: added missing brace

char *returnyourstring(char *K) { // Changed char to char*
    return K;
}

int main() {
    spawnthread(my_worker, "10, 20", 0);
    spawnthread(returnfortytwo, "", 1);
    spawnthread(returnhiifone, "1", 2);
    spawnthread(returnyourstring, "\"Hello!\n\"", 3);

    secondsleep(5);

    int res = *(int *)getreturn(1);
    char *hi = (char *)getreturn(2);
    char *string = (char *)getreturn(3);
    
    printf("Results: %d, %s, %s\n", res, hi, string);
    printf("Killing thread 1.\n"); killthread(1);
    printf("Blowing the rest of the threads...\n");
    bombthreads();

    return 0;
}
