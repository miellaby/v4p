#ifndef PARTICLES_H
#define PARTICLES_H

#include "v4p.h"
#include "quick/quickheap.h"

// Particle structure
typedef struct Particle {
    V4pPolygonP poly;               // The particle polygon (pre-cloned)
    int ttl;                        // Time-to-live counter
    float x, y;                     // Position
    float speed;                    // Current speed
    float move_angle;               // Movement direction angle (degrees)
    float rotation_angle;           // Visual rotation angle (degrees)
    float acceleration;             // Acceleration
    float rotation_speed;           // Rotation speed
    float growth;                   // Growth rate
    float scale;                    // Current scale
    Boolean active;                 // Whether particle is active
} Particle;

// Particle system structure
typedef struct {
    Particle* particles;             // Array of all particles (pre-allocated)
    V4pPolygonP prototype;             // Prototype polygon to clone
    int max_particles;                 // Maximum number of particles
    int active_particles;              // Currently active particles
    
    // System-level parameters
    int default_ttl;                  // Default time-to-live for particles
    float default_speed;              // Default speed for particles
    float default_acceleration;       // Default acceleration (can be negative)
    float default_rotation_speed;     // Default rotation speed
    float default_growth;             // Default growth rate (scale change)
    
    // Noise parameters (0-1 range)
    float speed_noise;                // Noise added to speed
    float angle_noise;                // Noise added to angle
    float rotation_noise;             // Noise added to rotation speed
    float growth_noise;               // Noise added to growth rate
} ParticleSystem;

// Function declarations
ParticleSystem* particles_create(int max_particles, V4pPolygonP prototype);
void particles_destroy(ParticleSystem* system);

// Set system parameters
void particles_set_defaults(ParticleSystem* system, int ttl, float speed, float acceleration, float rotation, float growth);
void particles_set_noise(ParticleSystem* system, float speed_noise, float angle_noise, float rotation_noise, float growth_noise);

// Add a particle to the scene
void particles_emit(ParticleSystem* system, float x, float y, float angle);

// Update all particles in the system
void particles_iterate(ParticleSystem* system, Int32 deltaTime);

// Create a default particle prototype (small square)
V4pPolygonP particles_create_default_prototype();

#endif  // PARTICLES_H