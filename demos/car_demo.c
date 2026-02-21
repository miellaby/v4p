#include <stdio.h>
#include <math.h>
#include "g4p.h"
#include "v4p.h"
#include "v4pserial.h"
#include "lowmath.h"  // For computeCosSin()

// Background layers from circuit.svg
V4pPolygonP land;  // Back layer (yellow)
V4pPolygonP grass;  // Second layer (green)
V4pPolygonP road;  // Road (dark gray)
V4pPolygonP shades;  // Shades (dark green)
V4pPolygonP building1;  // Buildings and panels (blue/red)
V4pPolygonP building2;  // Buildings and panels (blue/red)
V4pPolygonP hay_blocks;  // Small obstacles (orange)
V4pPolygonP plots;  // Small obstacles (red)
V4pPolygonP trees;  // Trees (light gray)

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

const float LEVEL_SCALE = 12.0f;  // Scale factor for SVG paths to fit the view

// Helper function to get sin/cos values using v4p's 512-unit circle system
void getSinCosFromDegrees(float degrees, int* sina, int* cosa) {
    // Convert degrees to v4p's 512-unit circle format
    // computeCosSin will handle angle wrapping via bitmasking (angle & 0x1FF)
    int v4p_angle = (int) (degrees * 512.0f / 360.0f);
    computeCosSin((UInt16) v4p_angle);
    *sina = lwmSina;
    *cosa = lwmCosa;
}

// Create background layers from circuit.svg using original paths
void create_background_layers() {
    // Layer 1: Yellow background (back layer) - original SVG path
    land = v4p_new(V4P_ABSOLUTE, V4P_YELLOW, 0);
    v4p_decodeSVGPath(land,
                      "M 377.4 93 L 288.8 86.8 L 245.4 58 L 175.4 45 L 78.4 61 L 43.4 99 L "
                      "30.4 175 L 18.4 253 L 64.4 320 L 75.4 409.6 L 136.8 461.6 L 261.4 452 "
                      "L 359.4 454 L 398.4 405 L 449.4 368 L 489.4 355 L 583.4 409 L 619.4 "
                      "401 L 628.4 358 L 607.4 303 L 546.4 284 L 487.4 257 L 477.4 199 L "
                      "534.4 169 L 583.4 120 L 560.4 57 L 506.4 26 L 450.4 37 L 406.4 72",
                      LEVEL_SCALE);

    // Layer 2: Green background (second layer) - original SVG path
    grass = v4p_addNewSub(land, V4P_ABSOLUTE, V4P_GREEN, 1);
    v4p_decodeSVGPath(grass,
                      "M 105 141.4 L 124 110.4 L 211 101.6 L 308.4 115.6 L 400.6 109.2 L 478 "
                      "88.4 L 504 112.4 L 466 159.4 L 421 204.4 L 437 245.4 L 523 289.4 L "
                      "551 293.4 L 594 312.4 L 616 352.4 L 616 389.4 L 600 397.4 L 573 395.4 "
                      "L 547 391.4 L 485 349.4 L 438 347.4 L 404 368.4 L 339 403.4 L 277 "
                      "434.4 L 191 425.4 L 124 391.4 L 95 362.4 L 89 305.4 L 69 265.4 L 67 "
                      "249.4 L 69 222.4 L 90 202.4 L 103.6 170.4 Z",
                      LEVEL_SCALE);

    // Layer 3: Road (dark gray) - original SVG path
    road = v4p_addNewSub(land, V4P_ABSOLUTE, V4P_GRAY, 2);

    // road_1  (fill: #7f7f7f -> V4P_GRAY)
    v4p_decodeSVGPath(road,
                      "M 182.6 168.2 L 156.4 177 L 136.2 193.4 L 122.2 215.4 L 117 237.55 L "
                      "139.3 237.3 L 147.6 218.4 L 160.4 202 L 183.8 190.8 L 207 186.8 L "
                      "207.3 165.55 Z",
                      LEVEL_SCALE);

    // road_2  (fill: #7f7f7f -> V4P_GRAY)
    v4p_decodeSVGPath(road, "m 207,186.8 0.3,-21.25 45.5,0.05 v 21.2 z", LEVEL_SCALE);

    // road_3  (fill: #7f7f7f -> V4P_GRAY)
    v4p_decodeSVGPath(road, "m 252.8,186.8 v -21.2 l 45.6,-0.2 v 21.2 z", LEVEL_SCALE);

    // path6  (fill: #7f7f7f -> V4P_GRAY)
    v4p_decodeSVGPath(road, "m 82,323.5 h 21.1 l -0.05,36.5 -21.25,0.1 z", LEVEL_SCALE);

    // road_4  (fill: #7f7f7f -> V4P_GRAY)
    v4p_decodeSVGPath(road, "m 298.4,186.6 v -21.2 l 45.8,-0.2 -0.1,21.3 z", LEVEL_SCALE);

    // road_5  (fill: #7f7f7f -> V4P_GRAY)
    v4p_decodeSVGPath(road,
                      "m 421.3,214.3 -14,-23.7 -20.3,-16.3 -24.5,-9 -18.3,-0.1 -0.1,21.3 "
                      "20.9,4.1 18.6,9.1 15.9,20.6 9.4,22.35 20.7,-4.6 z",
                      LEVEL_SCALE);

    // road_6  (fill: #7f7f7f -> V4P_GRAY)
    v4p_decodeSVGPath(road,
                      "M 408.9 242.65 L 411 253 L 407.5 255.7 L 397.1 237.3 L 376.1 222.3 L "
                      "345.1 219.2 L 347.7 241.2 L 370.7 246.2 L 386.5 263.2 L 397.2 281.2 L "
                      "413.8 287.3 L 428 279 L 435 260.8 L 429.6 238.05 Z",
                      LEVEL_SCALE);

    // road_7  (fill: #7f7f7f -> V4P_GRAY)
    v4p_decodeSVGPath(road,
                      "m 323.3,222.5 -26.2,8.8 -20.2,16.4 -14,22 -4.9,22.45 22.25,-0.3 "
                      "8.05,-19.15 12.8,-16.4 23.4,-11.2 23.2,-3.9 -2.6,-22 z",
                      LEVEL_SCALE);

    // road_8  (fill: #7f7f7f -> V4P_GRAY)
    v4p_decodeSVGPath(road,
                      "M 258 292.15 L 256.6 312.6 L 247 330.4 L 232.4 347.3 L 223.4 363.4 L "
                      "222.75 378.1 L 244.35 378.1 L 246.7 362.1 L 260 349.4 L 269.6 336.4 L "
                      "279.6 317.4 L 280.25 291.85 Z",
                      LEVEL_SCALE);

    // road_9  (fill: #7f7f7f -> V4P_GRAY)
    v4p_decodeSVGPath(road,
                      "M 178.9 447.6 L 205.1 438.8 L 225.3 422.4 L 239.3 400.4 L 244.35 "
                      "378.1 L 222.75 378.1 L 213.9 397.4 L 201.1 413.8 L 177.7 425 L 153.9 "
                      "428.5 L 154 450.3 Z",
                      LEVEL_SCALE);

    // road_10  (fill: #7f7f7f -> V4P_GRAY)
    v4p_decodeSVGPath(road,
                      "m 84.3,384.7 8.8,26.2 16.4,20.2 22,14 22.5,5.2 -0.1,-21.8 -19.4,-8.9 "
                      "-16.4,-12.8 -11.2,-23.4 -3.85,-23.4 -21.25,0.1 z",
                      LEVEL_SCALE);

    // road_11  (fill: #7f7f7f -> V4P_GRAY)
    v4p_decodeSVGPath(road,
                      "m 117,237.55 -1.6,20.25 -9.6,17.8 -14.6,16.9 -9,16.1 -0.2,14.9 h 21.1 "
                      "l 2.4,-16.2 13.3,-12.7 9.6,-13 10,-19 0.9,-25.3 z",
                      LEVEL_SCALE);

    // Layer 4: Shades (dark green) - original SVG path
    shades = v4p_addNewSub(land, V4P_ABSOLUTE, V4P_DARK_GREEN, 3);

    // shade1  (fill: #007f00 -> V4P_DARK_GREEN)
    v4p_decodeSVGPath(shades,
                      "m 446.5,276.8 20.3,11.6 12.4,12.4 -8.4,8.8 -40,20.4 -19.6,-6.4 "
                      "-11.2,-24 z",
                      LEVEL_SCALE);

    // shade2  (fill: #007f00 -> V4P_DARK_GREEN)
    v4p_decodeSVGPath(shades, "m 336.5,123.6 20.3,11.6 v 24.4 H 276 l -20.8,-9.2 v -26.8 z", LEVEL_SCALE);

    building1 = v4p_addNewSub(land, V4P_ABSOLUTE, V4P_BLUE, 4);

    // build1_terrace  (fill: #00f -> V4P_BLUE)
    v4p_decodeSVGPath(building1, "M 258.5 150.6 L 258.4 124.8 L 332.4 124.4 L 332.4 150.8 Z", LEVEL_SCALE);

    // build1  (fill: #00007f -> V4P_NAVY)
    v4p_decodeSVGPath(v4p_addNewSub(building1, V4P_ABSOLUTE, V4P_NAVY, 5),
                      "M 256.1 145.4 L 256 119.6 L 336.4 119.2 L 336.4 145.6 Z", LEVEL_SCALE);

    building2 = v4p_addNewSub(land, V4P_ABSOLUTE, V4P_RED, 4);

    // build2_terrace  (fill: #7f7f7f -> V4P_GRAY)
    v4p_decodeSVGPath(building2, "M 424.9 326.4 L 421.2 318.8 L 449.6 304 L 454 311.1", LEVEL_SCALE);

    // build2  (fill: #7f0000 -> V4P_DARK_RED)
    v4p_decodeSVGPath(v4p_addNewSub(building2, V4P_ABSOLUTE, V4P_DARK_RED, 5),
                      "M 400.1 299.6 L 446 276 L 459.2 299.2 L 411.2 323.6 Z", LEVEL_SCALE);

    // build2_halfroof  (fill: red -> V4P_RED)
    v4p_decodeSVGPath(v4p_addNewSub(building2, V4P_ABSOLUTE, V4P_RED, 6),
                      "M 407.3 311.6 L 401.6 300 L 446 277.6 L 452.8 288.4", LEVEL_SCALE);

    // build2_chemney  (fill: #bfbfbf -> V4P_SILVER)
    v4p_decodeSVGPath(v4p_addNewSub(building2, V4P_ABSOLUTE, V4P_SILVER, 7),
                      "M 410 311.6 L 408.4 308.5 L 415.8 304.8 L 417.6 307.8", LEVEL_SCALE);

    hay_blocks = v4p_addNewSub(land, V4P_ABSOLUTE, 120, 5);  // Orange color

    // hay_blocks  (fill: #ff7f00 -> 120)
    v4p_decodeSVGPath(hay_blocks,
                      "M 133 183.4 L 140.8 175.2 L 144 180 L 136.4 186 Z M 160.2 171.2 L "
                      "150.2 176.6 L 148.6 171.1 L 157.6 167.7 Z M 177 167.3 L 166 169.8 L "
                      "166 164 L 175.6 163.2 Z M 128.1 298.2 L 136.3 290 L 139.6 294.8 L "
                      "131.7 300.8 Z M 144 276 L 143.1 287.5 L 137.5 286 L 139.5 276.3 Z M "
                      "269.3 241 L 277.5 232.8 L 280.8 237.6 L 272.9 243.6 Z M 298 225.4 L "
                      "288.4 231.8 L 286.1 226.5 L 295.1 222.1 Z M 316.2 220.9 L 304.7 223.9 "
                      "L 302.6 218.6 L 313.1 217.2 Z",
                      LEVEL_SCALE);

    plots = v4p_addNewSub(land, V4P_ABSOLUTE, V4P_RED, 5);

    // plots  (fill: red -> V4P_RED)
    v4p_decodeSVGPath(plots,
                      "M 111 313 L 109.6 307.2 L 115.6 306.8 L 116.4 312.4 Z M 247 317 L "
                      "245.6 311.2 L 251.6 310.8 L 252.4 316.4 Z M 156.1 426.1 L 154.8 420.4 "
                      "L 160.8 420 L 161.6 425.6 Z M 325 253 L 323.6 247.2 L 329.6 246.8 L "
                      "330.4 252.4 Z M 407 256.5 L 405.6 250.8 L 411.6 250.4 L 412.4 256 Z M "
                      "176.1 201.3 L 174.8 195.6 L 180.8 195.2 L 181.6 200.8 Z",
                      LEVEL_SCALE);

    trees = v4p_addNewSub(land, V4P_ABSOLUTE, V4P_LIMEGREEN, 6);

    // tree_0  (fill: #c3ff00 -> V4P_LIMEGREEN)
    v4p_decodeSVGPath(trees,
                      "m 156.9,413 3,-14.5 16.7,-3.8 9.6,8.3 -2.4,6.8 -5.6,10 -10.5,3.9 "
                      "-4,-6.6 z",
                      LEVEL_SCALE);

    // tree_1  (fill: #c3ff00 -> V4P_LIMEGREEN)
    v4p_decodeSVGPath(trees,
                      "m 161.7,221 3,-14.5 16.7,-3.8 9.6,8.3 -2.4,6.8 -5.6,10 -10.5,3.9 "
                      "-4,-6.6 z",
                      LEVEL_SCALE);

    // tree_2  (fill: #c3ff00 -> V4P_LIMEGREEN)
    v4p_decodeSVGPath(trees,
                      "m 314.6,278.6 -4,-14.2 13,-11 12.4,2.8 1,7.2 -0.3,11.4 -7.6,8.3 "
                      "-6.5,-4 z",
                      LEVEL_SCALE);

    // tree_4  (fill: #c3ff00 -> V4P_LIMEGREEN)
    v4p_decodeSVGPath(trees,
                      "m 365,149 11,-9.8 15.6,6.8 2.8,12.4 -6,4 -10.4,4.8 -10.8,-3.2 "
                      "0.8,-7.6 z",
                      LEVEL_SCALE);

    v4p_centerPolygon(land);
}

Boolean g4p_onInit(int quality, Boolean fullscreen) {
    v4p_init2(quality, fullscreen);
    v4p_setView(-0.44 * v4p_displayWidth, -0.44 * v4p_displayHeight, v4p_displayWidth * 0.44, v4p_displayHeight * 0.44);
    v4p_setBGColor(V4P_TEAL);

    // Create background layers
    create_background_layers();

    // Add background layers to scene (in order from back to front)
    v4p_add(land);

    // Create car body (red main part)
    car_body_proto = v4p_new(V4P_ABSOLUTE, V4P_RED, 1);
    v4p_decodeSVGPath(car_body_proto,
                      "M260,355.5L259,212.5L277,180.5L363.5,180.5L380,211.5L381,354.5L363.5,376.5L277.5,377.5Z", 0.5f);

    // Create car windows (cyan parts)
    car_windows_proto = v4p_new(V4P_ABSOLUTE, V4P_TEAL, 2);
    v4p_decodeSVGPath(car_windows_proto,
                      "M272,267L282,235.5L352.1,235.5L362,267L357.1,270.5L350.8,253.5L282,253L274.7,270Z", 0.5f);
    v4p_decodeSVGPath(car_windows_proto,
                      "M270,348.8L286.5,366L337.1,366L355.5,346.7L350,337.7L342.2,349.6L275.4,349.4L263.7,339.5Z",
                      0.5f);
    v4p_setAnchorToCenter(car_windows_proto);

    // Create car wheels (black parts)
    car_wheels_proto = v4p_new(V4P_ABSOLUTE, V4P_BLACK, 0);
    v4p_decodeSVGPath(car_wheels_proto, "M264.5,240.5L245.5,198.5L270.5,184.5L289.5,221.5Z", 0.5f);
    v4p_decodeSVGPath(car_wheels_proto, "M365.5,243.5L345.5,203.5L372.5,186.5L390.5,226.5Z", 0.5f);
    v4p_decodeSVGPath(car_wheels_proto, "M250,319.5L276.4,319.5L277.5,370.5L250,370.5Z", 0.5f);
    v4p_decodeSVGPath(car_wheels_proto, "M359.5,319.5L387.4,319.5L388.5,371.5L359.5,371.5Z", 0.5f);
    v4p_setAnchorToCenter(car_wheels_proto);

    // Create car lights (gray parts)
    car_lights_proto = v4p_new(V4P_ABSOLUTE, V4P_GRAY, 2);
    v4p_decodeSVGPath(car_lights_proto, "M281.1,185.5L280.5,214.5L270.5,222.5L270.9,204.2L281,185.5Z", 0.5f);
    v4p_decodeSVGPath(car_lights_proto, "M364,187.5L364,216L373,224.5L373,207.2L364,187.5Z", 0.5f);
    v4p_setAnchorToCenter(car_lights_proto);

    // Create car instances
    v4p_addSub(car_body_proto, car_windows_proto);
    v4p_addSub(car_body_proto, car_wheels_proto);
    v4p_addSub(car_body_proto, car_lights_proto);
    v4p_centerPolygon(car_body_proto);

    car = v4p_addClone(car_body_proto);
    // Position car at center
    car_x = 0;
    car_y = -800;

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
    float target_speed_x = ((float) target_sina / 256.0f) * current_speed;
    float target_speed_y = (-((float) target_cosa / 256.0f)) * current_speed;

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
        car_speed_x += ((float) sina / 256.0f) * 0.001f * deltaTime;  // Accelerate in thrust direction
        car_speed_y -= ((float) cosa / 256.0f) * 0.001f * deltaTime;
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
    car_x += car_speed_x * (float) deltaTime;
    car_y += car_speed_y * (float) deltaTime;

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