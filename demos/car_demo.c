#include <stdio.h>
#include <math.h>
#include "g4p.h"
#include "v4p.h"
#include "v4pserial.h"
#include "lowmath.h"  // For computeCosSin()

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

Boolean g4p_onInit(int quality, Boolean fullscreen) {
    v4p_init2(quality, fullscreen);
    v4p_setView(-0.44 * v4p_displayWidth, -0.44 * v4p_displayHeight, v4p_displayWidth * 0.44, v4p_displayHeight * 0.44);
    v4p_setBGColor(V4P_DARK);

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

    // Wrap car around screen edges (like asteroids)
    if (car_x < -v4p_displayWidth / 2) {
        car_x = v4p_displayWidth / 2;
    } else if (car_x > v4p_displayWidth / 2) {
        car_x = -v4p_displayWidth / 2;
    }

    if (car_y < -v4p_displayHeight / 2) {
        car_y = v4p_displayHeight / 2;
    } else if (car_y > v4p_displayHeight / 2) {
        car_y = -v4p_displayHeight / 2;
    }

    // Update car position and rotation (convert degrees to V4P format)
    v4p_transform(car, car_x, car_y, car_angle * 512.f / 360.f, 0, 256, 256);

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