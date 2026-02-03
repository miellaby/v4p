#include "g4p.h"
#include "v4p.h"
#include "v4pserial.h"
#include <SDL/SDL.h>
#include <stdlib.h>
#include <math.h>    // For fabs()
#include "lowmath.h"  // For computeCosSin()
#include "addons/game_engine/collision.h"
#include "addons/qfont/qfont.h"  // For score display
#include "backends/v4pi.h"  // For v4pi_debug

#define MAX_ASTEROIDS 15
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
static V4pPolygonP score_poly = NULL;  // Score display polygon
#define MAX_LIVES 5
V4pPolygonP life_indicators[MAX_LIVES];  // Life indicators (small ships)
int life_count = 0;

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
float ship_speed_x = 0, ship_speed_y = 0;  // Ship's speed/moment vector
Boolean thrusting = false;
Boolean game_over = false;
int invulnerability_timer = 120;

// Objects to remove (can't remove in callback)
V4pPolygonP bullets_to_remove[MAX_BULLETS];
int bullets_to_remove_count = 0;
V4pPolygonP asteroids_to_remove[MAX_ASTEROIDS];
int asteroids_to_remove_count = 0;

// Ship prototype
V4pPolygonP getShipPrototypeSingleton() {
    static V4pPolygonP proto = NULL;
    if (proto == NULL) {
        proto = v4p_new(V4P_ABSOLUTE, V4P_WHITE, 0);
        // Create a simple triangle for the ship using v4p_addPoint
        v4p_addPoint(proto, 0, -20);  // Top point
        v4p_addPoint(proto, -10, 20); // Bottom right
        v4p_addPoint(proto, 10, 20); // Bottom left
        v4p_setAnchorToCenter(proto);
    }
    return proto;
}

// Asteroid prototype
V4pPolygonP getAsteroidPrototypeSingleton() {
    static V4pPolygonP proto = NULL;
    if (proto == NULL) {
        proto = v4p_new(V4P_ABSOLUTE, V4P_MAROON, 2);
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
        proto = v4p_new(V4P_ABSOLUTE, V4P_YELLOW, 0);
        // Create a simple square for the bullet
        v4p_rect(proto, -BULLET_SIZE/2, -BULLET_SIZE/2, BULLET_SIZE, BULLET_SIZE);
        v4p_setAnchorToCenter(proto);
    }
    return proto;
}

// Initialize life indicators
void initLifeIndicators() {
    // Clear any existing life indicators
    for (int i = 0; i < life_count; i++) {
        if (life_indicators[i]) {
            v4p_destroyFromScene(life_indicators[i]);
            life_indicators[i] = NULL;
        }
    }
    life_count = 0;
    
    // Create life indicators based on current lives
    V4pPolygonP ship_proto = getShipPrototypeSingleton();
    
    for (int i = 0; i < lives && i < MAX_LIVES; i++) {
        // Create a small ship for life indicator
        life_indicators[i] = v4p_addClone(ship_proto);
        v4p_setRelative(life_indicators[i], true);
        
        // Position in top-right corner, spaced out
        float x = v4p_displayWidth - 30 - (i * 25);
        float y = 20;
        
        // Scale down to make them smaller
        
        v4p_transform(life_indicators[i], x, y, 0, 0, 128, 128); // 50% scale
        
        life_count = i + 1;
    }
}

// Remove a life indicator
void removeLifeIndicator() {
    if (life_count > 0) {
        life_count--;
        if (life_indicators[life_count]) {
            v4p_destroyFromScene(life_indicators[life_count]);
            life_indicators[life_count] = NULL;
        }
    }
}

// Create a new asteroid
void createAsteroid() {
    if (asteroid_count >= MAX_ASTEROIDS) return;
    
    V4pPolygonP asteroid_proto = getAsteroidPrototypeSingleton();
    asteroids[asteroid_count] = v4p_addClone(asteroid_proto);
    v4p_setLayer(asteroids[asteroid_count], asteroid_count % 13 + 1);
    
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
        if (invulnerability_timer == 0) {
            // Ship hit asteroid
            lives--;
            removeLifeIndicator();
            
            if (lives <= 0) {
                game_over = true;
            } else {
                // Reset ship position and make invulnerable temporarily
                ship_x = 0;
                ship_y = 0;
                ship_angle = 90;
                invulnerability_timer = 120; // 4 seconds at 60 FPS
            }
        }
    }
}

Boolean g4p_onInit() {
    v4p_init2(V4P_QUALITY_NORMAL, V4P_UX_NORMAL);
    v4p_setView(-0.44 * v4p_displayWidth, -0.44 * v4p_displayHeight,
                 v4p_displayWidth * 0.44, v4p_displayHeight * 0.44);
    v4p_setBGColor(V4P_BLACK);
    
    // Set collision point callback
    g4p_setCollisionCallback(asteroids_onCollisionPoint);
    
    // Create ship
    V4pPolygonP ship_proto = getShipPrototypeSingleton();
    ship = v4p_addClone(ship_proto);
    v4p_setCollisionMask(ship, 1); // Ship is on layer 1

    // Initialize life indicators
    initLifeIndicators();

    // Create score display polygon
    score_poly = v4p_addNew(V4P_RELATIVE, V4P_WHITE, 15);
    
    // Create initial asteroids
    for (int i = 0; i < 1; i++) {
        createAsteroid();
    }
    
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    static int totalTime = 0;
    totalTime += deltaTime;
    int level = 3 + score / 1000;

    // remember last key press
    static UInt16 last_key = 0;

    // Handle invulnerability timer and blinking effect
    if (invulnerability_timer > 0) {
        invulnerability_timer--;
        if (invulnerability_timer <= 0) {
            // Make sure ship is visible when invulnerability ends
            v4p_enable(ship);
        } else {
            // Create blinking effect using totalTime modulo
            // Blink every 8 frames (4 frames visible, 4 frames invisible)
            if ((totalTime / 200) % 2 == 0) {
                v4p_enable(ship);
            } else {
                v4p_disable(ship);
            }
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
        ship_angle -= 3.f;
    }
    if (g4p_state.key == SDLK_RIGHT) {
        ship_angle += 3.f;
    }
    
    thrusting = false;
    if (g4p_state.key == SDLK_UP) {
        thrusting = true;
        // Apply thrust in the direction the ship is facing
        int sina, cosa;
        getSinCosFromDegrees(ship_angle, &sina, &cosa);
        ship_speed_x += (sina / 256.0f) * 0.1f;  // Accelerate in thrust direction
        ship_speed_y -= (cosa / 256.0f) * 0.1f;
    }
    
    // Apply friction/deceleration when no thrust
    if (g4p_state.key != SDLK_UP) {
        ship_speed_x *= 0.98f;  // Slow down gradually
        ship_speed_y *= 0.98f;
        
        // Small threshold to stop completely
        if (fabs(ship_speed_x) < 0.01f) ship_speed_x = 0;
        if (fabs(ship_speed_y) < 0.01f) ship_speed_y = 0;
    }
    
    // Apply the speed vector to ship position
    ship_x += ship_speed_x;
    ship_y += ship_speed_y;
    
    // Space bar: fire bullet AND apply extra thrust
    if (g4p_state.key == SDLK_SPACE) {
        if (last_key != SDLK_SPACE) {
            fireBullet();
        }
        
        // Apply extra thrust when space bar is held down
        int sina, cosa;
        getSinCosFromDegrees(ship_angle, &sina, &cosa);
        ship_speed_x += (sina / 256.0f) * 0.2f;  // Stronger acceleration on space
        ship_speed_y -= (cosa / 256.0f) * 0.2f;
    }
    last_key = g4p_state.key;

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
        // Move asteroid forward slightly
        int sina, cosa;
        getSinCosFromDegrees(asteroid_angle[i], &sina, &cosa);
        asteroid_x[i] += (sina / 256.0f) * 0.5f;
        asteroid_y[i] -= (cosa / 256.0f) * 0.5f;

        // Wrap asteroid around screen edges
        if (asteroid_x[i] < -v4p_displayWidth / 2) {
            asteroid_x[i] = v4p_displayWidth / 2;
        } else if (asteroid_x[i] > v4p_displayWidth / 2) {
            asteroid_x[i] = -v4p_displayWidth / 2;
        }

        if (asteroid_y[i] < -v4p_displayHeight / 2) {
            asteroid_y[i] = v4p_displayHeight / 2;
        } else if (asteroid_y[i] > v4p_displayHeight / 2) {
            asteroid_y[i] = -v4p_displayHeight / 2;
        }

        if (ship_y < -v4p_displayHeight / 2) {
            ship_y = v4p_displayHeight / 2;
        } else if (ship_y > v4p_displayHeight / 2) {
            ship_y = -v4p_displayHeight / 2;
        }

        // Update asteroid transform
        v4p_transform(asteroids[i], asteroid_x[i], asteroid_y[i], (asteroid_angle[i] + totalTime * 0.01f) * 256.f / 360.f, 0, 256, 256);
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
    if (asteroid_count < (level % MAX_ASTEROIDS) && rand() % 60 == 0) {
        createAsteroid();
    }

    // Update score display
    if (score_poly) {
        // Remove and destroy the old score polygon
        v4p_destroyFromScene(score_poly);
    }
    
    // Create new score polygon with current score
    score_poly = v4p_addNew(V4P_RELATIVE, V4P_BLUE, 15 /* top layer */);
    qfontDefinePolygonFromInt(score, score_poly, 10, 10, 20, 20, 5);

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
