/* ======================================================================= */
/* "QuickHeap" based many-in-one-cell table                                */
/* ======================================================================= */
#include "quickheap.h"
#include "sortable.h"
typedef struct sQuickTable {
   size_t sizeOfTable ;
   List* table;
   QuickHeapS listHeap;
} QuickTableS, *QuickTable ;

QuickTable QuickTableNew(size_t sizeOfItem, size_t sizeOfTable);
#define    QuickTableNewFor(T) QuickTableNew(sizeof(T))
void       QuickTableDelete(QuickTable t);
void       QuickTableReset(QuickTable q);

void*      QuickTableSet(QuickTable q, int index, void* p);
List       QuickTableGet(QuickTable q, int index);

