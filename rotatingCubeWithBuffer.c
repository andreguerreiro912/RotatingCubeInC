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

void getTerminalDimensions(int *width, int *height) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        perror("GetConsoleScreenBufferInfo");
        exit(EXIT_FAILURE);
    }
    *width = csbi.dwSize.X;
    *height = csbi.dwSize.Y;
}

// Function to initialize the buffer
void initializeBuffer(char **buffer, int width, int height) {
    *buffer = malloc(height * width * sizeof(char));  // Allocate memory for the buffer
    if (*buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Initialize buffer with empty spaces
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            (*buffer)[i * width + j] = ' ';  // Set each cell to a space character
        }
    }
}

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
void drawLine(int x1, int y1, int x2, int y2, char faceChar) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        moveCursor(x1, y1);
        putchar(faceChar);  // Or any character you want to use for the line

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

int pointInFace(Point3D point, Face face) {
    // Calculate vectors for each edge of the face
    Point3D v1v2 = {face.v2.x - face.v1.x, face.v2.y - face.v1.y, face.v2.z - face.v1.z};
    Point3D v2v3 = {face.v3.x - face.v2.x, face.v3.y - face.v2.y, face.v3.z - face.v2.z};
    Point3D v3v4 = {face.v4.x - face.v3.x, face.v4.y - face.v3.y, face.v4.z - face.v3.z};
    Point3D v4v1 = {face.v1.x - face.v4.x, face.v1.y - face.v4.y, face.v1.z - face.v4.z};
    
    // Vectors from the point to each vertex of the face
    Point3D p1 = {point.x - face.v1.x, point.y - face.v1.y, point.z - face.v1.z};
    Point3D p2 = {point.x - face.v2.x, point.y - face.v2.y, point.z - face.v2.z};
    Point3D p3 = {point.x - face.v3.x, point.y - face.v3.y, point.z - face.v3.z};
    Point3D p4 = {point.x - face.v4.x, point.y - face.v4.y, point.z - face.v4.z};

    // Cross products to determine if the point is inside the face
    float cross1 = v1v2.x * p1.y - v1v2.y * p1.x;
    float cross2 = v2v3.x * p2.y - v2v3.y * p2.x;
    float cross3 = v3v4.x * p3.y - v3v4.y * p3.x;
    float cross4 = v4v1.x * p4.y - v4v1.y * p4.x;

    // Check if all cross products have the same sign (or are zero)
    if ((cross1 > 0 && cross2 > 0 && cross3 > 0 && cross4 > 0) ||
        (cross1 < 0 && cross2 < 0 && cross3 < 0 && cross4 < 0) ||
        (cross1 == 0 && cross2 == 0 && cross3 == 0 && cross4 == 0)) {
        return 1; // Point is inside the face
    } else {
        return 0; // Point is outside the face
    }
}

/*
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
    *//*
    // Optionally, draw lines between the points (edges)
    // You can use a function like drawLine() for better visual representation.
    drawLine(x1, y1, x2, y2, face.depthChar);
    drawLine(x2, y2, x3, y3, face.depthChar);
    drawLine(x3, y3, x4, y4, face.depthChar);
    drawLine(x4, y4, x1, y1, face.depthChar);
    
    fflush(stdout); // Ensure output is shown immediately
}*/


//Print Face to the Console
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

    // Get the bounding box of the face (min/max x and y)
    int minX = fmin(fmin(x1, x2), fmin(x3, x4));
    int maxX = fmax(fmax(x1, x2), fmax(x3, x4));
    int minY = fmin(fmin(y1, y2), fmin(y3, y4));
    int maxY = fmax(fmax(y1, y2), fmax(y3, y4));

    // Iterate over the bounding box and fill the face
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            // Check if the point (x, y) is inside the face using pointInFace
            Point3D point = {(float)(x - center[0]) / 2.0f, (float)(center[1] - y), 0.0f};
            if (pointInFace(point, face)) {
                moveCursor(x, y);
                putchar(face.depthChar);  // Fill the face with the character
            }
        }
    }

    fflush(stdout);  // Ensure output is shown immediately
}

//Print Face to Char Buffer instead of printing to console (for animation)
void printFaceToBuffer(char *buffer, int width, int height, Face face, int center[2]) {
    // Project 3D points to 2D screen coordinates
    int x1 = center[0] + (int)round(face.v1.x * 2);
    int y1 = center[1] - (int)round(face.v1.y);

    int x2 = center[0] + (int)round(face.v2.x * 2);
    int y2 = center[1] - (int)round(face.v2.y);

    int x3 = center[0] + (int)round(face.v3.x * 2);
    int y3 = center[1] - (int)round(face.v3.y);

    int x4 = center[0] + (int)round(face.v4.x * 2);
    int y4 = center[1] - (int)round(face.v4.y);

    // Get the bounding box of the face (min/max x and y)
    int minX = fmin(fmin(x1, x2), fmin(x3, x4));
    int maxX = fmax(fmax(x1, x2), fmax(x3, x4));
    int minY = fmin(fmin(y1, y2), fmin(y3, y4));
    int maxY = fmax(fmax(y1, y2), fmax(y3, y4));

    // Iterate over the bounding box and fill the face in the buffer
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            // Check if the point (x, y) is inside the face using pointInFace
            Point3D point = {(float)(x - center[0]) / 2.0f, (float)(center[1] - y), 0.0f};
            if (pointInFace(point, face)) {
                // Ensure the coordinates are within the buffer bounds
                if (x >= 0 && x < width && y >= 0 && y < height) {
                    // Update the buffer with the character to represent the face
                    buffer[y * width + x] = face.depthChar;  // Set the face character in the buffer
                }
            }
        }
    }
}


//Print Cube to char Buffer
void printCubeToBuffer(char *buffer, int width, int height, Face cubeFaces[6], int center[2]) {
    // Loop through each face of the cube
    for (int i = 0; i < 6; i++) {
        // Print each face to the buffer
        printFaceToBuffer(buffer, width, height, cubeFaces[i], center);
    }
}

//Print Cube to Console 
void printCube(Face cubeFaces[6], int center[2])
{
    for(int i = 0; i < 6; i++)
        {
            printFace(cubeFaces[i], center);
        }
}

//Print Point to Buffer and not to the console 

void printPoint3DToBuffer(char *buffer, int width, int height, Point3D point, int center[2], char symbol) {
    // Round the floating-point coordinates to the nearest integers
    // Double the height (y-axis) to stretch it
    int translatedX = center[0] + (int)round(point.x * 2); // Round and translate x
    int translatedY = center[1] - (int)round(point.y); // Round and stretch y (Y-axis inverted)

    // Ensure that the translated coordinates are within the bounds of the buffer
    if (translatedX >= 0 && translatedX < width && translatedY >= 0 && translatedY < height) {
        // Index into the buffer using translated coordinates
        buffer[translatedY * width + translatedX] = symbol; // Modify the buffer at the calculated position
    }
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



void clearBuffer(char *buffer, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            buffer[i * width + j] = ' ';  // Reset each cell to a space
        }
    }
}

void printBuffer(char *buffer, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            char c = buffer[i * width + j];
            if (c != ' ') {  // Only print non-space characters
                moveCursor(j, i);  // Move cursor to the position
                putchar(c);         // Print the character at the cursor
            }
        }
    }
    fflush(stdout);  // Ensure output is displayed immediately
}


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

void rotateCube(Face cubeFaces[6], Point3D center, double thetaX, double thetaY, double thetaZ) {
    // Rotate each vertex of each face by applying the rotation functions
    for (int i = 0; i < 6; i++) {
        rotateFaceXY(&cubeFaces[i], center, thetaX);
        rotateFaceXZ(&cubeFaces[i], center, thetaY);
        rotateFaceYZ(&cubeFaces[i], center, thetaZ);
    }
}

void createCube(Face cubeFaces[6]) {
    // Define the 8 vertices of the cube, centered at (0, 0, 0) with side length 10
    Point3D vertices[8] = {
        {-8, -8, -8}, {-8, -8,  8}, {-8,  8, -8}, {-8,  8,  8},
        { 8, -8, -8}, { 8, -8,  8}, { 8,  8, -8}, { 8,  8,  8}
    };

    // Define the faces of the cube by connecting the vertices
    cubeFaces[0] = (Face) { vertices[0], vertices[1], vertices[3], vertices[2], '*' }; // Front
    cubeFaces[1] = (Face) { vertices[4], vertices[5], vertices[7], vertices[6], '#' }; // Back
    cubeFaces[2] = (Face) { vertices[0], vertices[4], vertices[6], vertices[2], '$' }; // Left
    cubeFaces[3] = (Face) { vertices[1], vertices[5], vertices[7], vertices[3], '-' }; // Right
    cubeFaces[4] = (Face) { vertices[0], vertices[1], vertices[5], vertices[4], '&' }; // Bottom
    cubeFaces[5] = (Face) { vertices[2], vertices[3], vertices[7], vertices[6], '@' }; // Top
}

int main() {
    // Define all the constants in the beggining 
    /*struct timespec req;
    double rotatingVelocityX = 0;
    double rotatingVelocityY = 0;
    double rotatingVelocityZ = 0;*/


    struct timespec req;

    // Get and update the screen Dimensions
    int screenWidth = 60;
    int screenHeight = 30;
    //getTerminalDimensions(&screenWidth, &screenHeight);

    // calculate the center of the screen to render the point (0,0,0) *close* to the center 
    int center[2] = {30, 15};
    //getCenterScreen(center); //refreshes the center of the screen


    Point3D v1 = {-5.0, -5.0, 0.0};
    Point3D v2 = {5.0, -5.0, 0.0};
    Point3D v3 = {5.0, 5.0, 0.0};
    Point3D v4 = {-5.0, 5.0, 0.0};

    // Create a face with these vertices
    Face face = {v1, v2, v3, v4, '#'}; // Use '#' to differentiate the face

    clearConsole();
    Point3D pontoCentro = {0.0, 0.0, 0.0};
    //Point3D point2 = {5, 3, 2};  // Point shifted in all directions
    double angle = M_PI / 50;

    // fazer o for loop infinito que vai fazer a animação

    Face cubeFaces[6];
    
    // Create the cube
    createCube(cubeFaces);


    char *buffer = NULL;

    //getCenterScreen(center);
    //getTerminalDimensions(&screenWidth,&screenHeight);

    initializeBuffer(&buffer, screenWidth, screenHeight);

    while (1)
    {
    // get the dimensions and center of the screen



    clearConsole();
    clearBuffer(buffer, screenWidth, screenHeight);
    //printPoint3D(pontoCentro, center, 'o');
    //printPoint3D(point2, center, '*');
    //rotatePointXY(&point2, pontoCentro, angle);
    //printf("x : %d\ny : %d\nz : %d\n\n", point2.x,point2.y,point2.z);



    //rotateFaceXY(&face, pontoCentro, angle);
    //rotateFaceXZ(&face, pontoCentro, angle);
    //rotateFaceXZ(&face, pontoCentro, angle);
    //printFace(face, center);

    rotateCube(cubeFaces,pontoCentro, angle,angle,angle);
    printCubeToBuffer(buffer, screenWidth, screenHeight, cubeFaces, center);
    printBuffer(buffer, screenWidth, screenHeight);

    req.tv_sec = 0;                 // Seconds
    req.tv_nsec = 200000000L;       // Nanoseconds (100 ms = 100,000,000 ns)
    nanosleep(&req, NULL);          // Pause execution for the specified time

    // Free the Buffer, clear the dinamically allocated char buffer
    free(buffer);
    }

    return 0;
}






































/*
int main() {
    // Define all the constants in the beggining 
    /*struct timespec req;
    double rotatingVelocityX = 0;
    double rotatingVelocityY = 0;
    double rotatingVelocityZ = 0;


    // Get and update the screen Dimensions
    int screenWidth, screenHeight;
    getTerminalDimensions(&screenWidth, &screenHeight);

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
    double angle = M_PI / 50;

    // fazer o for loop infinito que vai fazer a animação

    Face cubeFaces[6];
    
    // Create the cube
    createCube(cubeFaces);


    char *buffer = NULL;

    while (1)
    {
    // get the dimensions and center of the screen
    getCenterScreen(center);
    getTerminalDimensions(&screenWidth,&screenHeight);

    initializeBuffer(&buffer, screenWidth, screenHeight);


    clearConsole();
    printPoint3D(pontoCentro, center, 'o');
    //printPoint3D(point2, center, '*');
    //rotatePointXY(&point2, pontoCentro, angle);
    //printf("x : %d\ny : %d\nz : %d\n\n", point2.x,point2.y,point2.z);



    //rotateFaceXY(&face, pontoCentro, angle);
    //rotateFaceXZ(&face, pontoCentro, angle);
    //rotateFaceXZ(&face, pontoCentro, angle);
    //printFace(face, center);

    rotateCube(cubeFaces,pontoCentro, angle,angle,angle);
    printCube(cubeFaces, center);

    req.tv_sec = 0;                 // Seconds
    req.tv_nsec = 200000000L;       // Nanoseconds (100 ms = 100,000,000 ns)
    nanosleep(&req, NULL);          // Pause execution for the specified time

    // Free the Buffer, clear the dinamically allocated char buffer
    free(buffer);
    }

    return 0;
}*/