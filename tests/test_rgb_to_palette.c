#include "../v4p_color.h"
#include "../v4p_ll.h"
#include <stdio.h>

int main() {
    // Test the RGB to palette index function
    printf("Testing v4p_rgb_to_palette_index function:\n\n");
    
    // Test some known values from the palette
    printf("RGB(255,255,255) -> index %d (should be 0 - white)\n", 
           v4p_rgb_to_palette_index(255, 255, 255));
    
    printf("RGB(255,0,0) -> index %d (should be 30 - red)\n", 
           v4p_rgb_to_palette_index(255, 0, 0));
    
    printf("RGB(0,255,0) -> index %d (should be 180 - green)\n", 
           v4p_rgb_to_palette_index(0, 255, 0));
    
    printf("RGB(0,0,255) -> index %d (should be 30 - blue)\n", 
           v4p_rgb_to_palette_index(0, 0, 255));
    
    printf("RGB(255,255,0) -> index %d (should be 35 - yellow)\n", 
           v4p_rgb_to_palette_index(255, 255, 0));
    
    printf("RGB(0,255,255) -> index %d (should be 210 - cyan)\n", 
           v4p_rgb_to_palette_index(0, 255, 255));
    
    printf("RGB(255,0,255) -> index %d (should be 5 - magenta)\n", 
           v4p_rgb_to_palette_index(255, 0, 255));
    
    // Test some intermediate values
    printf("RGB(128,128,128) -> index %d (should be 215 - gray)\n", 
           v4p_rgb_to_palette_index(128, 128, 128));
    
    printf("RGB(100,150,200) -> index %d\n", 
           v4p_rgb_to_palette_index(100, 150, 200));
    
    // Verify the formula works correctly
    printf("\nVerifying formula for a few values:\n");
    
    // Test RGB(255,0,0): should be r_index=5, g_index=0, b_index=0
    // index = 0 + 5*6 + 0*36 = 30
    V4pColor index = v4p_rgb_to_palette_index(255, 0, 0);
    printf("RGB(255,0,0): index=%d, palette RGB=(%d,%d,%d)\n", 
           index, v4p_palette[index][0], v4p_palette[index][1], v4p_palette[index][2]);
    
    // Test RGB(0,255,0): should be r_index=0, g_index=5, b_index=0
    // index = 0 + 0*6 + 5*36 = 180
    index = v4p_rgb_to_palette_index(0, 255, 0);
    printf("RGB(0,255,0): index=%d, palette RGB=(%d,%d,%d)\n", 
           index, v4p_palette[index][0], v4p_palette[index][1], v4p_palette[index][2]);
    
    // Test RGB(0,0,255): should be r_index=0, g_index=0, b_index=5
    // index = 5 + 0*6 + 0*36 = 5
    index = v4p_rgb_to_palette_index(0, 0, 255);
    printf("RGB(0,0,255): index=%d, palette RGB=(%d,%d,%d)\n", 
           index, v4p_palette[index][0], v4p_palette[index][1], v4p_palette[index][2]);
    
    return 0;
}