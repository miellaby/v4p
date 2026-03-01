#ifndef QUICKTABLE_H
#define QUICKTABLE_H
/**
 * A simple hash Table to store "shortcuts" into preexisting sorted lists.
 * See table.c for implementation details and memory model.
 */
#include "quick/heap.h"
#include "quick/sortable.h"
typedef struct sQuickTable {
    size_t sizeOfTable;
    List* table;
} QuickTableS, *QuickTable;

QuickTable QuickTableNew(size_t sizeOfTable);
void QuickTableDestroy(QuickTable t);
void QuickTableReset(QuickTable q);
void QuickTableResetAndFree(QuickTable q);

List QuickTableAdd(QuickTable q, int index, List l);
void QuickTableRemove(QuickTable q, int index, List l);
List QuickTableGet(QuickTable q, int index);
#endif
