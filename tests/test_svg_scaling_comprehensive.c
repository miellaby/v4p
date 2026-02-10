#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "v4p.h"
#include "v4pserial.h"

int main() {
    v4p_init();
    
    printf("=== Comprehensive SVG Scaling Test ===\n\n");
    
    // Test 1: Simple square with various float scales
    printf("Test 1: Simple Square (0,0 to 100,100)\n");
    printf("SVG Path: M 0,0 L 100,0 L 100,100 L 0,100 Z\n\n");
    
    float test_scales[] = {2.0f, 1.0f, 0.5f, 0.25f, 0.125f, 0.1f, 0.05f};
    int i;
    
    printf("Scale  | Expected Size | Actual Size | Error%%\n");
    printf("-------|---------------|-------------|--------\n");
    
    for (i = 0; i < sizeof(test_scales)/sizeof(test_scales[0]); i++) {
        float scale = test_scales[i];
        V4pPolygonP p = v4p_new(V4P_ABSOLUTE, V4P_WHITE, 0);
        
        v4p_decodeSVGPath(p, "M 0,0 L 100,0 L 100,100 L 0,100 Z", scale);
        
        V4pPointP points = v4p_getPoints(p);
        V4pCoord min_x = INT32_MAX, max_x = INT32_MIN;
        V4pCoord min_y = INT32_MAX, max_y = INT32_MIN;
        
        V4pPointP current = points;
        while (current) {
            if (current->x < min_x) min_x = current->x;
            if (current->x > max_x) max_x = current->x;
            if (current->y < min_y) min_y = current->y;
            if (current->y > max_y) max_y = current->y;
            current = current->next;
        }
        
        V4pCoord actual_width = max_x - min_x;
        V4pCoord actual_height = max_y - min_y;
        float expected_width = 100.0f * scale;
        float expected_height = 100.0f * scale;
        
        float width_error = fabs(actual_width - expected_width) / expected_width * 100.0f;
        float height_error = fabs(actual_height - expected_height) / expected_height * 100.0f;
        
        printf("%.3f   | %4.1fx%4.1f     | %dx%d       | %.1f%%\n", 
               scale, expected_width, expected_height, 
               actual_width, actual_height, 
               (width_error + height_error) / 2.0f);
        
        v4p_destroy(p);
    }
    
    printf("\n");
    
    // Test 2: Star shape with problematic scales
    printf("Test 2: Complex Star Shape\n");
    printf("SVG Path: Star from demo (original size ~105x100)\n\n");
    
    const char* star_path = "M 478.1,5  L 490.5,43.2 L 530.7,43.2 L 498.2,66.8 \
          L 510.6,105 L 478.1,81.4 L 445.6,105 L 458,66.8 \
          L 425.5,43.2 L 465.7,43.2 L 478.1,5 z";
    
    float star_scales[] = {1.0f, 0.5f, 0.25f, 0.125f, 0.0625f};
    
    printf("Scale   | Expected Size | Actual Size | Error%%\n");
    printf("--------|---------------|-------------|--------\n");
    
    for (i = 0; i < sizeof(star_scales)/sizeof(star_scales[0]); i++) {
        float scale = star_scales[i];
        V4pPolygonP p = v4p_new(V4P_ABSOLUTE, V4P_WHITE, 0);
        
        v4p_decodeSVGPath(p, star_path, scale);
        
        V4pPointP points = v4p_getPoints(p);
        V4pCoord min_x = INT32_MAX, max_x = INT32_MIN;
        V4pCoord min_y = INT32_MAX, max_y = INT32_MIN;
        
        V4pPointP current = points;
        while (current) {
            if (current->x < min_x) min_x = current->x;
            if (current->x > max_x) max_x = current->x;
            if (current->y < min_y) min_y = current->y;
            if (current->y > max_y) max_y = current->y;
            current = current->next;
        }
        
        V4pCoord actual_width = max_x - min_x;
        V4pCoord actual_height = max_y - min_y;
        float expected_width = 105.0f * scale;  // Original star width
        float expected_height = 100.0f * scale; // Original star height
        
        float width_error = fabs(actual_width - expected_width) / expected_width * 100.0f;
        float height_error = fabs(actual_height - expected_height) / expected_height * 100.0f;
        
        printf("%.4f  | %4.1fx%4.1f     | %dx%d       | %.1f%%\n", 
               scale, expected_width, expected_height, 
               actual_width, actual_height, 
               (width_error + height_error) / 2.0f);
        
        v4p_destroy(p);
    }
    
    printf("\n");
    
    // Test 3: Fractional scales that were problematic before
    printf("Test 3: Fractional Scales (Previously Problematic)\n");
    printf("Testing scales that caused deformation in the old implementation\n\n");
    
    float fractional_scales[] = {0.333f, 0.666f, 0.75f, 0.8f, 0.9f};
    
    printf("Scale  | Expected | Actual | Error%%\n");
    printf("-------|----------|--------|--------\n");
    
    for (i = 0; i < sizeof(fractional_scales)/sizeof(fractional_scales[0]); i++) {
        float scale = fractional_scales[i];
        V4pPolygonP p = v4p_new(V4P_ABSOLUTE, V4P_WHITE, 0);
        
        v4p_decodeSVGPath(p, "M 0,0 L 100,0 L 100,100 L 0,100 Z", scale);
        
        V4pPointP points = v4p_getPoints(p);
        V4pCoord min_x = INT32_MAX, max_x = INT32_MIN;
        V4pCoord min_y = INT32_MAX, max_y = INT32_MIN;
        
        V4pPointP current = points;
        while (current) {
            if (current->x < min_x) min_x = current->x;
            if (current->x > max_x) max_x = current->x;
            if (current->y < min_y) min_y = current->y;
            if (current->y > max_y) max_y = current->y;
            current = current->next;
        }
        
        V4pCoord actual_size = max_x - min_x; // Should be square
        float expected_size = 100.0f * scale;
        float size_error = fabs(actual_size - expected_size) / expected_size * 100.0f;
        
        printf("%.3f   | %7.1f  | %6d  | %.1f%%\n", 
               scale, expected_size, actual_size, size_error);
        
        v4p_destroy(p);
    }
    
    printf("\n=== Test Complete ===\n");
    v4p_quit();
    return 0;
}