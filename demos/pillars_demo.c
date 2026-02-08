#include "g4p.h"
#include "v4p.h"
#include <stdlib.h>
#include <math.h>

// Pillar dimensions and positioning
#define PILLAR_WIDTH 16
#define SCREEN_CENTER_X 0
#define SCREEN_CENTER_Y 0
#define NUM_PILLARS 10

const float PILLAR_HALF_WIDTH = PILLAR_WIDTH / 2.0f;

// Camera position
int cameraX = 0;      // x position (0 = center of screen)
int cameraY = 0;      // y position (0 = center of screen)
int cameraZ = 0;      // z depth (0 = center depth)

// Pillar parameters
typedef struct {
    int x;      // x position
    int z;      // z depth
    int height; // pillar height
} Pillar;

Pillar pillars[NUM_PILLARS];

// 3D projection function for pillars
typedef struct {
    float x, y, z;  // 3D position
} Point3D;

// Project 3D point to 2D screen space
void projectPoint(float x3d, float y3d, float z3d, int* x2d, int* y2d) {
    // Simplified perspective projection
    // z3d is depth (higher = further away)
    
    // Scale factor based on depth - objects get smaller as they get further away
    float scaleFactor = 1.0f;
    if (z3d > 0) {
        scaleFactor = 400.0f / (400.0f + z3d);
    }
    
    // Apply perspective projection
    // For x: scale based on depth and center around screen
    *x2d = (int)(x3d * scaleFactor);
    
    // For y: In V4P, positive Y goes downward, so we need to position
    // the pillar above the ground. We'll use a base Y position and subtract height
    *y2d = (int)((v4p_displayHeight - y3d) * scaleFactor);
}

// Create a single 3D square pillar at given position
void createPillar(int x, int z, int y, int height) {
    // Define 3D coordinates for the pillar
    // The pillar is a square prism with:
    // - Bottom at (x, y, z) 
    // - Top at (x, y + height, z)
    // - Width = PILLAR_WIDTH (along x-axis)
    // - Depth = PILLAR_WIDTH (along z-axis)
    
    // z parameter represents depth (z-coordinate)
    
    // Define the 8 corners of the 3D pillar
    // Simplified coordinates for debugging
    Point3D corners[8];
    
    // Scale factors to make the pillar visible
    float scaleX = 10.0f;  // Scale x coordinates
    float scaleY = 10.0f;   // Scale y coordinates (height) - reduced for better visibility
    float scaleZ = 3.0f;  // Scale z coordinates (depth)
    
    // Bottom face (z = z)
    corners[0].x = x * scaleX;
    corners[0].y = y * scaleY;
    corners[0].z = z * scaleZ;  // Bottom center front
    
    corners[1].x = (x + PILLAR_HALF_WIDTH) * scaleX;
    corners[1].y = y * scaleY;
    corners[1].z = z * scaleZ;  // Bottom right front
    
    corners[2].x = (x + PILLAR_HALF_WIDTH) * scaleX;
    corners[2].y = y * scaleY;
    corners[2].z = (z + PILLAR_WIDTH) * scaleZ;  // Bottom right back
    
    corners[3].x = x * scaleX;
    corners[3].y = y * scaleY;
    corners[3].z = (z + PILLAR_WIDTH) * scaleZ;  // Bottom center back
    
    // Top face (z = z + height)
    corners[4].x = x * scaleX; 
    corners[4].y = (y + height) * scaleY; 
    corners[4].z = z * scaleZ;  // Top center front
    
    corners[5].x = (x + PILLAR_HALF_WIDTH) * scaleX; 
    corners[5].y = (y + height) * scaleY; 
    corners[5].z = z * scaleZ;  // Top right front
    
    corners[6].x = (x + PILLAR_HALF_WIDTH) * scaleX; 
    corners[6].y = (y + height) * scaleY; 
    corners[6].z = (z + PILLAR_WIDTH) * scaleZ;  // Top right back
    
    corners[7].x = x * scaleX; 
    corners[7].y = (y + height) * scaleY; 
    corners[7].z = (z + PILLAR_WIDTH) * scaleZ;  // Top center back
    
    // Project all 3D points to 2D screen space
    int points2D[8][2];
    for (int i = 0; i < 8; i++) {
        projectPoint(corners[i].x, corners[i].y, corners[i].z, 
                    &points2D[i][0], &points2D[i][1]);
    }

    int layer = (int)(z / PILLAR_WIDTH) % 31;  // Layer based on depth (z coordinate)
    
    // Create the visible faces of the pillar
    // We'll create polygons for the visible sides
    // 1. Front face (quadrilateral)
    V4pPolygonP frontFace = v4p_addNew(V4P_ABSOLUTE, V4P_RED, (layer < 30 ? layer + 2 : 31));
    v4p_addPoint(frontFace, points2D[0][0], points2D[0][1]); // Bottom left front
    v4p_addPoint(frontFace, points2D[1][0], points2D[1][1]); // Bottom right front
    v4p_addPoint(frontFace, points2D[5][0], points2D[5][1]); // Top right front
    v4p_addPoint(frontFace, points2D[4][0], points2D[4][1]); // Top left front
    v4p_addPoint(frontFace, points2D[0][0], points2D[0][1]); // Close polygon
    
    // 2. Top face (quadrilateral) - only visible if we're looking from above
    // Top face is visible if front top y > back top y (front is below back in screen coordinates)
    if (points2D[4][1] > points2D[7][1]) { // Top left front y > Top left back y
        V4pPolygonP topFace = v4p_addNew(V4P_ABSOLUTE, V4P_BLUE, (layer < 31 ? layer + 1 : 31));
        v4p_addPoint(topFace, points2D[4][0], points2D[4][1]); // Top left front
        v4p_addPoint(topFace, points2D[5][0], points2D[5][1]); // Top right front
        v4p_addPoint(topFace, points2D[6][0], points2D[6][1]); // Top right back
        v4p_addPoint(topFace, points2D[7][0], points2D[7][1]); // Top left back
        v4p_addPoint(topFace, points2D[4][0], points2D[4][1]); // Close polygon
    }
    
    // 3. Right side face (quadrilateral) - visible if right front x < right back x
    if (points2D[1][0] < points2D[2][0]) { // Bottom right front x < Bottom right back x
        V4pPolygonP rightFace = v4p_addNew(V4P_ABSOLUTE, V4P_MAROON, layer);
        v4p_addPoint(rightFace, points2D[1][0] + 1, points2D[1][1]); // Bottom right front
        v4p_addPoint(rightFace, points2D[2][0] + 1, points2D[2][1]); // Bottom right back
        v4p_addPoint(rightFace, points2D[6][0] + 1, points2D[6][1]);  // Top right back
        v4p_addPoint(rightFace, points2D[5][0] + 1, points2D[5][1]); // Top right front
        v4p_addPoint(rightFace, points2D[1][0] + 1, points2D[1][1]);  // Close polygon
    }

    // 4. Left side face (quadrilateral) - visible if left front x > left back x
    if (points2D[0][0] > points2D[3][0]) {  // Bottom left front x >  Bottom left back x
        V4pPolygonP leftFace = v4p_addNew(V4P_ABSOLUTE, V4P_CYAN, layer);
        v4p_addPoint(leftFace, points2D[0][0], points2D[0][1]); // Bottom left front
        v4p_addPoint(leftFace, points2D[3][0], points2D[3][1]); // Bottom left back
        v4p_addPoint(leftFace, points2D[7][0], points2D[7][1]); // Top left back
        v4p_addPoint(leftFace, points2D[4][0], points2D[4][1]); // Top left front
        v4p_addPoint(leftFace, points2D[0][0], points2D[0][1]); // Close polygon
    }
}

// Initialize pillars with various heights
void initPillars() {
    for (int i = 0; i < NUM_PILLARS; i++) {
        pillars[i].x = i * (2 * PILLAR_WIDTH) - 150;  // Aligned pillars spaced 30 units apart
        pillars[i].z = 50;   // depth
        pillars[i].height = 50 + rand() % 200; // Random heights between 50-250
    }
}

Boolean g4p_onInit(int quality, Boolean fullscreen) {
    v4p_init2(quality, fullscreen);
    v4p_setView(-v4p_displayWidth / 2, -v4p_displayHeight / 2, v4p_displayWidth / 2, v4p_displayHeight / 2);
    v4p_setBGColor(V4P_BLACK);
    
    // Initialize pillars
    initPillars();
    
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    (void)deltaTime; // Unused parameter
    
    // Clear the scene by removing all polygons
    v4p_clearScene();
    
    // Camera/position controls
    if (g4p_state.buttons[G4P_LEFT]) {
        cameraX -= 2;
    }
    if (g4p_state.buttons[G4P_RIGHT]) {
        cameraX += 2;
    }
    if (g4p_state.buttons[G4P_UP]) {
        cameraZ -= 2;
    }
    if (g4p_state.buttons[G4P_DOWN]) {
        cameraZ += 2;
    }
    
    // Update camera Y position (up/down movement)
    if (g4p_state.buttons[G4P_SHIFT]) {
        cameraY -= 2;
    }
    if (g4p_state.buttons[G4P_CTRL]) {
        cameraY += 2;
    }
    
    // Create all pillars with camera offset
    for (int i = 0; i < NUM_PILLARS; i++) {
        // Apply camera offset: pillarX - cameraX
        int pillarScreenX = pillars[i].x - cameraX;
        int pillarScreenZ = pillars[i].z - cameraZ;
        int pillarScreenY = -200 - cameraY;
        createPillar(pillarScreenX, pillarScreenZ, pillarScreenY, pillars[i].height);
    }
    
    return success;
}

Boolean g4p_onFrame() {
    return v4p_render();
}

void g4p_onQuit() {
    v4p_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}