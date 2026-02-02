#include "g4p.h"
#include "v4p.h"
#include "v4pserial.h"
#include <SDL/SDL.h>
#include <stdlib.h>
#include "lowmath.h"  // For computeCosSin()
#include "addons/game_engine/collision.h"
#include "backends/v4pi.h"  // For v4pi_debug

#define MAX_ASTEROIDS 10
#define MAX_BULLETS 5
#define SHIP_SIZE 28
#define ASTEROID_SIZE 50
#define BULLET_SIZE 5

// Helper function to get sin/cos values using v4p's 256-unit circle system
void getSinCosFromDegrees(float degrees, int* sina, int* cosa) {
    // Convert degrees to v4p's 256-unit circle format
    UInt16 v4p_angle = (UInt16)(degrees * 256.0f / 360.0f);
    computeCosSin(v4p_angle);
    *sina = lwmSina;
    *cosa = lwmCosa;
}

// Game objects
V4pPolygonP ship;
V4pPolygonP asteroids[MAX_ASTEROIDS];
V4pPolygonP bullets[MAX_BULLETS];

// Track positions and angles for objects we can't query
float bullet_x[MAX_BULLETS];
float bullet_y[MAX_BULLETS];
float bullet_angle[MAX_BULLETS];
float asteroid_x[MAX_ASTEROIDS];
float asteroid_y[MAX_ASTEROIDS];
float asteroid_angle[MAX_ASTEROIDS];

// Game state
int score = 0;
int lives = 3;
int asteroid_count = 0;
int bullet_count = 0;
float ship_angle = 0;
float ship_x = 0, ship_y = 0;
Boolean thrusting = false;
Boolean game_over = false;
Boolean ship_invulnerable = false;
int invulnerability_timer = 0;

// Objects to remove (can't remove in callback)
V4pPolygonP bullets_to_remove[MAX_BULLETS];
int bullets_to_remove_count = 0;
V4pPolygonP asteroids_to_remove[MAX_ASTEROIDS];
int asteroids_to_remove_count = 0;

// Ship prototype
V4pPolygonP createShipPrototype() {
    static V4pPolygonP proto = NULL;
    if (proto == NULL) {
        proto = v4p_new(V4P_ABSOLUTE, V4P_WHITE, 1);
        // Create a simple triangle for the ship using v4p_addPoint
        v4p_addPoint(proto, 0, -20);  // Top point
        v4p_addPoint(proto, -10, 20); // Bottom right
        v4p_addPoint(proto, 10, 20); // Bottom left
        v4p_setAnchorToCenter(proto);
    }
    return proto;
}

// Asteroid prototype
V4pPolygonP createAsteroidPrototype() {
    static V4pPolygonP proto = NULL;
    if (proto == NULL) {
        proto = v4p_new(V4P_ABSOLUTE, V4P_GRAY, 2);
        // Create a simple octagon for the asteroid using v4p_addPoint
        v4p_addPoint(proto, 50, 0);
        v4p_addPoint(proto, 35, 35);
        v4p_addPoint(proto, 0, 50);
        v4p_addPoint(proto, -35, 35);
        v4p_addPoint(proto, -50, 0);
        v4p_addPoint(proto, -35, -35);
        v4p_addPoint(proto, 0, -50);
        v4p_addPoint(proto, 35, -35);
        v4p_setAnchorToCenter(proto);
    }
    return proto;
}

// Bullet prototype
V4pPolygonP createBulletPrototype() {
    static V4pPolygonP proto = NULL;
    if (proto == NULL) {
        proto = v4p_new(V4P_ABSOLUTE, V4P_YELLOW, 3);
        // Create a simple square for the bullet
        v4p_rect(proto, -BULLET_SIZE/2, -BULLET_SIZE/2, BULLET_SIZE, BULLET_SIZE);
        v4p_setAnchorToCenter(proto);
    }
    return proto;
}

// Create a new asteroid
void createAsteroid() {
    if (asteroid_count >= MAX_ASTEROIDS) return;
    
    V4pPolygonP asteroid_proto = createAsteroidPrototype();
    asteroids[asteroid_count] = v4p_addClone(asteroid_proto);
    
    // Position asteroid randomly around the edges
    int side = rand() % 4;
    float x, y;
    
    switch (side) {
        case 0: // top
            x = rand() % v4p_displayWidth - v4p_displayWidth/2;
            y = -v4p_displayHeight/2 - 50;
            break;
        case 1: // right
            x = v4p_displayWidth/2 + 50;
            y = rand() % v4p_displayHeight - v4p_displayHeight/2;
            break;
        case 2: // bottom
            x = rand() % v4p_displayWidth - v4p_displayWidth/2;
            y = v4p_displayHeight/2 + 50;
            break;
        case 3: default: // left
            x = -v4p_displayWidth/2 - 50;
            y = rand() % v4p_displayHeight - v4p_displayHeight/2;
            break;
    }
    
    // Random rotation in degrees (0-360)
    float rotation_deg = (rand() % 360);
    
    // Set asteroid properties (convert degrees to V4P format: 0-256)
    asteroid_x[asteroid_count] = x;
    asteroid_y[asteroid_count] = y;
    asteroid_angle[asteroid_count] = rotation_deg;
    
    v4p_transform(asteroids[asteroid_count], x, y, rotation_deg * 256.f / 360.f, 0, 256, 256);
    v4p_setCollisionMask(asteroids[asteroid_count], 2); // Asteroids are on layer 2
    
    asteroid_count++;
}

// Fire a bullet
void fireBullet() {
    if (bullet_count >= MAX_BULLETS) return;
    
    V4pPolygonP bullet_proto = createBulletPrototype();
    bullets[bullet_count] = v4p_addClone(bullet_proto);
    
    // Position bullet at ship's nose using v4p's trigonometric system
    int sina, cosa;
    getSinCosFromDegrees(ship_angle, &sina, &cosa);
    bullet_x[bullet_count] = ship_x + (sina / 256.0f) * SHIP_SIZE;
    bullet_y[bullet_count] = ship_y - (cosa / 256.0f) * SHIP_SIZE;
    bullet_angle[bullet_count] = ship_angle;
    
    v4p_transform(bullets[bullet_count], bullet_x[bullet_count], bullet_y[bullet_count], ship_angle * 256.f / 360.f, 0, 256, 256);
    v4p_setCollisionMask(bullets[bullet_count], 4); // Bullets are on layer 3
    
    bullet_count++;
}



// Collision point callback for finalized averages
void asteroids_onCollisionPoint(V4pPolygonP p1, V4pPolygonP p2, V4pCoord avg_x, V4pCoord avg_y, UInt16 count) {
    // Get collision layers for both polygons
    V4pCollisionMask mask1 = v4p_getCollisionMask(p1);
    V4pCollisionMask mask2 = v4p_getCollisionMask(p2);
    
    // Classify collision based on layers
    Boolean isBullet1 = (mask1 == 4); // Bullet mask is 4
    Boolean isAsteroid1 = (mask1 == 2); // Asteroid mask is 2
    Boolean isShip1 = (p1 == ship);
    
    Boolean isBullet2 = (mask2 == 4); // Bullet mask is 4
    Boolean isAsteroid2 = (mask2 == 2); // Asteroid mask is 2
    Boolean isShip2 = (p2 == ship);

    // Bullet-asteroid collision
    if ((isBullet1 && isAsteroid2) || (isBullet2 && isAsteroid1)) {
        // Bullet hit asteroid
        score += 100;
        
        V4pPolygonP bullet = isBullet1 ? p1 : p2;
        V4pPolygonP asteroid = isAsteroid1 ? p1 : p2;
        
        // Mark bullet for removal
        for (int i = 0; i < bullet_count; i++) {
            if (bullets[i] == bullet) {
                bullets_to_remove[bullets_to_remove_count++] = bullet;
                break;
            }
        }
        
        // Mark asteroid for removal
        for (int i = 0; i < asteroid_count; i++) {
            if (asteroids[i] == asteroid) {
                asteroids_to_remove[asteroids_to_remove_count++] = asteroid;
                break;
            }
        }
    }
    
    // Ship-asteroid collision
    if ((isShip1 && isAsteroid2) || (isShip2 && isAsteroid1)) {
        if (!ship_invulnerable) {
            // Ship hit asteroid
            lives--;
            
            if (lives <= 0) {
                game_over = true;
            } else {
                // Reset ship position and make invulnerable temporarily
                ship_x = 0;
                ship_y = 0;
                ship_angle = 90;
                ship_invulnerable = true;
                invulnerability_timer = 120; // 2 seconds at 60 FPS
            }
        }
    }
}

Boolean g4p_onInit() {
    v4p_init2(V4P_QUALITY_NORMAL, V4P_UX_NORMAL);
    v4p_setView(-v4p_displayWidth / 2, -v4p_displayHeight / 2,
                 v4p_displayWidth / 2, v4p_displayHeight / 2);
    v4p_setBGColor(V4P_BLACK);
    
    // Set collision point callback
    g4p_setCollisionCallback(asteroids_onCollisionPoint);
    
    // Create ship
    V4pPolygonP ship_proto = createShipPrototype();
    ship = v4p_addClone(ship_proto);
    v4p_setCollisionMask(ship, 1); // Ship is on layer 1

    // Create initial asteroids
    for (int i = 0; i < 3; i++) {
        createAsteroid();
    }
    
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    // Handle invulnerability timer
    if (ship_invulnerable) {
        invulnerability_timer--;
        if (invulnerability_timer <= 0) {
            ship_invulnerable = false;
        }
    }
    
    // Remove marked objects
    for (int i = 0; i < bullets_to_remove_count; i++) {
        for (int j = 0; j < bullet_count; j++) {
            if (bullets[j] == bullets_to_remove[i]) {
                v4p_destroyFromScene(bullets[j]);
                // Shift remaining bullets
                for (int k = j; k < bullet_count - 1; k++) {
                    bullets[k] = bullets[k + 1];
                    bullet_x[k] = bullet_x[k + 1];
                    bullet_y[k] = bullet_y[k + 1];
                    bullet_angle[k] = bullet_angle[k + 1];
                }
                bullet_count--;
                break;
            }
        }
    }
    bullets_to_remove_count = 0;
    
    for (int i = 0; i < asteroids_to_remove_count; i++) {
        for (int j = 0; j < asteroid_count; j++) {
            if (asteroids[j] == asteroids_to_remove[i]) {
                v4p_destroyFromScene(asteroids[j]);
                // Shift remaining asteroids
                for (int k = j; k < asteroid_count - 1; k++) {
                    asteroids[k] = asteroids[k + 1];
                    asteroid_x[k] = asteroid_x[k + 1];
                    asteroid_y[k] = asteroid_y[k + 1];
                    asteroid_angle[k] = asteroid_angle[k + 1];
                }
                asteroid_count--;
                break;
            }
        }
    }
    asteroids_to_remove_count = 0;
    
    // Handle input
    if (g4p_state.key == SDLK_LEFT) {
        ship_angle -= 1.f;
    }
    if (g4p_state.key == SDLK_RIGHT) {
        ship_angle += 1.f;
    }
    
    thrusting = false;
    if (g4p_state.key == SDLK_UP) {
        thrusting = true;
        // Move ship forward using v4p's trigonometric system
        int sina, cosa;
        getSinCosFromDegrees(ship_angle, &sina, &cosa);
        ship_x += (sina / 256.0f) * 2.;
        ship_y -= (cosa / 256.0f) * 2.;
    }
    
    if (g4p_state.key == SDLK_SPACE) {
        fireBullet();
    }
    
    // Update ship position and rotation (convert degrees to V4P format)
    v4p_transform(ship, ship_x, ship_y, ship_angle * 256.f / 360.f, 0, 256, 256);
    
    // Update bullets - move them forward
    for (int i = 0; i < bullet_count; i++) {
        // Move bullet in its own direction
        int sina, cosa;
        getSinCosFromDegrees(bullet_angle[i], &sina, &cosa);
        
        // Move bullet forward
        bullet_x[i] += (sina / 256.0f) * 5.;
        bullet_y[i] -= (cosa / 256.0f) * 5.;
        
        // Update bullet position
        v4p_transform(bullets[i], bullet_x[i], bullet_y[i], bullet_angle[i] * 256.f / 360.f, 0, 256, 256);
        
        // Remove bullets that go off screen
        if (bullet_x[i] < -v4p_displayWidth/2 - 50 || bullet_x[i] > v4p_displayWidth/2 + 50 ||
            bullet_y[i] < -v4p_displayHeight/2 - 50 || bullet_y[i] > v4p_displayHeight/2 + 50) {
            v4p_destroyFromScene(bullets[i]);
            // Shift remaining bullets
            for (int j = i; j < bullet_count - 1; j++) {
                bullets[j] = bullets[j + 1];
                bullet_x[j] = bullet_x[j + 1];
                bullet_y[j] = bullet_y[j + 1];
                bullet_angle[j] = bullet_angle[j + 1];
            }
            bullet_count--;
            i--; // Adjust index after removal
        }
    }
    
    // Update asteroids - rotate them
    for (int i = 0; i < asteroid_count; i++) {
        // Rotate asteroid slightly
        asteroid_angle[i] += 1.f;
        
        // Move asteroid forward slightly
        int sina, cosa;
        getSinCosFromDegrees(asteroid_angle[i], &sina, &cosa);
        asteroid_x[i] += (sina / 256.0f) * 0.5f;
        asteroid_y[i] -= (cosa / 256.0f) * 0.5f;
        
        // Update asteroid transform
        v4p_transform(asteroids[i], asteroid_x[i], asteroid_y[i], asteroid_angle[i] * 256.f / 360.f, 0, 256, 256);
    }
    
    // Wrap ship around screen edges
    if (ship_x < -v4p_displayWidth/2) {
        ship_x = v4p_displayWidth/2;
    } else if (ship_x > v4p_displayWidth/2) {
        ship_x = -v4p_displayWidth/2;
    }
    
    if (ship_y < -v4p_displayHeight/2) {
        ship_y = v4p_displayHeight/2;
    } else if (ship_y > v4p_displayHeight/2) {
        ship_y = -v4p_displayHeight/2;
    }

    // Create new asteroid to keep game going
    if (asteroid_count < MAX_ASTEROIDS && rand() % 60 == 0) {
        createAsteroid();
    }

    return game_over ? failure : success;  // Return failure to exit when game is over
}

Boolean g4p_onFrame() {
    // Render frame
    v4p_render();
    return success;
}

void g4p_onQuit() {
    v4p_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}
