#include <time.h>
#include <stdio.h>

// Function to clear the screen
void clearConsole() {
    printf("\033[H\033[J");
}

int main() {

    struct timespec req;

    // fazer o for loop infinito que vai fazer a animação

    int continuar = 1; 
    while (continuar)
    {
    clearConsole();
    req.tv_sec = 0;                 // Seconds
    req.tv_nsec = 200000000L;       // Nanoseconds (200 ms = 200,000,000 ns)


    // 



    nanosleep(&req, NULL);          // Pause execution for the specified time
    }

    return 0;
}