/* ========================================================================== */
/* "QuickHeap" based many-in-one-cell table                                   */
/* ========================================================================== */
#include <stdlib.h>
#include "sortable.h"
#include "quickheap.h"
#include "quicktable.h"

QuickTable QuickTableNew(size_t sizeOfItem, size_t sizeOfTable) {
  List* table;
  QuickTable t = (QuickTable)malloc(sizeOf(QuickTableS));
  if (!t) return NULL;
  t->sizeOfTable = sizeOfTable;
  t->table = malloc(sizeOf(List) * sizeOfTable);
  if (!t->table) { free(t) ; return NULL; }
  t->listHeap = QuickHeapNewFor(List);
  if (!t->listHeap) { free (t->table) ; free(t) ; return NULL; }
  QuickTableReset(t);
  return t;
}

void QuickTableDelete(QuickTable t) {
  QuickHeapDelete(t->listHeap); 
  free(t->table) ;
  free(t);
}

void QuickTableReset(QuickTable t) {
  memset(t->table, 0, sizeOf(List) * sizeOfTable);
  QuickHeapReset(t->listHeap); 
}

void* QuickTableSet(QuickTable q, int index, void* p) {
   List _n = (List)QuickHeapAlloc(q->listHeap);
   ListSetData(_n, p);
   ListAdd(q->table[index], _n);
   return p;
}

List QuickTableGet(QuickTable q, int index) {
   return q->table[index];
}

#endif

