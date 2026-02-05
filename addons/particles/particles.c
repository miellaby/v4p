#include "particles.h"
#include "v4p.h"
#include "quick/quickheap.h"
#include "lowmath.h"
#include <stdlib.h>
#include <math.h>

// Helper function to add noise to a value
static float add_noise(float value, float noise_amount) {
    if (noise_amount <= 0) return value;
    float noise = (rand() % 1000) / 1000.0f * 2.0f - 1.0f; // -1 to 1
    return value + value * noise * noise_amount;
}

// Create a new particle system
ParticleSystem* particles_create(int max_particles, V4pPolygonP prototype) {
    ParticleSystem* system = (ParticleSystem*)malloc(sizeof(ParticleSystem));
    if (!system) return NULL;
    
    // Initialize the system
    system->max_particles = max_particles;
    system->active_particles = 0;
    system->prototype = prototype;
    
    // Set default parameters
    system->default_ttl = 60;  // 1 second at 60 FPS
    system->default_speed = 2.0f;
    system->default_acceleration = 0.0f;  // No acceleration by default
    system->default_rotation_speed = 0.0f;  // No rotation by default
    system->default_growth = 0.0f;  // No growth by default
    
    // Set default noise parameters
    system->speed_noise = 0.2f;
    system->angle_noise = 0.1f;
    system->rotation_noise = 0.2f;
    system->growth_noise = 0.1f;
    
    // Allocate particle array
    system->particles = (Particle*)malloc(max_particles * sizeof(Particle));
    if (!system->particles) {
        free(system);
        return NULL;
    }
    
    // Pre-clone all polygons and initialize particles
    for (int i = 0; i < max_particles; i++) {
        system->particles[i].poly = v4p_clone(prototype);
        system->particles[i].active = false;
        system->particles[i].ttl = 0;
        system->particles[i].x = 0;
        system->particles[i].y = 0;
        system->particles[i].speed = 0;
        system->particles[i].move_angle = 0;
        system->particles[i].rotation_angle = 0;
        system->particles[i].acceleration = 0;
        system->particles[i].rotation_speed = 0;
        system->particles[i].growth = 0;
        system->particles[i].scale = 1.0f;
        
        // Start with polygons removed from scene (they'll be added when emitted)
        // Note: v4p_clone doesn't add to scene, so no need to remove
    }
    
    return system;
}

// Destroy a particle system
void particles_destroy(ParticleSystem* system) {
    if (!system) return;
    
    // Destroy all particle polygons
    for (int i = 0; i < system->max_particles; i++) {
        if (system->particles[i].poly) {
            // Remove from scene only if active (in scene), then destroy
            if (system->particles[i].active)
                v4p_remove(system->particles[i].poly);
            v4p_destroy(system->particles[i].poly);
            system->particles[i].poly = NULL;
        }
    }
    
    // Free memory
    free(system->particles);
    free(system);
}

// Set default system parameters
void particles_set_defaults(ParticleSystem* system, int ttl, float speed, float acceleration, float rotation, float growth) {
    if (!system) return;
    
    system->default_ttl = ttl;
    system->default_speed = speed;
    system->default_acceleration = acceleration;
    system->default_rotation_speed = rotation;
    system->default_growth = growth;
}

// Set noise parameters
void particles_set_noise(ParticleSystem* system, float speed_noise, float angle_noise, float rotation_noise, float growth_noise) {
    if (!system) return;
    
    system->speed_noise = speed_noise;
    system->angle_noise = angle_noise;
    system->rotation_noise = rotation_noise;
    system->growth_noise = growth_noise;
}

// Create a default particle prototype (small square)
V4pPolygonP particles_create_default_prototype() {
    V4pPolygonP proto = v4p_new(V4P_ABSOLUTE, V4P_WHITE, 0);
    if (!proto) return NULL;
    
    // Create a small square
    v4p_rect(proto, -2, -2, 4, 4);
    v4p_setAnchorToCenter(proto);
    
    return proto;
}

// Find an available particle slot
static int find_available_particle(ParticleSystem* system) {
    for (int i = 0; i < system->max_particles; i++) {
        if (!system->particles[i].active) {
            return i;
        }
    }
    return -1; // No available slots
}

// Emit a new particle
void particles_emit(ParticleSystem* system, float x, float y, float angle) {
    if (!system || system->active_particles >= system->max_particles) return;
    
    // Find an available particle slot
    int particle_index = find_available_particle(system);
    if (particle_index == -1) return;
    
    Particle* particle = &system->particles[particle_index];
    
    // Enable the polygon (add to scene)
    v4p_add(particle->poly);
    
    // Set particle properties with noise
    particle->ttl = system->default_ttl;
    particle->x = x;
    particle->y = y;
    particle->speed = add_noise(system->default_speed, system->speed_noise);
    particle->move_angle = add_noise(angle, system->angle_noise);
    particle->rotation_angle = add_noise(angle, system->angle_noise); // Initialize rotation to move angle
    particle->acceleration = add_noise(system->default_acceleration, system->speed_noise);
    particle->rotation_speed = add_noise(system->default_rotation_speed, system->rotation_noise);
    particle->growth = add_noise(system->default_growth, system->growth_noise);
    particle->scale = 1.0f;
    particle->active = true;
    
    // Transform the particle - computeCosSin/v4p_transform will handle angle wrapping
    int v4p_angle = (int)(particle->rotation_angle * 512.0f / 360.0f);
    v4p_transform(particle->poly, x, y, (UInt16)v4p_angle, 0, 256, 256);

    system->active_particles++;
}

// Update all particles in the system
void particles_iterate(ParticleSystem* system) {
    if (!system) return;
    
    for (int i = 0; i < system->max_particles; i++) {
        Particle* particle = &system->particles[i];
        
        if (!particle->active) continue;
        
        // Update TTL
        particle->ttl--;
        if (particle->ttl <= 0) {
            // Particle has expired - remove from scene
            v4p_remove(particle->poly);
            particle->active = false;
            system->active_particles--;
            continue;
        }
        
        // Update speed with acceleration
        particle->speed += particle->acceleration;
        
        // Update position based on speed and move angle
        int sina, cosa;
        // Convert to V4P angle format - computeCosSin will handle wrapping via bitmasking
        int v4p_move_angle = (int)(particle->move_angle * 512.0f / 360.0f);
        computeCosSin(v4p_move_angle);
        sina = lwmSina;
        cosa = lwmCosa;
        
        particle->x += (sina / 256.0f) * particle->speed;
        particle->y -= (cosa / 256.0f) * particle->speed;
        
        // Update rotation angle separately from movement angle
        particle->rotation_angle += particle->rotation_speed;
        
        // Update scale with growth
        particle->scale += particle->growth;
        
        // Transform the particle using double modulo for proper V4P angle wrapping
        int scale_int = (int)(particle->scale * 256.0f);
        int v4p_angle = ((int)(particle->rotation_angle * 512.0f / 360.0f) % 512 + 512) % 512;
        v4p_transform(particle->poly, 
                      particle->x, 
                      particle->y, 
                      (UInt16)v4p_angle, 
                      0, 
                      scale_int, 
                      scale_int);
    }
}