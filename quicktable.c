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
  QuickTableReset(q);
  return q;
}

void QuickTableDelete(QuickTable q) {
  free(q->table) ;
  free(q);
}

void QuickTableReset(QuickTable q) {
  memset(q->table, 0, sizeof(List) * q->sizeOfTable);
}

List QuickTableAdd(QuickTable q, int index, List l) {
   l->quick = q->table[index];
   q->table[index] = l;
   return l;
}

void QuickTableRemove(QuickTable q, int index, List toBeRemoved) {
   List previous = NULL, l = q->table[index];
   // search loop
   while (l && l != toBeRemoved)  {
     previous = l;
     l = l->quick;
   }

   if (!l) // not found
     return;

   List n = l->quick ;
   if (previous)
     previous->quick = n;
   else
     q->table[index] = n;
}

List QuickTableGet(QuickTable q, int index) {
  return q->table[index];
}

