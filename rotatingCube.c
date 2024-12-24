#include <time.h>
#include <stdio.h>

void clearConsole() {
    printf("\033[H\033[J");
}

typedef struct {
    float x, y, z; 
} Point3D;

typedef struct {
    Point3D v1;  // First vertex
    Point3D v2;  // Second vertex
    Point3D v3;  // Third vertex
    Point3D v4;  // Fourth vertex
    char depthChar; // Char that will be used to differentiate the face of the cube
} Face;

int main() {

    struct timespec req;
    double rotatingVelocityX = 0;
    double rotatingVelocityY = 0;
    double rotatingVelocityZ = 0;

    // fazer o for loop infinito que vai fazer a animação

    int continuar = 1; 
    /*while (continuar)
    {
    clearConsole();
    req.tv_sec = 0;                 // Seconds
    req.tv_nsec = 200000000L;       // Nanoseconds (200 ms = 200,000,000 ns)

    // 



    nanosleep(&req, NULL);          // Pause execution for the specified time
    }*/

    return 0;
}