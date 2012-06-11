#ifndef SORTABLE_H
#define SORTABLE_H
/* ========================================================================== */
/*   Sortable Lists +                                                         */
/*   Experimental inline version of a Divide & Conquer type sort algorihm     */
/*   Support: Garden Sylvain sylvain.garden@gmail.com                         */
/* ========================================================================== */

// A settable function to compare lists. Please set it before sorting!
// must return (arg1 < arg2)
extern int (*ListDataPrior)(void *, void *) ;
#define ListSetDataPrior(p) ListDataPrior = (p)

// A classical linked list of orderable data
typedef struct sList { void *data ; struct sList *next ; struct sList *quick /* reserved for quicktable */ ; } *List ;

// create list
List ListNew() ;

// free list and return next
List ListFree(List p) ;

#define ListData(l) ((l)->data)
#define ListNext(l) ((l)->next)
#define ListSetNext(l,n) ((l)->next=(n))
#define ListSetData(l,d) ((l)->data=(d))
#define ListPrior(A,B) ListDataPrior((A)->data,(B)->data)
#define ListAdd(l,n) (ListSetNext((n), (l)), (l) = (n))
#define ListAddData(l,d) { List _nXYZ = ListNew() ; ListSetData(_nXYZ, (d)) ; ListAdd((l), _nXYZ) ; }

// merge 2 lists
List ListMerge(List previous, List after) ;

// cut list at end of the first rise, and returns the next rise
List ListGetNextRise(List l) ;

// repeat until exhaustion of the unordered list :
//  - merge-sort a sub-list which weights as much as the already ordered list
//  - then merge both as a weight+1 ordered list
List inlineDivideAndConquerSort(List list) ;
#define ListSort(l) inlineDivideAndConquerSort(l)

#endif

