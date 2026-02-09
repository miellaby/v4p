#include "collision.h"
#include "g4p.h"
#include "v4pi.h"
#include "quick/sortable.h"
#include <stdlib.h>
#include <stdint.h>  // for uintptr_t
#include <assert.h>

#define CRASH do { *(volatile int*) 0 = 0; } while(0)  // Force immediate crash

// Global collision points system
static CollisionPointsSystem g4p_collision_points_system = {NULL, 0, NULL, NULL};

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
    
    uint32_t hash = id1 * 31 + id2;
    return (int)(hash % g4p_collision_points_system.table_size);
}

// Initialize collision points system
void g4p_initCollisions(size_t table_size) {
    if (g4p_collision_points_system.table != NULL) {
        v4p_trace(G4P, "Collision points system already initialized\n");
        return;
    }
    
    g4p_collision_points_system.table_size = table_size;
    g4p_collision_points_system.table = QuickTableNew(table_size);
    
    if (!g4p_collision_points_system.table) {
        v4p_error("Failed to create collision points QuickTable\n");
        CRASH;
    }

    // Initialize QuickHeap for collision data storage
    g4p_collision_points_system.data_heap = QuickHeapNew(sizeof(CollisionPointData));
    if (!g4p_collision_points_system.data_heap) {
        v4p_error("Failed to create collision data QuickHeap\n");
        CRASH;
    }

    v4p_trace(G4P, "Collision points system initialized with table size %zu\n", table_size);
}

// Reset collision points data
void g4p_resetCollisions() {
    // Clear all entries in the QuickTable and free associated memory
    // Using QuickHeap for data storage and QuickHeapReset for efficient clearing
    QuickTableResetAndFree(g4p_collision_points_system.table);
    
    // Reset the QuickHeap to clear all collision data efficiently
    if (g4p_collision_points_system.data_heap) {
        QuickHeapReset(g4p_collision_points_system.data_heap);
    }
    
    v4p_trace(G4P, "Collision points system reset\n");
}

// Finalize collision points - compute averages and call callback
void g4p_finalizeCollisions() {
    v4p_trace(G4P, "Finalizing collision points...\n");
    
    // Iterate through all buckets in the QuickTable
    for (size_t i = 0; i < g4p_collision_points_system.table_size; i++) {
        List current = QuickTableGet(g4p_collision_points_system.table, i);
        
        while (current != NULL) {
            CollisionPointData* data = (CollisionPointData*)ListData(current);
            
            if (data && data->count > 0) {
                // Compute average coordinates
                data->avg_x = data->x_sum / data->count;
                data->avg_y = data->y_sum / data->count;
                
                v4p_trace(G4P, "Polygon pair (%d, %d): avg_x=%d, avg_y=%d, count=%d\n",
                          v4p_getId(data->p1), v4p_getId(data->p2), data->avg_x, data->avg_y, data->count);
                
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
        g4p_resetCollisions();
        QuickTableDelete(g4p_collision_points_system.table);
        g4p_collision_points_system.table = NULL;
        g4p_collision_points_system.table_size = 0;
        g4p_collision_points_system.callback = NULL;
        v4p_trace(G4P, "Collision points system destroyed\n");
    }
    
    // Clean up the QuickHeap
    if (g4p_collision_points_system.data_heap) {
        QuickHeapDestroy(g4p_collision_points_system.data_heap);
        g4p_collision_points_system.data_heap = NULL;
    }
}

// Set callback function for collision point finalization
void g4p_setCollisionCallback(G4pCollisionCallback callback) {
    g4p_collision_points_system.callback = callback;
    v4p_trace(G4P, "Collision point callback set to %p\n", (void*)callback);
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
    // Check pointers BEFORE calling v4p_getId to avoid crash in v4p_getId
    if (!p1 || !p2) {
        v4p_trace(G4P, "CRASH: Null polygon pointer in g4p_addCollisionPoint: p1=%p, p2=%p at x=%d, y=%d\n", 
                  (void*)p1, (void*)p2, x, y);
        v4p_trace(G4P, "This indicates a dangling pointer in concretePolygons array\n");
        CRASH;
    }

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
        CRASH;
    }
    
    // Allocate collision data from QuickHeap instead of malloc
    CollisionPointData* new_data = (CollisionPointData*)QuickHeapAlloc(g4p_collision_points_system.data_heap);
    if (!new_data) {
        CRASH;
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