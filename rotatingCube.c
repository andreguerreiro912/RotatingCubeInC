#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

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

void getCenterScreen(int center[2]) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    // Retrieve console screen buffer info
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;  // Width
        int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1; // Height

        // Calculate center
        center[0] = consoleWidth / 2; // centerX
        center[1] = consoleHeight / 2; // centerY
    } else {
        center[0] = 0;
        center[1] = 0; // Default values if the console size can't be retrieved
    }
}

void moveCursor(int x, int y) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {x, y};
    SetConsoleCursorPosition(hConsole, pos);
#else
    printf("\033[%d;%dH", y + 1, x + 1); // ANSI escape sequence for positioning (1-based)
#endif
}
/*
// Function to print a Point3D, ignoring the z-coordinate
void printPoint3D(Point3D point, int center[2], char symbol) {
    int translatedX = center[0] + point.x; // Translate relative x to absolute
    int translatedY = center[1] - point.y; // Translate relative y to absolute (Y-axis inverted)
    moveCursor(translatedX, translatedY); // Move the cursor
    putchar(symbol); // Print the symbol
    fflush(stdout); // Ensure the output is displayed immediately
}
*/


// Function to draw a line between two points (using Bresenham's line algorithm)
void drawLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        moveCursor(x1, y1);
        putchar('*');  // Or any character you want to use for the line

        if (x1 == x2 && y1 == y2)
            break;

        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }

    fflush(stdout);
}


// Function to print a face of the cube
void printFace(Face face, int center[2]) {
    // Project 3D points to 2D screen coordinates
    int x1 = center[0] + (int)round(face.v1.x * 2);
    int y1 = center[1] - (int)round(face.v1.y);

    int x2 = center[0] + (int)round(face.v2.x * 2);
    int y2 = center[1] - (int)round(face.v2.y);

    int x3 = center[0] + (int)round(face.v3.x * 2);
    int y3 = center[1] - (int)round(face.v3.y);

    int x4 = center[0] + (int)round(face.v4.x * 2);
    int y4 = center[1] - (int)round(face.v4.y);
    /*
    // Draw the edges of the face (quadrilateral)
    moveCursor(x1, y1);
    putchar(face.depthChar);
    moveCursor(x2, y2);
    putchar(face.depthChar);
    moveCursor(x3, y3);
    putchar(face.depthChar);
    moveCursor(x4, y4);
    putchar(face.depthChar);
    */
    // Optionally, draw lines between the points (edges)
    // You can use a function like drawLine() for better visual representation.
    drawLine(x1, y1, x2, y2);
    drawLine(x2, y2, x3, y3);
    drawLine(x3, y3, x4, y4);
    drawLine(x4, y4, x1, y1);
    
    fflush(stdout); // Ensure output is shown immediately
}



void printPoint3D(Point3D point, int center[2], char symbol) {
    // Round the floating-point coordinates to the nearest integers
    // Double the height (y-axis) to stretch it
    int translatedX = center[0] + (int)round(point.x * 2); // Round and translate x
    int translatedY = center[1] - (int)round(point.y); // Round and stretch y (Y-axis inverted)

    // Move the cursor to the calculated position
    moveCursor(translatedX, translatedY);

    // Print the symbol at the calculated position
    putchar(symbol);

    // Ensure the output is displayed immediately
    fflush(stdout);
}

void printEmptyPoint(Point3D point, int center[2]) { 
    // Calculate the position based on the point's coordinates and center
    int translatedX = center[0] + (int)round(point.x * 2);  // Adjust for x-axis
    int translatedY = center[1] - (int)round(point.y);      // Adjust for y-axis (inverted)

    // Move the cursor to the calculated position
    moveCursor(translatedX, translatedY);

    // Print a blank space to erase the previous symbol
    putchar(' ');

    // Ensure the output is displayed immediately
    fflush(stdout);
}

void clearConsole() {
    printf("\033[H\033[J");
}

/*
Point3D rotatePointXY(Point3D point, Point3D center, double theta) {
    // Translate point to origin
    double translatedX = point.x - center.x;
    double translatedY = point.y - center.y;

    // Apply 2D rotation formula
    double rotatedX = translatedX * cos(theta) - translatedY * sin(theta);
    double rotatedY = translatedX * sin(theta) + translatedY * cos(theta);

    // Translate back to original position
    Point3D result;
    result.x = rotatedX + center.x;
    result.y = rotatedY + center.y;
    result.z = point.z; // Z-coordinate remains unchanged
    return result;
}*/



void rotatePointXY(Point3D *point, Point3D center, double theta) {
    // Translate point to origin
    double translatedX = point->x - center.x;
    double translatedY = point->y - center.y;

    // Apply 2D rotation formula
    double rotatedX = translatedX * cos(theta) - translatedY * sin(theta);
    double rotatedY = translatedX * sin(theta) + translatedY * cos(theta);

    // Translate back to original position
    point->x = rotatedX + center.x;
    point->y = rotatedY + center.y;
    // Z-coordinate remains unchanged
    // No need to modify point->z as it stays the same
}

void rotatePointXZ(Point3D *point, Point3D center, double theta) {
    // Translate point to origin
    double translatedX = point->x - center.x;
    double translatedZ = point->z - center.z;

    // Apply rotation formula around the Y-axis (XZ plane)
    double rotatedX = translatedX * cos(theta) - translatedZ * sin(theta);
    double rotatedZ = translatedX * sin(theta) + translatedZ * cos(theta);

    // Translate back to original position
    point->x = rotatedX + center.x;
    point->z = rotatedZ + center.z;
    // Y-coordinate remains unchanged
}

void rotatePointYZ(Point3D *point, Point3D center, double theta) {
    // Translate point to origin
    double translatedY = point->y - center.y;
    double translatedZ = point->z - center.z;

    // Apply rotation formula around the X-axis (YZ plane)
    double rotatedY = translatedY * cos(theta) - translatedZ * sin(theta);
    double rotatedZ = translatedY * sin(theta) + translatedZ * cos(theta);

    // Translate back to original position
    point->y = rotatedY + center.y;
    point->z = rotatedZ + center.z;
    // X-coordinate remains unchanged
}



void rotateFaceXY(Face *face, Point3D center, double theta){
    rotatePointXY(&face->v1, center,theta);
    rotatePointXY(&face->v2, center,theta);
    rotatePointXY(&face->v3, center,theta);
    rotatePointXY(&face->v4, center,theta);
}

void rotateFaceXZ(Face *face, Point3D center, double theta){
    rotatePointXZ(&face->v1, center,theta);
    rotatePointXZ(&face->v2, center,theta);
    rotatePointXZ(&face->v3, center,theta);
    rotatePointXZ(&face->v4, center,theta);
}

void rotateFaceYZ(Face *face, Point3D center, double theta){
    rotatePointYZ(&face->v1, center,theta);
    rotatePointYZ(&face->v2, center,theta);
    rotatePointYZ(&face->v3, center,theta);
    rotatePointYZ(&face->v4, center,theta);
}

int main() {
    // Define all the constants in the beggining 
    struct timespec req;
    double rotatingVelocityX = 0;
    double rotatingVelocityY = 0;
    double rotatingVelocityZ = 0;

    // calculate the center of the screen to render the point (0,0,0) *close* to the center 
    int center[2] = {0, 0};

    getCenterScreen(center); //refreshes the center of the screen

    Point3D v1 = {-5.0, -5.0, 0.0};
    Point3D v2 = {5.0, -5.0, 0.0};
    Point3D v3 = {5.0, 5.0, 0.0};
    Point3D v4 = {-5.0, 5.0, 0.0};

    // Create a face with these vertices
    Face face = {v1, v2, v3, v4, '#'}; // Use '#' to differentiate the face

    clearConsole();
    Point3D pontoCentro = {0.0, 0.0, 0.0};
    //Point3D point2 = {5, 3, 2};  // Point shifted in all directions
    double angle = M_PI / 100;

    // fazer o for loop infinito que vai fazer a animação

    int continuar = 1; 
    while (continuar)
    {
    clearConsole();
    getCenterScreen(center);
    printPoint3D(pontoCentro, center, 'o');
    //printPoint3D(point2, center, '*');
    //rotatePointXY(&point2, pontoCentro, angle);
    //printf("x : %d\ny : %d\nz : %d\n\n", point2.x,point2.y,point2.z);

    rotateFaceXY(&face, pontoCentro, angle);
    rotateFaceXZ(&face, pontoCentro, angle);
    rotateFaceXZ(&face, pontoCentro, angle);
    printFace(face, center);

    req.tv_sec = 0;                 // Seconds
    req.tv_nsec = 100000000L;       // Nanoseconds (100 ms = 100,000,000 ns)
    nanosleep(&req, NULL);          // Pause execution for the specified time
    }

    return 0;
}