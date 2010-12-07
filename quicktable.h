#ifndef QUICKTABLE_H
#define QUICKTABLE_H
/* ======================================================================= */
/* "QuickHeap" based many-in-one-cell table                                */
/* ======================================================================= */
#include "quickheap.h"
#include "sortable.h"
typedef struct sQuickTable {
   size_t sizeOfTable ;
   List* table;
   QuickHeap listHeap;
} QuickTableS, *QuickTable;

QuickTable QuickTableNew(size_t sizeOfTable);
void       QuickTableDelete(QuickTable t);
void       QuickTableReset(QuickTable q);

void*      QuickTableAdd(QuickTable q, int index, void* p);
void       QuickTableRemove(QuickTable q, int index, void* p);
List       QuickTableGet(QuickTable q, int index);
#endif

