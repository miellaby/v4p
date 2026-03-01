#include "quick/math.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Testing floorLog2 with 32-bit values...\n");
    
    // Test some key values
    UInt32 test_values[] = {1, 2, 3, 4, 15, 16, 17, 31, 32, 63, 64, 127, 128, 255, 256, 511, 512, 1023, 1024, 2047, 2048, 4095, 4096, 8191, 8192, 16383, 16384, 32767, 32768, 65535, 65536, 131071, 131072, 262143, 262144, 524287, 524288, 1048575, 1048576, 2097151, 2097152, 4194303, 4194304, 8388607, 8388608, 16777215, 16777216, 33554431, 33554432, 67108863, 67108864, 134217727, 134217728, 268435455, 268435456, 536870911, 536870912, 1073741823, 1073741824, 2147483647, 2147483648};
    
    for (int i = 0; i < (int) (sizeof(test_values)/sizeof(test_values[0])); i++) {
        UInt32 val = test_values[i];
        int result = floorLog2(val);
        printf("floorLog2(%u) = %d\n", val, result);
        
        // Verify the result is correct (avoid overflow for max values)
        if (result >= 0) {
            UInt32 expected_min = 1 << result;
            UInt32 expected_max = (result < 31) ? (UInt32) ((1 << (result + 1)) - 1) : 0xFFFFFFFF;
            
            if (val < expected_min || (result < 31 && val > expected_max)) {
                printf("ERROR: floorLog2(%u) = %d is incorrect! Expected range [%u, %u]\n", 
                       val, result, expected_min, expected_max);
                return 1;
            }
        }
    }
    
    printf("All tests passed!\n");
    return 0;
}