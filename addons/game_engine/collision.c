#include "collision.h"
#include "g4p.h"
#include "v4pi.h"
#include "quick/sortable.h"
#include <stdlib.h>
#include <stdint.h>  // for uintptr_t

// Global collision points system
static CollisionPointsSystem g4p_collision_points_system = {NULL, 0, NULL};

// Hash function to generate a key for polygon pair (p1, p2)
// Uses polygon IDs for more efficient and stable hashing
static int hash_polygon_pair(V4pPolygonP p1, V4pPolygonP p2) {
    // Use polygon IDs instead of pointers for more stable hashing
    uint32_t id1 = v4p_getId(p1);
    uint32_t id2 = v4p_getId(p2);
    
    // Ensure consistent ordering to avoid duplication
    if (id1 > id2) {
        uint32_t temp = id1;
        id1 = id2;
        id2 = temp;
    }
    
    // Better hash function using both IDs with multiplication
    // This provides better distribution than simple XOR
    uint32_t hash = id1 * 31 + id2;  // 31 is a good prime for hashing
    hash = (hash * 31) + (id1 ^ (id2 << 2));
    
    return (int)(hash % g4p_collision_points_system.table_size);
}

// Initialize collision points system
void g4p_initCollisions(size_t table_size) {
    if (g4p_collision_points_system.table != NULL) {
        v4pi_debug("Collision points system already initialized\n");
        return;
    }
    
    g4p_collision_points_system.table_size = table_size;
    g4p_collision_points_system.table = QuickTableNew(table_size);
    
    if (!g4p_collision_points_system.table) {
        v4p_error("Failed to create collision points QuickTable\n");
        return;
    }
    
    v4pi_debug("Collision points system initialized with table size %zu\n", table_size);
}

// Reset collision points data
void g4p_resetCollisions() {
    if (!g4p_collision_points_system.table) {
        return;
    }
    
    // Clear all entries in the QuickTable and free associated memory
    QuickTableResetAndFree(g4p_collision_points_system.table);
    v4pi_debug("Collision points system reset\n");
}

// Finalize collision points - compute averages and call callback
void g4p_finalizeCollisions() {
    if (!g4p_collision_points_system.table) {
        return;
    }
    
    v4pi_debug("Finalizing collision points...\n");
    
    // Iterate through all buckets in the QuickTable
    for (size_t i = 0; i < g4p_collision_points_system.table_size; i++) {
        List current = QuickTableGet(g4p_collision_points_system.table, i);
        
        while (current != NULL) {
            CollisionPointData* data = (CollisionPointData*)ListData(current);
            
            if (data && data->count > 0) {
                // Compute average coordinates
                data->avg_x = data->x_sum / data->count;
                data->avg_y = data->y_sum / data->count;
                
                v4pi_debug("Polygon pair (%p, %p): avg_x=%d, avg_y=%d, count=%d\n",
                          (void*)data->p1, (void*)data->p2, data->avg_x, data->avg_y, data->count);
                
                // Call callback if set
                if (g4p_collision_points_system.callback != NULL) {
                    g4p_collision_points_system.callback(data->p1, data->p2, data->avg_x, data->avg_y, data->count);
                }
            }
            
            // Check if there are more entries in the QuickTable's linked list
            if (current->quick != NULL) {
                current = current->quick;
            } else {
                break;
            }
        }
    }
}

// Destroy collision points system
void g4p_destroyCollisions() {
    if (g4p_collision_points_system.table) {
        QuickTableDelete(g4p_collision_points_system.table);
        g4p_collision_points_system.table = NULL;
        g4p_collision_points_system.table_size = 0;
        g4p_collision_points_system.callback = NULL;
        v4pi_debug("Collision points system destroyed\n");
    }
}

// Set callback function for collision point finalization
void g4p_setCollisionCallback(G4pCollisionCallback callback) {
    g4p_collision_points_system.callback = callback;
    v4pi_debug("Collision point callback set to %p\n", (void*)callback);
}

// Get average collision point for a polygon pair
Boolean g4p_getCollisionPoint(V4pPolygonP p1, V4pPolygonP p2, V4pCoord* avg_x, V4pCoord* avg_y) {
    if (!g4p_collision_points_system.table || !p1 || !p2) {
        return false;
    }
    
    // Get polygon IDs for efficient comparison
    uint32_t id1 = v4p_getId(p1);
    uint32_t id2 = v4p_getId(p2);
    
    int key = hash_polygon_pair(p1, p2);
    List current = QuickTableGet(g4p_collision_points_system.table, key);
    
    while (current != NULL) {
        CollisionPointData* data = (CollisionPointData*)ListData(current);
        
        if (data) {
            uint32_t data_id1 = v4p_getId(data->p1);
            uint32_t data_id2 = v4p_getId(data->p2);
            
            // Direct ID comparison is faster than pointer comparison
            if ((data_id1 == id1 && data_id2 == id2) || (data_id1 == id2 && data_id2 == id1)) {
                *avg_x = data->avg_x;
                *avg_y = data->avg_y;
                return true;
            }
        }
        
        // Check if there are more entries in the QuickTable's linked list
        if (current->quick != NULL) {
            current = current->quick;
        } else {
            break;
        }
    }
    
    return false;
}

// Add collision point data for a polygon pair
void g4p_addCollisionPoint(V4pPolygonP p1, V4pPolygonP p2, V4pCoord x, V4pCoord y) {
    if (!g4p_collision_points_system.table || !p1 || !p2) {
        return;
    }
    
    // Get polygon IDs for efficient comparison
    uint32_t id1 = v4p_getId(p1);
    uint32_t id2 = v4p_getId(p2);
    
    int key = hash_polygon_pair(p1, p2);
    List current = QuickTableGet(g4p_collision_points_system.table, key);
    
    // Check if this polygon pair already has collision data
    while (current != NULL) {
        CollisionPointData* data = (CollisionPointData*)ListData(current);
        
        if (data) {
            uint32_t data_id1 = v4p_getId(data->p1);
            uint32_t data_id2 = v4p_getId(data->p2);
            if ((data_id1 == id1 && data_id2 == id2) || (data_id1 == id2 && data_id2 == id1)) {
                // Found existing data, update it
                data->x_sum += x;
                data->y_sum += y;
                data->count++;
                return;
            }
        }
        
        current = ListNext(current);
    }
    
    // No existing data found, create new entry
    List new_list = ListNew();
    if (!new_list) {
        return;
    }
    
    CollisionPointData* new_data = (CollisionPointData*)malloc(sizeof(CollisionPointData));
    if (!new_data) {
        ListFree(new_list);
        return;
    }
    
    // Initialize new collision data
    new_data->p1 = p1;
    new_data->p2 = p2;
    new_data->x_sum = x;
    new_data->y_sum = y;
    new_data->count = 1;
    new_data->avg_x = x;
    new_data->avg_y = y;
    
    ListSetData(new_list, new_data);
    QuickTableAdd(g4p_collision_points_system.table, key, new_list);
}