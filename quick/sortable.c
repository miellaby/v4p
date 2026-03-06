/**
 * inline version of a merge sort algorithm with stride detections for lists.
 *
 * Description:
 * This code aims a O(n) complexity when the to-be-ordered list is the
 * concatenation of a few medium-sized sub-lists.
 * In V4p, Active Edge Lists are mostly sorted after updating scan-line intersections,
 * and this is the reason why this algorithm is used.
 */
#include <stdlib.h>
#include "sortable.h"
#include "heap.h"

// A Heap for lists
static QuickHeapS listHeapS = QuickHeapInitializerFor(struct sList);
static QuickHeap listHeap = &listHeapS;

// A settable function to compare lists. Please set it before sorting!
// Must return (arg1 < arg2)
int (*ListCompareFunc)(void*, void*) = NULL;

// create a list item
List ListNew() {
    // QuickHeapReserve(listHeap, 2048); // This will work only once. That's
    // fine.
    return (List) QuickHeapAlloc(listHeap);
}

// free a list item and return next
List ListFree(List p) {
    List next = p->next;
    QuickHeapFree(listHeap, (void*) p);
    return next;
}

// merge 2 lists
// sort direction is kept by altering links in place
List ListMerge(List previous, List after) {
    List head, last, tmp;

    if (! previous && ! after)
        return NULL;

    if (! previous
        || (after && ListCompare(after, previous))) {  // 1st swapping of after & previous lists
        tmp = previous;
        previous = after;
        after = tmp;
    }
    head = last = previous;
    previous = ListNext(last);

    while (previous && after) {
        if (ListCompare(after, previous)) {
            // swap after & previous lists // xor swap?
            tmp = previous;
            previous = after;
            after = tmp;
            // correct link to previous
            ListSetNext(last, previous);
        }
        last = previous;
        previous = ListNext(last);
    }

    if (after)
        ListSetNext(last, after);

    return head;
}

// cut list at end of the first rise, and returns the next rise
List ListExtractRise(List list) {
    List last, l;

    if (! list)
        return NULL;

    // sorting break search loop
    for (last = l = list, l = ListNext(l); l && ! ListCompare(l, last); last = l, l = ListNext(l))
        ;

    // cut l at ordering break
    if (l)
        ListSetNext(last, NULL);

    return l;
}

// look for n=2^level consecutive rises in a list
// and merge them each others according to a binary tree scheme
// in 'list' : a list
// in 'level' : binary tree size
// returns: the ordered sub-list
// out 'remaining' : the remaining unordered part of the list
static List down(List list, int level, List* remaining) {
    List sub_remaining, l1, l2;
    if (! list) {
        *remaining = NULL;
        return NULL;
    }

    if (! level) {
        *remaining = ListExtractRise(list);
        return list;
    }

    l1 = down(list, level - 1, &sub_remaining);
    l2 = down(sub_remaining, level - 1, remaining);
    return ListMerge(l1, l2);
}

// repeat until exhaustion of the unordered part of the list :
//  merge-sort a part of list of same weight than the already ordered list
//  then merge both as a weight+1 ordered list

List ListSort(List list) {
    List done = NULL, remaining, ordered;
    int level = 0;
    if (! list)
        return NULL;
    done = list;
    list = ListExtractRise(list);
    while (list) {
        ordered = down(list, level, &remaining);
        done = ListMerge(done, ordered);
        level++;
        list = remaining;
    }
    return done;
}
