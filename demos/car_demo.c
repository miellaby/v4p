#include <stdio.h>
#include "g4p.h"
#include "v4p.h"
#include "v4pserial.h"

// Car components
V4pPolygonP car_body_proto;
V4pPolygonP car_windows_proto;
V4pPolygonP car_wheels_proto;
V4pPolygonP car_lights_proto;

V4pPolygonP car;

// Car position and speed
V4pCoord car_x = 0;
V4pCoord car_y = 0;
Int32 car_speed = 2;
Int32 car_rotation = 0;

Boolean g4p_onInit(int quality, Boolean fullscreen) {
    v4p_init2(quality, fullscreen);
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
    v4p_setAnchorToCenter(car_body_proto);

    car = v4p_addClone(car_body_proto);
    // Position car at center
    car_x = v4p_displayWidth / 2;
    car_y = v4p_displayHeight / 2;
    
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    // Simple animation - move car horizontally
    car_x += car_speed;
    car_rotation += 1;
    
    // Bounce at edges
    if (car_x > v4p_displayWidth + 100) {
        car_x = -100;
        car_rotation = 0;
    }
    
    // Update all car parts
    v4p_transform(car, car_x, car_y, car_rotation, 1, 256, 256);

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