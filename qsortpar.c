/***************************************************************************
 *
 * Sequential version of Quick sort
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define KILO (1024)
#define MEGA (1024*1024)
#define MAX_ITEMS (64*MEGA)
#define swap(v, a, b) {unsigned tmp; tmp=v[a]; v[a]=v[b]; v[b]=tmp;}

static int *v;

#include <time.h>

static double now(void)// helper for timing
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static void
print_array(void)
{
    int i;
    for (i = 0; i < MAX_ITEMS; i++)
        printf("%d ", v[i]);
    printf("\n");
}

static void
init_array(void)
{
    int i;
    v = (int *) malloc(MAX_ITEMS*sizeof(int));
    for (i = 0; i < MAX_ITEMS; i++)
        v[i] = rand();
}

static unsigned
partition(int *v, unsigned low, unsigned high, unsigned pivot_index)
{
    /* move pivot to the bottom of the vector */
    if (pivot_index != low)
        swap(v, low, pivot_index);

    pivot_index = low;
    low++;

    /* invariant:
     * v[i] for i less than low are less than or equal to pivot
     * v[i] for i greater than high are greater than pivot
     */

    /* move elements into place */
    while (low <= high) {
        if (v[low] <= v[pivot_index])
            low++;
        else if (v[high] > v[pivot_index])
            high--;
        else
            swap(v, low, high);
    }

    /* put pivot back between two groups */
    if (high != pivot_index)
        swap(v, pivot_index, high);
    return high;
}


struct sortArgs {// stuct for passing args to thread
    int *v;
    unsigned low;
    unsigned high;
    int depth;
};

static void* sort_thread(void *p); // forward declaring func as two bellow call eachother

static void
quick_sort(int *v, unsigned low, unsigned high, int depth)
{
    unsigned pivot_index;

    /* no need to sort a vector of zero or one element */
    if (low >= high)
        return;

    /* select the pivot value */
    pivot_index = (low+high)/2;

    /* partition the vector */
    pivot_index = partition(v, low, high, pivot_index);
    pthread_t thread;
    struct sortArgs args;
    /* sort the two sub arrays */
    if (depth < 4 && low < pivot_index){
        args.v = v;
        args.low = low;
        args.high = pivot_index-1;
        args.depth = depth+1;
        pthread_create(&(thread), // our handle for the child
        NULL, // attributes of the child
        sort_thread, // the function it should run
        (void*)&args); // args to that function
    }
    else if (low < pivot_index)
    {
        quick_sort(v, low, pivot_index-1, depth+1);
    }
    
    if (pivot_index < high)
        quick_sort(v, pivot_index+1, high, depth+1);
    if (depth < 4 && low < pivot_index)pthread_join(thread, NULL );
}




static void* sort_thread(void *p)          // wrapper for passing args via struct in threads
{
    struct sortArgs *a = (struct sortArgs *) p;
    quick_sort(a->v, a->low, a->high, a->depth);
    return NULL;
}

int
main(int argc, char **argv)
{
    init_array();
    //print_array();
    double t0 = now();
    quick_sort(v, 0, MAX_ITEMS-1, 0);
    double t1 = now();
    //print_array();


    printf("sort time: %.3f s\n", t1 - t0);
}
