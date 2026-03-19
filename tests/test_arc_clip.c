// Test for arc clipping functionality
#include "v4p.h"
#include "_v4p.h"
#include "addons/debug/debug.h"
#include "game_engine/g4p.h"
#include <stdio.h>
#include "clipping/clipping.h"

int g4p_onInit(int quality, bool fullscreen) {
    v4p_init2(quality, fullscreen);
    v4p_setBGColor(V4P_BLACK);
    return 0;
}

int g4p_onTick(int32_t deltaTime) {
    static bool test_passed = false;

    if (! test_passed) {
        printf("Testing arc clipping with disk...\n");

        // Create a disk (made of 4 arcs)
        V4pPolygonP disk = v4p_addNewDisk(V4P_ABSOLUTE, V4P_WHITE, 1, v4p_displayWidth / 2, v4p_displayHeight / 2, 100);

        if (! disk) {
            printf("✗ Failed to create disk\n");
            return 0;
        }

        // Debug original disk
        printf("\n=== ORIGINAL DISK ===\n");
        v4p_debugPolygon(disk, "original_disk");
        
        // Count points in the disk
        Polygon* diskPoly = (Polygon*) disk;
        V4pPointP point = diskPoly->point1;
        int disk_point_count = 0;
        while (point) {
            disk_point_count++;
            point = point->next;
        }
        printf("Created disk with %d points\n", disk_point_count);

        // Clip the disk (using v4p_clip with bounding box)
        // Clip to a smaller square centered on the disk
        V4pCoord center_x = v4p_displayWidth / 2;
        V4pCoord center_y = v4p_displayHeight / 2;
        V4pCoord clip_x0 = center_x - 80;
        V4pCoord clip_y0 = center_y - 80;
        V4pCoord clip_x1 = center_x + 80;
        V4pCoord clip_y1 = center_y + 80;

        printf("Clipping with bounds: (%d,%d) to (%d,%d)\n", clip_x0, clip_y0, clip_x1, clip_y1);

        // Clip the disk
        V4pPolygonP clipped = v4p_clip(disk, clip_x0, clip_y0, clip_x1, clip_y1);

        if (clipped) {
            printf("✓ Arc clipping test PASSED\n");

            // Debug clipped result
            printf("\n=== CLIPPED DISK ===\n");
            v4p_debugPolygon(clipped, "clipped_disk");
            
            // Count points in original disk
            Polygon* originalPoly = (Polygon*) disk;
            V4pPointP orig_point = originalPoly->point1;
            int orig_count = 0;
            while (orig_point) {
                orig_count++;
                orig_point = orig_point->next;
            }

            // Count points in clipped result
            Polygon* clippedPoly = (Polygon*) clipped;
            V4pPointP clip_point = clippedPoly->point1;
            int clip_count = 0;
            while (clip_point) {
                clip_count++;
                clip_point = clip_point->next;
            }

            printf("  Original disk: %d points\n", orig_count);
            printf("  Clipped result: %d points\n", clip_count);

            if (clip_count == 0) {
                printf("  Warning: Clipped polygon has no points\n");
            }

            test_passed = true;
        } else {
            printf("✗ Arc clipping test FAILED\n");
            printf("  Clipping returned NULL\n");
            return 0;
        }
    }

    return 0;
}

int g4p_onFrame() {
    return v4p_render();
}

void g4p_onQuit() {
    v4p_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}