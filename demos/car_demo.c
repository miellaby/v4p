#include <stdio.h>
#include <math.h>
#include "g4p.h"
#include "v4p.h"
#include "v4pserial.h"
#include "lowmath.h"  // For computeCosSin()

// Background layers from circuit.svg
V4pPolygonP level_ground;  // Back layer (yellow)
V4pPolygonP level_grass;   // Second layer (green)
V4pPolygonP level_road;     // Road (dark gray)
V4pPolygonP level_shades;   // Shades (dark green)
V4pPolygonP level_buildings; // Buildings and panels (blue/red)
V4pPolygonP bg_layer5_buildings_2;  // Buildings and panels (blue/red)
V4pPolygonP level_hay_piles;  // Small obstacles (orange)
V4pPolygonP level_plots;  // Small obstacles (red)
V4pPolygonP level_trees;  // Trees (light gray)

// Car components
V4pPolygonP car_body_proto;
V4pPolygonP car_windows_proto;
V4pPolygonP car_wheels_proto;
V4pPolygonP car_lights_proto;

V4pPolygonP car;

// Car position and movement
float car_x = 0;
float car_y = 0;
float car_angle = 90;  // Start facing right (90 degrees)
float car_speed_x = 0;
float car_speed_y = 0;
Boolean thrusting = false;

// Helper function to get sin/cos values using v4p's 512-unit circle system
void getSinCosFromDegrees(float degrees, int* sina, int* cosa) {
    // Convert degrees to v4p's 512-unit circle format
    // computeCosSin will handle angle wrapping via bitmasking (angle & 0x1FF)
    int v4p_angle = (int)(degrees * 512.0f / 360.0f);
    computeCosSin((UInt16)v4p_angle);
    *sina = lwmSina;
    *cosa = lwmCosa;
}

// Create background layers from circuit.svg using original paths
void create_background_layers() {
    // Layer 1: Yellow background (back layer) - original SVG path
    level_ground = v4p_new(V4P_ABSOLUTE, V4P_YELLOW, 0);
    v4p_decodeSVGPath(level_ground,
                            "m377.4 93-88.6-6.2L245.4 58l-70-13-97 16-35 38-13 76-12 78 46 67 11 89.6 61.4 52 124.6-9.6 98 2 39-49 51-37 40-13 94 54 36-8 9-43-21-55-61-19-59-27-10-58 57-30 49-49-23-63-54-31-56 11-44 35",
                            16.0f);

    // Layer 2: Green background (second layer) - original SVG path
    level_grass = v4p_addNewSub(level_ground, V4P_ABSOLUTE, V4P_GREEN, 1);
    v4p_decodeSVGPath(level_grass,
                      "m105 141.4 19-31 87-8.8 97.4 14 92.2-6.4L478 88.4l26 24-38 47-45 45 16 41 86 44 28 4 43 19 22 "
                      "40v37l-16 8-27-2-26-4-62-42-47-2-34 21-65 35-62 31-86-9-67-34-29-29-6-57-20-40-2-16 2-27 21-20 "
                      "13.6-32z",
                      16.0f);

    // Layer 3: Road (dark gray) - original SVG path
    level_road = v4p_addNewSub(level_ground, V4P_ABSOLUTE, V4P_GRAY, 2);
    v4p_decodeSVGPath(
        level_road,
        "m182.6 168.2-26.2 8.8-20.2 16.4-14 22L117 238h22.2l8.4-19.6 12.8-16.4 23.4-11.2 23.2-4 .6-21.3z", 16.0f);
    v4p_decodeSVGPath(
        level_road,
        "m117 237.1-1.6 20.7-9.6 17.8-14.6 16.9-9 16.1-.2 15h21l2.5-16.3 13.3-12.7 9.6-13 10-19 1-26z"
        "M207 186.8v-21.2h46v21.2z"
        "M82 323.4h21.2v37.2H82z"
        "M252.6 186.8v-21.2h46v21.2z"
        "M298.2 186.4v-21.2h46v21.2z"
        "M84.3 384.7l8.8 26.2 16.4 20.2 22 14 22.6 5.2V428l-19.6-8.4-16.4-12.8-11.2-23.4-4-24-21.3.2z", 16.0f);
    v4p_decodeSVGPath(
        level_road,
        "m178.9 447.6 26.2-8.8 20.2-16.4 14-22 5.2-22.6h-22.2l-8.4 19.6-12.8 16.4-23.4 11.2-24 4 .2 21.3z", 16.0f);
    v4p_decodeSVGPath(
        level_road,
        "m258.3 292-1.7 20.6-9.6 17.8-14.6 16.9-9 16.1-.2 15h21l2.5-16.3 13.3-12.7 9.6-13 10-19 1-26z", 16.0f);
    v4p_decodeSVGPath(
        level_road,
        "m323.3 222.5-26.2 8.8-20.2 16.4-14 22-5.2 22.6h22.2l8.4-19.6 12.8-16.4 23.4-11.2 24-4-.2-21.3z"
        "M421.3 214.3l-14-23.7-20.3-16.3-24.5-9-23.2-.4 4.7 21.7 21 4 18.6 9.1 15.9 20.6 9 22.6 20.7-4.7z"
        "M409.3 242.4 411 253l-3.5 2.7-10.4-18.4-21-15-31-3.1 1.8 22.1 23.8 4.9 15.8 17 10.7 18 16.6 6.1 14.2-8.3 7-18.2-5-22.9z",
        16.0f);

    // Layer 4: Shades (dark green) - original SVG path
    level_shades = v4p_addNewSub(level_ground, V4P_ABSOLUTE, V4P_OLIVE, 3);
    v4p_decodeSVGPath(level_shades,
                            "M336.5 123.6l20.3 11.6v24.4H276l-20.8-9.2v-26.8z"
                            "M446.5 276.8l20.3 11.6 12.4 12.4-8.4 8.8-40 20.4-19.6-6.4-11.2-24z",
                            16.0f);

    // Layer 5: Buildings and panels (blue/red) - original SVG path
    level_buildings = v4p_addNewSub(level_ground, V4P_ABSOLUTE, V4P_BLUE, 4);
    v4p_decodeSVGPath(level_buildings,
                      "m258.5 150.6-.1-25.8 74-.4v26.4z"
                      "M407.3 311.6l-5.7-11.6 44.4-22.4 6.8 10.8z",
                      16.0f);
    v4p_decodeSVGPath(v4p_addNewSub(level_buildings , V4P_ABSOLUTE, V4P_RED, 5),
                      "m256.1 145.4-.1-25.8 80.4-.4v26.4z"
                      "M424.9 326.4l-3.7-7.6 28.4-14.8 4.4 7.1z",
                      16.0f);
    v4p_decodeSVGPath(v4p_addNewSub(level_buildings, V4P_ABSOLUTE, V4P_YELLOW, 5),
                      "m410 311.6-1.6-3.1 7.4-3.7 1.8 3",
                      16.0f);
    // Layer 6: Small obstacles (orange) - original SVG path
    level_hay_piles = v4p_addNewSub(level_ground, V4P_ABSOLUTE, 120, 5);  // Orange color
    v4p_decodeSVGPath(level_hay_piles,
                            "m133 183.4 7.8-8.2 3.2 4.8-7.6 6z"
                            "M160.2 171.2l-10 5.4-1.6-5.5 9-3.4z"
                            "M177 167.3l-11 2.5V164l9.6-.8z"
                            "M128.1 298.2l8.2-8.2 3.3 4.8-7.9 6z"
                            "M144 276l-.9 11.5-5.6-1.5 2-9.7z"
                            "M269.3 241l8.2-8.2 3.3 4.8-7.9 6z"
                            "M298 225.4l-9.6 6.4-2.3-5.3 9-4.4z"
                            "M316.2 220.9l-11.5 3-2.1-5.3 10.5-1.4z",
                            16.0f);
    level_plots = v4p_addNewSub(level_ground, V4P_ABSOLUTE, V4P_RED, 5);
    v4p_decodeSVGPath(level_plots,
                      "m111 313-1.4-5.8 6-.4.8 5.6z"
                      "M247 317l-1.4-5.8 6-.4.8 5.6z"
                      "M156.1 426.1l-1.3-5.7 6-.4.8 5.6z"
                      "M325 253l-1.4-5.8 6-.4.8 5.6z"
                      "M407 256.5l-1.4-5.7 6-.4.8 5.6z"
                      "M176.1 201.3l-1.3-5.7 6-.4.8 5.6z",
                      16.0f);

    // Layer 7: Trees (light gray) - original SVG path
    level_trees = v4p_addNewSub(level_ground, V4P_ABSOLUTE, V4P_WHITE, 6);
    v4p_decodeSVGPath(level_trees,
                            "m365 149 11-9.8 15.6 6.8 2.8 12.4-6 4-10.4 4.8-10.8-3.2.8-7.6z"
                            "M314.6 278.6l-4-14.2 13-11 12.4 2.8 1 7.2-.3 11.4-7.6 8.3-6.5-4z"
                            "M161.7 221l3-14.5 16.7-3.8 9.6 8.3-2.4 6.8-5.6 10-10.5 3.9-4-6.6z"
                            "M156.9 413l3-14.5 16.7-3.8 9.6 8.3-2.4 6.8-5.6 10-10.5 3.9-4-6.6z",
                            16.0f);
    v4p_centerPolygon(level_ground);
}

Boolean g4p_onInit(int quality, Boolean fullscreen) {
    v4p_init2(quality, fullscreen);
    v4p_setView(-0.44 * v4p_displayWidth, -0.44 * v4p_displayHeight, v4p_displayWidth * 0.44, v4p_displayHeight * 0.44);
    v4p_setBGColor(V4P_DARK);

    // Create background layers
    create_background_layers();

    // Add background layers to scene (in order from back to front)
    v4p_add(level_ground);

    // Create car body (red main part)
    car_body_proto = v4p_new(V4P_ABSOLUTE, V4P_RED, 1);
    v4p_decodeSVGPath(car_body_proto,
                            "M260,355.5L259,212.5L277,180.5L363.5,180.5L380,211.5L381,354.5L363.5,376.5L277.5,377.5Z",
                            0.5f);

    // Create car windows (cyan parts)
    car_windows_proto = v4p_new(V4P_ABSOLUTE, V4P_CYAN, 2);
    v4p_decodeSVGPath(car_windows_proto,
                            "M272,267L282,235.5L352.1,235.5L362,267L357.1,270.5L350.8,253.5L282,253L274.7,270Z",
                            0.5f);
    v4p_decodeSVGPath(car_windows_proto,
                            "M270,348.8L286.5,366L337.1,366L355.5,346.7L350,337.7L342.2,349.6L275.4,349.4L263.7,339.5Z",
                            0.5f);
    v4p_setAnchorToCenter(car_windows_proto);

    // Create car wheels (black parts)
    car_wheels_proto = v4p_new(V4P_ABSOLUTE, V4P_BLACK, 0);
    v4p_decodeSVGPath(car_wheels_proto,
                            "M264.5,240.5L245.5,198.5L270.5,184.5L289.5,221.5Z",
                            0.5f);
    v4p_decodeSVGPath(car_wheels_proto,
                            "M365.5,243.5L345.5,203.5L372.5,186.5L390.5,226.5Z",
                            0.5f);
    v4p_decodeSVGPath(car_wheels_proto,
                            "M250,319.5L276.4,319.5L277.5,370.5L250,370.5Z",
                            0.5f);
    v4p_decodeSVGPath(car_wheels_proto,
                            "M359.5,319.5L387.4,319.5L388.5,371.5L359.5,371.5Z",
                            0.5f);
    v4p_setAnchorToCenter(car_wheels_proto);

    // Create car lights (gray parts)
    car_lights_proto = v4p_new(V4P_ABSOLUTE, V4P_GRAY, 2);
    v4p_decodeSVGPath(car_lights_proto,
                            "M281.1,185.5L280.5,214.5L270.5,222.5L270.9,204.2L281,185.5Z",
                            0.5f);
    v4p_decodeSVGPath(car_lights_proto,
                            "M364,187.5L364,216L373,224.5L373,207.2L364,187.5Z",
                            0.5f);
    v4p_setAnchorToCenter(car_lights_proto);

    // Create car instances
    v4p_addSub(car_body_proto, car_windows_proto);
    v4p_addSub(car_body_proto, car_wheels_proto);
    v4p_addSub(car_body_proto, car_lights_proto);
    v4p_centerPolygon(car_body_proto);

    car = v4p_addClone(car_body_proto);
    // Position car at center
    car_x = 0;
    car_y = 0;
    
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    // Calculate current speed magnitude for realistic car handling
    float current_speed = sqrtf(car_speed_x * car_speed_x + car_speed_y * car_speed_y);

    // Handle rotation input - car can only rotate when moving
    // Rotation rate proportional to speed (faster = easier to turn, but with limits)
    float rotation_rate = fminf(current_speed * 0.4f, 0.3f);
    if (g4p_state.buttons[G4P_LEFT]) {  // Left Arrow - rotate left
        car_angle -= rotation_rate * deltaTime;
    }
    if (g4p_state.buttons[G4P_RIGHT]) {  // Right Arrow - rotate right
        car_angle += rotation_rate * deltaTime;
    }

    // Continuous convergence: gradually align speed vector with car direction
    // This happens regardless of user input for realistic car physics
    int target_sina, target_cosa;
    getSinCosFromDegrees(car_angle, &target_sina, &target_cosa);
    
    // Calculate target velocity components for current direction
    float target_speed_x = ((float)target_sina / 256.0f) * current_speed;
    float target_speed_y = (-((float)target_cosa / 256.0f)) * current_speed;
    
    // Blend current velocity towards the target direction
    // Faster convergence when moving faster, but with limits
    float blend_factor = fminf(current_speed * 0.01f, 0.15f) * deltaTime;
    car_speed_x = car_speed_x * (1.0f - blend_factor) + target_speed_x * blend_factor;
    car_speed_y = car_speed_y * (1.0f - blend_factor) + target_speed_y * blend_factor;

    thrusting = false;
    if (g4p_state.buttons[G4P_UP]) {  // Up Arrow - thrust forward
        thrusting = true;
        // Apply thrust in the direction the car is facing
        int sina, cosa;
        getSinCosFromDegrees(car_angle, &sina, &cosa);
        car_speed_x += ((float)sina / 256.0f) * 0.001f * deltaTime;  // Accelerate in thrust direction
        car_speed_y -= ((float)cosa / 256.0f) * 0.001f * deltaTime;
    } else {
        // Apply friction/deceleration when no thrust
        float friction_factor = powf(0.999f, deltaTime);
        car_speed_x *= friction_factor;  // Slow down gradually
        car_speed_y *= friction_factor;

        // Small threshold to stop completely
        if (fabs(car_speed_x) + fabs(car_speed_y) < 0.0001f) {
            car_speed_x = 0;
            car_speed_y = 0;
        }
    }

    // Apply the speed vector to car position
    car_x += car_speed_x * (float)deltaTime;
    car_y += car_speed_y * (float)deltaTime;

    // Update car position and rotation (convert degrees to V4P format)
    v4p_transform(car, car_x, car_y, car_angle * 512.f / 360.f, 10, 256, 256);

    // Update view to center on car position
    float zooming = current_speed < 0.5 ? 0.5 : current_speed;
    float camera_x = car_x + car_speed_x * 100;
    float camera_y = car_y + car_speed_y * 100;
    v4p_setView(camera_x - v4p_displayWidth * zooming, camera_y - v4p_displayHeight * zooming,
                camera_x + v4p_displayWidth * zooming, camera_y + v4p_displayHeight * zooming);
    return success;
}

Boolean g4p_onFrame() {
    v4p_render();
    return success;
}

void g4p_onQuit() {
    v4p_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}