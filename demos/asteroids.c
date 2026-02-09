#include "g4p.h"
#include "v4p.h"
#include "v4pserial.h"
#include <stdlib.h>
#include <math.h>    // For fabs()
#include "lowmath.h"  // For computeCosSin()
#include "addons/game_engine/collision.h"
#include "addons/qfont/qfont.h"  // For score display
#include "addons/particles/particles.h"  // For particle effects
#include "backends/v4pi.h"  // For v4pi_debug
#ifdef DEBUG
    #include "addons/debug/debug.h"  // For debug functions
#endif

#define MAX_ASTEROIDS 15
#define MAX_BULLETS 5
#define SHIP_SIZE 16
#define ASTEROID_SIZE 50
#define BULLET_SIZE 5

// Helper function to get sin/cos values using v4p's 512-unit circle system
void getSinCosFromDegrees(float degrees, int* sina, int* cosa) {
    // Convert degrees to v4p's 512-unit circle format
    // computeCosSin will handle angle wrapping via bitmasking (angle & 0x1FF)
    int v4p_angle = (int)(degrees * 512.0f / 360.0f);
    computeCosSin((UInt16)v4p_angle);
    *sina = lwmSina;
    *cosa = lwmCosa;
}

// Game objects
V4pPolygonP ship;
V4pPolygonP ship_flame = NULL;  // Reference to the ship's flame polygon
V4pPolygonP asteroids[MAX_ASTEROIDS];
V4pPolygonP bullets[MAX_BULLETS];
static V4pPolygonP score_poly = NULL;  // Score display polygon
#define MAX_LIVES 5
V4pPolygonP life_indicators[MAX_LIVES];  // Life indicators (small ships)
int life_count = 0;

// Particle systems
ParticleSystem* explosion_system = NULL;
ParticleSystem* thrust_system = NULL;

// Track positions and angles for objects we can't query
float bullet_x[MAX_BULLETS];
float bullet_y[MAX_BULLETS];
float bullet_angle[MAX_BULLETS];
int bullet_ttl[MAX_BULLETS];  // Time To Live for each bullet (in frames)
float asteroid_x[MAX_ASTEROIDS];
float asteroid_y[MAX_ASTEROIDS];
float asteroid_angle[MAX_ASTEROIDS];
float asteroid_speed[MAX_ASTEROIDS];  // Individual speed for each asteroid
int asteroid_size[MAX_ASTEROIDS];  // 0=big, 1=medium, 2=small

// Game state
typedef enum {
    GAME_MODE_TITLE,
    GAME_MODE_PLAYING,
    GAME_MODE_GAMEOVER
} GameMode;

GameMode game_mode = GAME_MODE_TITLE;
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

// Title/Gameover mode text
V4pPolygonP title_poly = NULL;
V4pPolygonP gameover_poly = NULL;
V4pPolygonP press_space_poly = NULL;

// Long press detection for spacebar
int space_press_timer = 0;
#define LONG_PRESS_THRESHOLD 60  // About 1 second at 60 FPS

// Ship prototype
static V4pPolygonP shipProto = NULL;
V4pPolygonP getShipPrototypeSingleton() {
    if (shipProto == NULL) {
        shipProto = v4p_new(V4P_ABSOLUTE, V4P_WHITE, 0);
        // Create a a classic V shape ship (like the original Asteroids)
        v4p_addPoint(shipProto, 0, -20);    // Top point of chevron
        v4p_addPoint(shipProto, -15, 15);    // Left point of chevron
        v4p_addPoint(shipProto, 0, 10);     // Bottom center point
        v4p_addPoint(shipProto, 15, 15);     // Right point of chevron
        v4p_addPoint(shipProto, 0, -20);    // Back to top point to close the shape
        v4p_setAnchorToCenter(shipProto);
        
        // Add flame as a subparent (will be cloned with the ship)
        V4pPolygonP flame = v4p_addNewSub(shipProto, V4P_ABSOLUTE, V4P_RED, 0);
        // Create a simple flame for the chevron ship
        v4p_addPoint(flame, 0, 25);    // Tip of flame (pointing downward)
        v4p_addPoint(flame, -8, 5);   // Left base point
        v4p_addPoint(flame, 8, 5);    // Right base point
        v4p_setAnchorToCenter(flame);
        // Position flame relative to ship's back
        v4p_transform(flame, 0, 10, 0, 0, 256, 256); // Move down from ship center
    }
    return shipProto;
}

// Get a rock prototype in different shapes
static V4pPolygonP rockProto[2] = { NULL, NULL };
V4pPolygonP getAsteroidPrototypeSingleton() {
    static int initialized = 0;
    
    if (!initialized) {
        // Prototype 1: Octagon (original)
        rockProto[0] = v4p_new(V4P_ABSOLUTE, V4P_MAROON, 2);
        v4p_addPoint(rockProto[0], 45 /* 50 */, 0);
        v4p_addPoint(rockProto[0], 35, 35);
        v4p_addPoint(rockProto[0], 0, 50);
        v4p_addPoint(rockProto[0], -35, 35);
        v4p_addPoint(rockProto[0], -50, 0);
        v4p_addPoint(rockProto[0], -35, -35);
        v4p_addPoint(rockProto[0], 0, -50);
        v4p_addPoint(rockProto[0], 35, -35);
        v4p_setAnchorToCenter(rockProto[0]);

        rockProto[1] = v4p_new(V4P_ABSOLUTE, V4P_MAROON, 2);
        v4p_addPoint(rockProto[1], 20, -5);
        v4p_addPoint(rockProto[1], 20, 5);
        v4p_addPoint(rockProto[1], 35, 35);
        v4p_addPoint(rockProto[1], 0, 50);
        v4p_addPoint(rockProto[1], -30, 35);
        v4p_addPoint(rockProto[1], -50, 0);
        v4p_addPoint(rockProto[1], -35, -35);
        v4p_addPoint(rockProto[1], 0, -50);
        v4p_addPoint(rockProto[1], 35, -35);
        v4p_setAnchorToCenter(rockProto[1]);

        initialized = 1;
    }
    
    // Return a random prototype
    return rockProto[rand() % 2];
}

// Bullet prototype
static V4pPolygonP bulletProto = NULL;
V4pPolygonP createBulletPrototype() {
    if (bulletProto == NULL) {
        bulletProto = v4p_new(V4P_ABSOLUTE, V4P_YELLOW, 0);
        // Create a simple square for the bullet
        v4p_rect(bulletProto, -BULLET_SIZE/2, -BULLET_SIZE/2, BULLET_SIZE/2, BULLET_SIZE/2);
        v4p_setAnchorToCenter(bulletProto);
    }
    return bulletProto;
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
        
        v4p_transform(life_indicators[i], x, y, 32, 31, 128, 128); // 50% scale
        
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

// Create title screen text
void createTitleScreen() {
    // Clear any existing title text
    if (title_poly) {
        v4p_destroyFromScene(title_poly);
        title_poly = NULL;
    }
    if (press_space_poly) {
        v4p_destroyFromScene(press_space_poly);
        press_space_poly = NULL;
    }
    
    // Create title text - centered on screen
    title_poly = v4p_addNew(V4P_RELATIVE, V4P_WHITE, 15);
    qfontDefinePolygonFromString("ASTEROIDS", title_poly, 
                                v4p_displayWidth / 2 - 40,  // Center horizontally
                                v4p_displayHeight / 2 - 10,  // Center vertically
                                12, 12, 2);
    
    // Create press space text - centered below title
    press_space_poly = v4p_addNew(V4P_RELATIVE, V4P_YELLOW, 15);
    qfontDefinePolygonFromString("LONG PRESS SPACE", press_space_poly,
                                v4p_displayWidth / 2 - 50,  // Center horizontally
                                v4p_displayHeight / 2 + 10,   // Below title
                                8, 8, 1);
}

// Create gameover screen text
void createGameOverScreen() {
    // Clear any existing gameover text
    if (gameover_poly) {
        v4p_destroyFromScene(gameover_poly);
        gameover_poly = NULL;
    }
    if (press_space_poly) {
        v4p_destroyFromScene(press_space_poly);
        press_space_poly = NULL;
    }
    
    // Create gameover text - centered on screen
    gameover_poly = v4p_addNew(V4P_RELATIVE, V4P_RED, 15);
    qfontDefinePolygonFromString("GAME OVER", gameover_poly,
                                v4p_displayWidth / 2 - 40,  // Center horizontally
                                v4p_displayHeight / 2 - 10,  // Center vertically
                                12, 12, 2);
    
    // Create press space text - centered below gameover
    press_space_poly = v4p_addNew(V4P_RELATIVE, V4P_YELLOW, 15);
    qfontDefinePolygonFromString("LONG PRESS SPACE",
                                 press_space_poly,
                                 v4p_displayWidth / 2 - 50,  // Center horizontally
                                 v4p_displayHeight / 2 + 10,  // Below gameover
                                 8,
                                 8,
                                 1);
}

// Clear title/gameover screen
void clearTitleGameOverScreen() {
    if (title_poly) {
        v4p_destroyFromScene(title_poly);
        title_poly = NULL;
    }
    if (gameover_poly) {
        v4p_destroyFromScene(gameover_poly);
        gameover_poly = NULL;
    }
    if (press_space_poly) {
        v4p_destroyFromScene(press_space_poly);
        press_space_poly = NULL;
    }
}

// Forward declaration for resetGameState
void resetGameState();

// Create a new asteroid
void createAsteroid() {
    static unsigned createdAsteroids = 0;
    if (asteroid_count >= MAX_ASTEROIDS) return;
    
    V4pPolygonP asteroid_proto = getAsteroidPrototypeSingleton();
    asteroids[asteroid_count] = v4p_addClone(asteroid_proto);
    v4p_setLayer(asteroids[asteroid_count], createdAsteroids++ % 30 + 1);
    v4p_setColor(asteroids[asteroid_count], 139 + v4p_getId(asteroids[asteroid_count]) % 14);

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
    asteroid_speed[asteroid_count] = 0.3f + (rand() % 5) * 0.1f; // Random speed between 0.3 and 0.8
    asteroid_size[asteroid_count] = 0; // Default size is big (0)
    
    v4p_transform(asteroids[asteroid_count], x, y, rotation_deg * 512.f / 360.f, 0, 256, 256);
    v4p_setCollisionMask(asteroids[asteroid_count], 2); // Asteroids are on layer 2
    
    asteroid_count++;
}

// Reset game state for new game
void resetGameState() {
    // Clear all game objects
    for (int i = 0; i < asteroid_count; i++) {
        if (asteroids[i]) {
            v4p_destroyFromScene(asteroids[i]);
            asteroids[i] = NULL;
        }
    }
    asteroid_count = 0;
    // Clear asteroid sizes and speeds arrays
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        asteroid_size[i] = 0;
        asteroid_speed[i] = 0.0f;
    }
    
    for (int i = 0; i < bullet_count; i++) {
        if (bullets[i]) {
            v4p_destroyFromScene(bullets[i]);
            bullets[i] = NULL;
        }
    }
    bullet_count = 0;
    
    // Reset ship position and state
    ship_x = 0;
    ship_y = 0;
    ship_angle = 90;
    ship_speed_x = 0;
    ship_speed_y = 0;
    
    // Reset game state
    lives = 3;
    score = 0;
    game_over = false;
    invulnerability_timer = 120;
    
    // Reinitialize life indicators
    initLifeIndicators();
    
    // Create initial asteroids
    for (int i = 0; i < 1; i++) {
        createAsteroid();
    }
}

// Fire a bullet
void fireBullet() {
    
    V4pPolygonP bullet_proto = createBulletPrototype();
    bullets[bullet_count] = v4p_addClone(bullet_proto);
    
    // Position bullet at ship's nose using v4p's trigonometric system
    int sina, cosa;
    getSinCosFromDegrees(ship_angle, &sina, &cosa);
    bullet_x[bullet_count] = ship_x + (sina / 256.0f) * SHIP_SIZE;
    bullet_y[bullet_count] = ship_y - (cosa / 256.0f) * SHIP_SIZE;
    bullet_angle[bullet_count] = ship_angle;
    bullet_ttl[bullet_count] = 80; // 1 second + at 60 FPS
    
    v4p_transform(bullets[bullet_count], bullet_x[bullet_count], bullet_y[bullet_count], ship_angle * 512.f / 360.f, 0, 256, 256);
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
        V4pPolygonP bullet = isBullet1 ? p1 : p2;
        V4pPolygonP asteroid = isAsteroid1 ? p1 : p2;
        
        // Find the asteroid size for scoring
        int asteroid_score = 100; // default for big asteroids
        for (int i = 0; i < asteroid_count; i++) {
            if (asteroids[i] == asteroid) {
                if (asteroid_size[i] == 1) { // medium
                    asteroid_score = 150;
                } else if (asteroid_size[i] == 2) { // small
                    asteroid_score = 200;
                }
                break;
            }
        }
        score += asteroid_score;
        
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
                
                // Create explosion particles at asteroid position
                if (explosion_system) {
                    for (int j = 0; j < 10; j++) {  // 10 particles per explosion
                        particles_emit(explosion_system, asteroid_x[i], asteroid_y[i], asteroid_angle[i] + (rand() % 360));
                    }
                }
                break;
            }
        }
    }
    
    // Ship-asteroid collision
    if ((isShip1 && isAsteroid2) || (isShip2 && isAsteroid1)) {
        if (invulnerability_timer == 0) {
            // Ship hit asteroid
            if (explosion_system) {
                for (int j = 0; j < 10; j++) {  // 10 particles per explosion
                    particles_emit(explosion_system, ship_x, ship_y, (rand() % 360));
                }
            }

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

Boolean g4p_onInit(int quality, Boolean fullscreen) {
    v4p_init2(quality, fullscreen);
    v4p_setView(-0.44 * v4p_displayWidth, -0.44 * v4p_displayHeight,
                 v4p_displayWidth * 0.44, v4p_displayHeight * 0.44);
    v4p_setBGColor(V4P_BLACK);
    
    // Set collision point callback
    g4p_setCollisionCallback(asteroids_onCollisionPoint);
    
    // Create ship
    V4pPolygonP ship_proto = getShipPrototypeSingleton();
    ship = v4p_addClone(ship_proto);
    v4p_setCollisionMask(ship, 1); // Ship is on layer 1
    
    // Get reference to the flame sub-polygon using the new tree traversal functions
    // The flame was added as the first sub-polygon in getShipPrototypeSingleton
    ship_flame = v4p_getFirstSub(ship);
    
    v4p_disable(ship); // Start with ship disabled in title mode

    // Initialize life indicators
    initLifeIndicators();

    // Create score display polygon
    score_poly = v4p_addNew(V4P_RELATIVE, V4P_WHITE, 31);
    
    // Start in title mode
    game_mode = GAME_MODE_TITLE;
    createTitleScreen();
    
    // Initialize particle systems
    V4pPolygonP particle_proto = particles_create_default_prototype();
    
    // Explosion system - for asteroid destruction
    explosion_system = particles_create(50, particle_proto);
    particles_set_defaults(explosion_system, 60, 1.5f, -0.01f, 5.0f, 0.0f);
    particles_set_noise(explosion_system, 0.5f, 0.3f, 0.5f, 0.2f);
    
    // Thrust system - for ship engine
    thrust_system = particles_create(20, particle_proto);
    particles_set_defaults(thrust_system, 30, 2.f, -0.05f, 2.0f, 0.0f);
    particles_set_noise(thrust_system, 0.3f, 0.2f, 0.3f, 0.1f);
    
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    static int totalTime = 0;
    totalTime += deltaTime;
    int level = 3 + score / 1000;

    // remember last space button state
    static Boolean last_space_pressed = false;

    // Handle mode switching based on spacebar long press
    if (g4p_state.buttons[G4P_SPACE]) {  // Space button
        space_press_timer++;
        
        // Long press detected - switch modes
        if (space_press_timer >= LONG_PRESS_THRESHOLD) {
            if (game_mode == GAME_MODE_TITLE) {
                // Start new game from title screen
                game_mode = GAME_MODE_PLAYING;
                clearTitleGameOverScreen();
                v4p_enable(ship); // Enable ship for gameplay
                v4p_disable(ship_flame); // Start with flame disabled
                resetGameState(); // Reset game state
            } else if (game_mode == GAME_MODE_GAMEOVER) {
                // Restart game from gameover screen
                game_mode = GAME_MODE_PLAYING;
                clearTitleGameOverScreen();
                v4p_enable(ship); // Enable ship for gameplay
                v4p_disable(ship_flame); // Start with flame disabled
                resetGameState(); // Reset game state
            }
            space_press_timer = 0; // Reset timer
        }
    } else {
        space_press_timer = 0; // Reset timer if spacebar released
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
                // Get the size of the asteroid being removed
                int size = asteroid_size[j];
                V4pLayer layer = v4p_getLayer(asteroids[j]);
                
                // Only split if it's not already the smallest size
                if (size < 2 && asteroid_count + 2 <= MAX_ASTEROIDS) {
                    // Create 2 smaller asteroids
                    for (int k = 0; k < 2; k++) {
                        if (asteroid_count < MAX_ASTEROIDS) {
                            V4pPolygonP asteroid_proto = getAsteroidPrototypeSingleton();
                            asteroids[asteroid_count] = v4p_addClone(asteroid_proto);
                            v4p_setLayer(asteroids[asteroid_count], layer); // Keep same layer for split asteroids
                            v4p_setColor(asteroids[asteroid_count], v4p_getColor(asteroids[j]));
                            
                            // Calculate split angle: -90° for k=0, +90° for k=1 relative to original asteroid angle
                            float split_angle = asteroid_angle[j] + (k == 0 ? -90.0f : 90.0f);
                            
                            // Position new asteroid by moving 30 pixels in the split direction
                            int sina, cosa;
                            getSinCosFromDegrees(split_angle, &sina, &cosa);
                            asteroid_x[asteroid_count] = asteroid_x[j] + (sina / 256.0f) * (25 - size * 15);
                            asteroid_y[asteroid_count] = asteroid_y[j] - (cosa / 256.0f) * (25 - size * 15);
                            
                            // Set the asteroid's movement angle (keep some randomization for variety)
                            asteroid_angle[asteroid_count] = asteroid_angle[j] + (12 + (rand() % 52)) * (k - 1); // Randomize angle
                            
                            asteroid_size[asteroid_count] = size + 1; // Increase size (0->1, 1->2)
                            asteroid_speed[asteroid_count] = 0.3f + (rand() % 5) * 0.1f; // Random speed between 0.3 and 0.8
                            v4p_setCollisionMask(asteroids[asteroid_count], 2); // Asteroids are on layer 2
                            asteroid_count++;
                        }
                    }
                }
                
                v4p_destroyFromScene(asteroids[j]);
                // Shift remaining asteroids
                for (int k = j; k < asteroid_count - 1; k++) {
                    asteroids[k] = asteroids[k + 1];
                    asteroid_x[k] = asteroid_x[k + 1];
                    asteroid_y[k] = asteroid_y[k + 1];
                    asteroid_angle[k] = asteroid_angle[k + 1];
                    asteroid_size[k] = asteroid_size[k + 1];
                }
                asteroid_count--;
                break;
            }
        }
    }
    asteroids_to_remove_count = 0;

    if (game_mode == GAME_MODE_PLAYING) {
        // In title/gameover mode, disable ship controls and logic

        // Handle invulnerability timer and blinking effect
        if (invulnerability_timer > 0) {
            invulnerability_timer--;
            if (invulnerability_timer <= 0) {
                // Make sure ship is visible when invulnerability ends
                v4p_enable(ship);
                v4p_disable(ship_flame);
            } else {
                // Create blinking effect using totalTime modulo
                // Blink every 8 frames (4 frames visible, 4 frames invisible)
                if ((totalTime / 200) % 2 == 0) {
                    v4p_enable(ship);
                    v4p_disable(ship_flame);
                } else {
                    v4p_disable(ship);
                }
            }
        }

#ifdef DEBUG
        // Debug: Press SHIFT button to display ship information
        if (g4p_state.buttons[G4P_SHIFT]) {
            v4pi_debug("=== SHIP DEBUG INFO ===\n");
            v4pi_debug("Position: x=%.2f, y=%.2f, angle=%.2f\n", ship_x, ship_y, ship_angle);
            v4pi_debug("Speed: sx=%.2f, sy=%.2f\n", ship_speed_x, ship_speed_y);

            // Use the debug addon to dump polygon information
            v4p_debugPolygon(ship, "ship");
        }
#endif

        // Handle input
        if (g4p_state.buttons[G4P_LEFT]) {  // Left Arrow
            ship_angle -= 3.f;
        }
        if (g4p_state.buttons[G4P_RIGHT]) {  // Right Arrow
            ship_angle += 3.f;
        }

        thrusting = false;
        if (g4p_state.buttons[G4P_UP]) {  // Up Arrow
            thrusting = true;
            // Apply thrust in the direction the ship is facing
            int sina, cosa;
            getSinCosFromDegrees(ship_angle, &sina, &cosa);
            ship_speed_x += (sina / 256.0f) * 0.1f;  // Accelerate in thrust direction
            ship_speed_y -= (cosa / 256.0f) * 0.1f;
            
            // Randomly show/hide flame for flickering effect
            if (rand() % 3 == 0) {  // About 1/3 chance to show flame each frame
                v4p_enable(ship_flame);
            } else {
                v4p_disable(ship_flame);
            }
            
            // Emit thrust particles
            if (thrust_system) {
                particles_emit(thrust_system, ship_x - (sina / 256.0f) * 45, ship_y + (cosa / 256.0f) * 45, ship_angle + 180.0f);
            }
        } else {
            // Hide flame when not thrusting
            v4p_disable(ship_flame);
        }

        // Apply friction/deceleration when no thrust
        if (!g4p_state.buttons[G4P_UP]) {  // No Up Arrow
            ship_speed_x *= 0.98f;  // Slow down gradually
            ship_speed_y *= 0.98f;

            // Small threshold to stop completely
        if (fabs(ship_speed_x) < 0.01f) ship_speed_x = 0;
        if (fabs(ship_speed_y) < 0.01f) ship_speed_y = 0;
        }

        // Apply the speed vector to ship position
        ship_x += ship_speed_x;
        ship_y += ship_speed_y;

        // Space bar: fire bullet AND apply extra opposite thrust
        if (g4p_state.buttons[G4P_SPACE]) {  // Space button
            if (!last_space_pressed) {
                if (bullet_count < MAX_BULLETS) {
                    fireBullet();

                    // Apply thrust in the opposite direction of the bullet for recoil effect
                    int sina, cosa;
                    getSinCosFromDegrees(ship_angle, &sina, &cosa);
                    ship_speed_x -= (sina / 256.0f) * 0.2f;  // Stronger acceleration on space
                    ship_speed_y += (cosa / 256.0f) * 0.2f;
                }
            }
        }
        last_space_pressed = g4p_state.buttons[G4P_SPACE];

        // Wrap ship around screen edges
        if (ship_x < -v4p_displayWidth / 2) {
            ship_x = v4p_displayWidth / 2;
        } else if (ship_x > v4p_displayWidth / 2) {
            ship_x = -v4p_displayWidth / 2;
        }

        if (ship_y < -v4p_displayHeight / 2) {
            ship_y = v4p_displayHeight / 2;
        } else if (ship_y > v4p_displayHeight / 2) {
            ship_y = -v4p_displayHeight / 2;
        }

        // Update ship position and rotation (convert degrees to V4P format)
        v4p_transform(ship, ship_x, ship_y, ship_angle * 512.f / 360.f, 0, 256, 256);
    }

    // Update bullets - move them forward
    for (int i = 0; i < bullet_count; i++) {
        // Move bullet in its own direction
        int sina, cosa;
        getSinCosFromDegrees(bullet_angle[i], &sina, &cosa);

        // Move bullet forward
        bullet_x[i] += (sina / 256.0f) * 5.;
        bullet_y[i] -= (cosa / 256.0f) * 5.;

        // Update bullet position
        v4p_transform(bullets[i], bullet_x[i], bullet_y[i], bullet_angle[i] * 512.f / 360.f, 0, 256, 256);

        // Wrap bullets around screen edges (like asteroids)
        if (bullet_x[i] < -v4p_displayWidth / 2) {
            bullet_x[i] = v4p_displayWidth / 2;
        } else if (bullet_x[i] > v4p_displayWidth / 2) {
            bullet_x[i] = -v4p_displayWidth / 2;
        }

        if (bullet_y[i] < -v4p_displayHeight / 2) {
            bullet_y[i] = v4p_displayHeight / 2;
        } else if (bullet_y[i] > v4p_displayHeight / 2) {
            bullet_y[i] = -v4p_displayHeight / 2;
        }

        // Decrement bullet TTL and remove when expired
        bullet_ttl[i]--;
        if (bullet_ttl[i] <= 0) {
            v4p_destroyFromScene(bullets[i]);
            // Shift remaining bullets
            for (int j = i; j < bullet_count - 1; j++) {
                bullets[j] = bullets[j + 1];
                bullet_x[j] = bullet_x[j + 1];
                bullet_y[j] = bullet_y[j + 1];
                bullet_angle[j] = bullet_angle[j + 1];
                bullet_ttl[j] = bullet_ttl[j + 1];
            }
            bullet_count--;
            i--; // Adjust index after removal
        }
    }

    // Update asteroids
    for (int i = 0; i < asteroid_count; i++) {
        // Move asteroid forward with individual speed
        int sina, cosa;
        getSinCosFromDegrees(asteroid_angle[i], &sina, &cosa);
        asteroid_x[i] += (sina / 256.0f) * asteroid_speed[i];
        asteroid_y[i] -= (cosa / 256.0f) * asteroid_speed[i];

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

        // Scale down smaller asteroids
        int scale;
        if (asteroid_size[i] == 2) {  // small
            scale = 64;  // 25% size
        } else if (asteroid_size[i] == 1) { // medium
            scale = 128;  // 50% size
        } else {
            scale = 256; // 100% size for big asteroids
        }

        // Update asteroid transform rotating slowly
        v4p_transform(asteroids[i],
                        asteroid_x[i],
                        asteroid_y[i],
                        (asteroid_angle[i] + totalTime * 0.02f) * 512.f / 360.f,
                        0,
                        scale,
                        scale);
    }

    // Create new asteroid to keep game going
    if (asteroid_count < (level < MAX_ASTEROIDS ? level : MAX_ASTEROIDS) && rand() % 60 == 0) {
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

    // Check if game over and switch to gameover mode
    if (game_over && game_mode == GAME_MODE_PLAYING) {
        game_mode = GAME_MODE_GAMEOVER;
        v4p_disable(ship); // Disable ship in gameover mode
        createGameOverScreen();
        return success; // Don't exit, just show gameover screen
    }

    // Update particle systems
    if (explosion_system) {
        particles_iterate(explosion_system);
    }
    if (thrust_system) {
        particles_iterate(thrust_system);
    }

    return success;
}

Boolean g4p_onFrame() {
    // Render frame
    v4p_render();
    return success;
}

void g4p_onQuit() {
    if (explosion_system) {
        particles_destroy(explosion_system);
    }
    if (thrust_system) {
        particles_destroy(thrust_system);
    }
    if (shipProto) {
        v4p_destroy(shipProto);
    }
    if (rockProto[0]) {
        v4p_destroy(rockProto[0]);
    }
    if (rockProto[1]) {
        v4p_destroy(rockProto[1]);
    }
    if (bulletProto) {
        v4p_destroy(bulletProto);
    }
    
    v4p_clearScene();
    v4p_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}
