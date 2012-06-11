/*======================================================================= */
// Quick Heaps
/*======================================================================= */
#include "quickheap.h"
#include <stdio.h>
#include <stdlib.h>

QuickHeap QuickHeapNew(int sizeOfItem) {
  QuickHeap p = (QuickHeap)malloc(sizeof(QuickHeapS)) ;
  QuickHeapS newHeap = QuickHeapInitializerFor(void *) ;
  newHeap.sizeOfItem = (sizeOfItem < sizeof(void *) ? sizeof(void *) : sizeOfItem) ;
  *p = newHeap ;
  return p ;
}

QuickHeap QuickHeapReservedNew(int sizeOfItem, int maxSize) {
  QuickHeap p = (QuickHeap)malloc(sizeof(QuickHeapS)) ;
  QuickHeapS newHeap = QuickHeapInitializerFor(void *) ;
  newHeap.sizeOfItem = (sizeOfItem < sizeof(void *) ? sizeof(void *) : sizeOfItem) ;
  newHeap.maxSize = maxSize ;
  if (newHeap.maxSize)
     newHeap.heap = (char *)malloc(newHeap.sizeOfItem * newHeap.maxSize) ;
  *p = newHeap ;
  return p ;
}

// Call this function to reserve more memory than the default 128 bytes chunck
// It will improve performance of QuickHeapFree and indice based functions
void QuickHeapReserve(QuickHeap q, int reservedSize) {
  if (q->maxSize) return;
  q->maxSize = reservedSize ;
  q->heap = (char *)malloc(q->sizeOfItem * reservedSize) ;
}

void QuickHeapReset(QuickHeap q) {
  QuickHeap p = q->next; // we keep first heap block
  while (p) {
    QuickHeap next = p->next ;
    if (p->heap) free(p->heap) ;
    free(p) ;
    p = next ;
  }
  // if (q->heap) free(q->heap) ;
  // q->heap = NULL;
  q->hole = 0;
  q->size = 0;
  // q->maxSize = 0;
  q->baseIndice = 0;
  q->next = NULL;
}

void QuickHeapDelete(QuickHeap q) {
  QuickHeapReset(q);
  free(q);
}

void *QuickHeapAlloc(QuickHeap q) {
   if (q->maxSize == 0) {
      q->maxSize = 1024 ;
      q->heap = (char *)malloc(q->sizeOfItem * 1024) ;
      q->size = 1 ;
      return (void *)q->heap ;
   }
   do {
       if (q->hole != 0) {
          void* p = q->heap + q->sizeOfItem * q->hole ;
          q->hole = *(int *)p ;
          return p ;
       }
   } while (q->next != NULL && (q = q->next));

   if (q->size >= q->maxSize) {
      q->next = QuickHeapReservedNew(q->sizeOfItem, q->maxSize * 2) ;
      q->next->baseIndice = q->baseIndice + q->maxSize ;
      return QuickHeapAlloc(q->next) ;
   }

   return q->heap + q->sizeOfItem * q->size++ ;
}

int QuickHeapAllocIndice(QuickHeap q) {
   if (q->maxSize == 0) {
      q->maxSize = 1024 ;
      q->heap = (void *)malloc(q->sizeOfItem * 1024) ;
      q->size = 1 ;
      return 0 ;
   }
   do {
       if (q->hole) {
         int o = q->hole ;
         void* p = q->heap + q->sizeOfItem * o ;
         q->hole = *(int *)p ;
         return q->baseIndice + o ;
       }
   } while (q->next && (q = q->next));

   if (q->size >= q->maxSize) {
      q->next = QuickHeapReservedNew(q->sizeOfItem, q->maxSize * 2) ;
      q->next->baseIndice = q->baseIndice + q->maxSize ;
      return QuickHeapAllocIndice(q->next) ;
   }

   return q->baseIndice + q->size++;
}

void* QuickHeapGetPointer(QuickHeap q, int i) {
  int o = i - q->baseIndice;
  if (o < q->maxSize)
     return (void *)q->heap + q->sizeOfItem * o;
  else
     return QuickHeapGetPointer(q->next, i);
}

void QuickHeapFree(QuickHeap q, void *p) {
  char *pc = p;
  if (pc >= q->heap && pc < q->heap + q->sizeOfItem * q->maxSize) {
     int o = (pc - q->heap) / q->sizeOfItem;
     *(int *)p = q->hole;
     q->hole = o;
  } else
     QuickHeapFree(q->next, p);
}

void QuickHeapFreeIndice(QuickHeap q, int i) {
  int o = i - q->baseIndice;
  if (o >= 0 && o < q->maxSize) {
     *(int *)(q->heap + q->sizeOfItem * o) = q->hole;
     q->hole = o;
  } else
     QuickHeapFreeIndice(q->next, i);
}

