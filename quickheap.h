#ifndef QUICKHEAP_H
#define QUICKHEAP_H
/* ======================================================================= */
// Quick Heaps
/* ======================================================================= */
typedef struct sQuickHeap {
   int sizeOfItem ;
   char *heap ;
   int maxSize ;
   int size ;
   int baseIndice ;
   int hole ;
   struct sQuickHeap *next ;
} QuickHeapS, *QuickHeap ;

#define   QuickHeapInitializer(S) { (S), NULL, 0, 0, 0, 0, NULL }
#define   QuickHeapInitializerFor(T) QuickHeapInitializer(sizeof(T))
QuickHeap QuickHeapNew(int sizeOfItem) ;
void      QuickHeapDelete(QuickHeap q);
#define   QuickHeapNewFor(T) QuickHeapNew(sizeof(T))
void      QuickHeapReset(QuickHeap q) ;
void      QuickHeapReserve(QuickHeap q, int reservedSize);

// Pointer mode
void* QuickHeapAlloc(QuickHeap q) ;
void  QuickHeapFree(QuickHeap q, void *p) ;

// Indice mode
int QuickHeapAllocIndice(QuickHeap q);
void QuickHeapFreeIndice(QuickHeap q, int i);
void* QuickHeapGetPointer(QuickHeap q, int i);
#endif

