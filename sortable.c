/* ========================================================================== */
/*                                                                            */
/*   Experimental inline version of a Divide & Conquer type sort algorihm     */
/*   Support: Garden Sylvain sylvain.garden@gmail.com                         */
/*                                                                            */
/*   Description:                                                             */
/*   This code aims a O(n) complexity when the to-be-ordered list is the      */
/*   concatenation of y (y << n) median-sized sub-lists.                      */
/*   There are a lot of applicative case.                                     */
/* ========================================================================== */
#include <stdlib.h>
#include "sortable.h"
#include "quickheap.h"

// A Heap for lists
static QuickHeapS listHeapS = QuickHeapInitializerFor(struct sList) ;
static QuickHeap listHeap = &listHeapS ;

// A settable function to compare lists. Please set it before sorting!
// must return (arg1 < arg2)
int (*ListDataPrior)(void *, void *) = NULL ;

// create a list item
List ListNew() {
  QuickHeapReserve(listHeap, 2048); // This will work only once. That's fine.
  return (List)QuickHeapAlloc(listHeap) ;
}

// free a list item and return next
List ListFree(List p) {
  List next = p->next ;
  QuickHeapFree(listHeap, (void *)p) ;
  return next ;
}

// merge 2 lists
// sort direction is kept by altering links in place
List ListMerge(List previous, List after) {
   List head, last, tmp ;

   if (!previous && !after) return NULL ;

   if (!previous || after && ListPrior(after, previous))
      { // 1st swapping of after & previous lists
      tmp = previous ;
      previous = after ;
      after = tmp ;
      }
   head = last = previous ;
   previous = ListNext(last) ;

   while (previous && after)
     {
       if (ListPrior(after, previous))
         {
           // swap after & previous lists // xor swap?
           tmp = previous ;
           previous = after ;
           after = tmp ;
           // correct link to previous
           ListSetNext(last, previous) ;
         }
       last = previous ;
       previous = ListNext(last) ;
     }

   if (after)
      ListSetNext(last, after) ;

   return head ;
}

// cut list at end of the first rise, and returns the next rise
List ListGetNextRise(List list) {
   List last, l ;

   if (!list) return NULL ;

   // sorting break search loop
   for (last = l = list, l = ListNext(l) ;
        l && !ListPrior(l, last) ;
        last = l, l = ListNext(l)) ;

   // cut l at ordering break
   if (l) ListSetNext(last, NULL) ;

   return l ;
}

// look for n=2^level consecutive rises in a list
// and merge them each others according to a binary tree scheme
// in 'list' : a list
// in 'level' : binary tree size
// returns: the ordered sub-list
// out 'remaining' : the remaining unordered part of the list
static List down(List list, int level, List *remaining) {
  List sub_remaining, l1, l2 ;
  if (!list)
     {
     *remaining = NULL ;
     return NULL ;
     }

  if (!level)
     {
     *remaining = ListGetNextRise(list) ;
     return list ;
     }

  l1 = down(list, level-1, &sub_remaining) ;
  l2 = down(sub_remaining, level-1, remaining) ;
  return ListMerge(l1, l2) ;
}

// repeat until exhaustion of the unordered part of the list :
//  merge-sort a part of list of same weight than the already ordered list
//  then merge both as a weight+1 ordered list

List inlineDivideAndConquerSort(List list) {
  List done = NULL, remaining, ordered ;
  int level = 0 ;
  if (!list) return NULL ;
  done = list ;
  list = ListGetNextRise(list) ;
  while (list) {
     ordered = down(list, level, &remaining)  ;
     done = ListMerge(done, ordered) ;
     level++ ;
     list = remaining ;
  }
  return done ;
}

// compile the test exe with "gcc -Dtest foo.c -o test"
#ifdef TEST
#include <stdio.h>

int intDataPrior(void *a, void *b) {
   return ((int)a) >= ((int b)) ;
}

void main()
{
   List demo = NULL, orderedList ;

   ListSetDataPrior(intDataPrior) ;

   // create a random list of integers
   for (int i = 0 ; i < 1000 ; i++) {
      List n = ListNew() ;

      ListSetNext(n, demo) ;

      {
        int* ip = malloc(int) ;
        *ip = (int)random(10000) ;
        ListSetData(n, ip) ;
      }

      demo = n ;
   }

   orderedList = inlineDivideAndConquerSort(demo) ;

   for (List p = orderedList ; p ;  p = ListNext(p) ) {
      printf("%d\t", *((int *)ListData(p)) ;
   }
   printf("\n") ;

   while (orderedList) {
     free(ListData(orderedList)) ;
     orderedList = ListFree(orderedList) ;
   }
}

#endif // TEST
