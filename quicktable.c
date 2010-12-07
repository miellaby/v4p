/* ========================================================================== */
/* "QuickHeap" based many-in-one-cell table                                   */
/* ========================================================================== */
#include <stdlib.h>
#include <string.h>
#include "sortable.h"
#include "quickheap.h"
#include "quicktable.h"

QuickTable QuickTableNew(size_t sizeOfTable) {
  QuickTable q = (QuickTable)malloc(sizeof(struct sQuickTable));
  if (!q) return NULL;
  q->sizeOfTable = sizeOfTable;
  q->table = (List*)malloc(sizeof(List) * sizeOfTable);
  if (!q->table) { free(q) ; return NULL; }
  q->listHeap = QuickHeapNewFor(struct sList);
  if (!q->listHeap) { free (q->table) ; free(q) ; return NULL; }
  QuickHeapReserve(q->listHeap, 2048);
  QuickTableReset(q);
  return q;
}

void QuickTableDelete(QuickTable q) {
  QuickHeapDelete(q->listHeap); 
  free(q->table) ;
  free(q);
}

void QuickTableReset(QuickTable q) {
  memset(q->table, 0, sizeof(List) * q->sizeOfTable);
  QuickHeapReset(q->listHeap); 
}

void* QuickTableAdd(QuickTable q, int index, void* p) {
   List _n = (List)QuickHeapAlloc(q->listHeap);
   ListSetData(_n, p);
   ListAdd(q->table[index], _n);
   return p;
}

void QuickTableRemove(QuickTable q, int index, void* p) {
   List previous = NULL, l = q->table[index];
   // search loop
   while (l && ListData(l) != p)  {
     previous = l;
     l = ListNext(l);
   }

   if (!l) // not found
     return;

   List n = l->next ;
   QuickHeapFree(q->listHeap, l);
   if (previous)
     ListSetNext(previous, n);
   else
     q->table[index] = n;
}

List QuickTableGet(QuickTable q, int index) {
  return q->table[index];
}

