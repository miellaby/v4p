/**
 * "QuickHeap" based many-in-one-cell table
 */
#include <stdlib.h>
#include <string.h>
#include "sortable.h"
#include "quickheap.h"
#include "quicktable.h"

QuickTable QuickTableNew(size_t sizeOfTable) {
    QuickTable q = (QuickTable) malloc(sizeof(struct sQuickTable));
    if (! q) {
        return NULL;
    }
    q->sizeOfTable = sizeOfTable;
    q->table = (List*) malloc(sizeof(List) * sizeOfTable);
    if (! q->table) {
        free(q);
        return NULL;
    }
    memset(q->table, 0, sizeof(List) * q->sizeOfTable);
    return q;
}

void QuickTableDestroy(QuickTable q) {
    free(q->table);
    free(q);
}

void QuickTableReset(QuickTable q) {
    memset(q->table, 0, sizeof(List) * q->sizeOfTable);
}

// Free all List elements in the table (but not their data) and clear the table
void QuickTableResetAndFree(QuickTable q) {
    for (size_t i = 0; i < q->sizeOfTable; i++) {
        List current = q->table[i];
        while (current != NULL) {
            List next = current->quick;  // Get next before freeing current
            // Free the list element itself
            ListFree(current);
            
            current = next;
        }
        q->table[i] = NULL;  // Clear the table entry
    }
}

List QuickTableAdd(QuickTable q, int index, List l) {
    l->quick = q->table[index];
    q->table[index] = l;
    return l;
}

void QuickTableRemove(QuickTable q, int index, List toBeRemoved) {
    List previous = NULL, l = q->table[index];
    // search loop
    while (l && l != toBeRemoved) {
        previous = l;
        l = l->quick;
    }

    if (! l)  // not found
        return;

    List n = l->quick;
    if (previous)
        previous->quick = n;
    else
        q->table[index] = n;
}

List QuickTableGet(QuickTable q, int index) {
    return q->table[index];
}
