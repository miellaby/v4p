/**
 * Test for sortable list functionality
 * This test verifies the ListSort function from sortable.c
 */

#include "quick/sortable.h"
#include "quick/heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Comparison function for integers (descending order)
int intDataPrior(void* a, void* b) {
    return (*((int *)a)) >= (*((int *)b));
}

int main() {
    List demo = NULL, orderedList;

    // Seed random number generator
    srand(time(NULL));

    ListSetCompareFunc(intDataPrior);

    // create a random list of integers
    for (int i = 0; i < 1000; i++) {
        List n = ListNew();

        ListSetNext(n, demo);

        {
            int* ip = malloc(sizeof(int));
            *ip = rand() % 10000;  // Random value between 0-9999
            ListSetData(n, ip);
        }

        demo = n;
    }

    printf("Sorting list of 1000 random integers...\n");
    orderedList = ListSort(demo);

    // Verify the list is sorted in descending order
    int errors = 0;
    List p = orderedList;
    if (p) {
        int prev = *((int *)ListData(p));
        p = ListNext(p);
        
        while (p) {
            int current = *((int *)ListData(p));
            if (current > prev) {  // Should be descending order
                errors++;
                if (errors <= 5) {  // Show first few errors
                    printf("ERROR: Sorting violation at position: %d > %d\n", current, prev);
                }
            }
            prev = current;
            p = ListNext(p);
        }
    }

    if (errors == 0) {
        printf("SUCCESS: List is properly sorted in descending order!\n");
        
        // Print first 10 elements to show it worked
        printf("First 10 elements: ");
        p = orderedList;
        for (int i = 0; i < 10 && p; i++) {
            printf("%d ", *((int *)ListData(p)));
            p = ListNext(p);
        }
        printf("...\n");
    } else {
        printf("FAILED: Found %d sorting violations\n", errors);
    }

    // Clean up
    while (orderedList) {
        free(ListData(orderedList));
        orderedList = ListFree(orderedList);
    }

    return errors > 0 ? 1 : 0;
}