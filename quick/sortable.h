#ifndef SORTABLE_H
#define SORTABLE_H
/**
 * Sortable Lists
 * Experimental inline version of a Divide & Conquer type sort algorithm
 */

// A settable function to compare lists. Please set it before sorting!
// must return (arg1 < arg2)
extern int (*ListCompareFunc)(void*, void*);
#define ListSetCompareFunc(p) ListCompareFunc = (p)

// A classical linked list of orderable data
typedef struct sList {
    void* data;
    struct sList* next;
    struct sList* quick /* reserved for quicktable */;
}* List;

// create list
List ListNew();

// free list and return next
List ListFree(List p);

#define ListData(l) ((l)->data)
#define ListNext(l) ((l)->next)
#define ListSetData(l, d) ((l)->data = (d))
#define ListSetNext(l, n) ((l)->next = (n))
#define ListCompare(A, B) ListCompareFunc((A)->data, (B)->data)
#define ListPrependElement(l, n) (ListSetNext((n), (l)), (l) = (n))
#define ListPrepend(l, d) \
    { \
        List _nXYZ = ListNew(); \
        ListSetData(_nXYZ, (d)); \
        ListPrependElement((l), _nXYZ); \
    }

// merge 2 lists
List ListMerge(List previous, List after);

// cut list at end of the first rise, and returns the next rise
List ListExtractRise(List l);

// repeat until exhaustion of the unordered list :
//  - merge-sort a sub-list which weights as much as the already ordered list
//  - then merge both as a weight+1 ordered list
List ListSort(List list);

#endif
