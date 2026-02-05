#include "v4p.h"
#include "g4p.h"
#include "addons/particles/particles.h"

// Global particle system
ParticleSystem* test_system = NULL;

Boolean g4p_onInit() {
    v4p_init2(V4P_QUALITY_NORMAL, V4P_UX_NORMAL);
    v4p_setView(-0.44 * v4p_displayWidth, -0.44 * v4p_displayHeight,
                 v4p_displayWidth * 0.44, v4p_displayHeight * 0.44);
    v4p_setBGColor(V4P_BLACK);
    
    // Create particle system
    V4pPolygonP particle_proto = particles_create_default_prototype();
    v4p_setColor(particle_proto, V4P_YELLOW);
    
    test_system = particles_create(10, particle_proto);
    particles_set_defaults(test_system, 120, 1.0f, 0.0f, 1.0f, 0.0f);
    particles_set_noise(test_system, 0.1f, 0.1f, 0.1f, 0.0f);
    
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    static int counter = 0;
    counter++;
    
    // Emit particles every 20 frames
    if (counter % 20 == 0) {
        particles_emit(test_system, 0, 0, counter * 3);
    }
    
    // Update particle system
    if (test_system) {
        particles_iterate(test_system);
    }
    
    return success;
}

Boolean g4p_onFrame() {
    v4p_render();
    return success;
}

void g4p_onQuit() {
    if (test_system) {
        particles_destroy(test_system);
        test_system = NULL;
    }
    v4p_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}