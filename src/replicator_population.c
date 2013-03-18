#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "replicator_population.h"

struct Population *
Population_create(int size)
{
    struct Population *pop = malloc(sizeof(struct Population));
    assert(pop != NULL);
    
    pop->size = size;
    pop->proportions = malloc(size * sizeof(double));
    assert(pop->proportions != NULL);
    
    return pop;
}

void 
Population_destroy(struct Population *pop)
{
    if (pop != NULL){
        free(pop->proportions);
        free(pop);    
    }
}

int
Population_equal(struct Population *pop1, struct Population *pop2, double effective_zero)
{
    assert(pop1 != NULL);
    assert(pop2 != NULL);
    assert(pop1->size == pop2->size);
    
    int i;
    for (i = 0; i < pop1->size; i++){
        if (fabs(*(pop1->proportions + i) - *(pop2->proportions + i)) < effective_zero){
            return 0;
        }
    }
    
    return 1;
}

void
Population_copy(struct Population *target, struct Population *source)
{
    assert(source != NULL);
    assert(target != NULL);
    assert(source->size == target->size);
    
    int i;
    for (i = 0; i < source->size; i++){
        *(target->proportions + i) = *(source->proportions + i);
    }
}

void
Population_randomize(struct Population *pop){
    assert(pop != NULL);
    
    /* Lifted from NumPy and translated from cython to C
    
    cdef npy_intp   k
    cdef npy_intp   totsize
    cdef ndarray    alpha_arr, val_arr
    cdef double     *alpha_data, *val_data
    cdef npy_intp   i, j
    cdef double     acc, invacc

    k           = len(alpha)
    alpha_arr   = <ndarray>PyArray_ContiguousFromObject(alpha, NPY_DOUBLE, 1, 1)
    alpha_data  = <double*>PyArray_DATA(alpha_arr)

    if size is None:
        shape = (k,)
    elif type(size) is int:
        shape = (size, k)
    else:
        shape = size + (k,)

    diric   = np.zeros(shape, np.float64)
    val_arr = <ndarray>diric
    val_data= <double*>PyArray_DATA(val_arr)

    i = 0
    totsize = PyArray_SIZE(val_arr)
    while i < totsize:
        acc = 0.0
        for j from 0 <= j < k:
            val_data[i+j]   = rk_standard_gamma(self.internal_state, alpha_data[j])
            acc             = acc + val_data[i+j]
        invacc  = 1/acc
        for j from 0 <= j < k:
            val_data[i+j]   = val_data[i+j] * invacc
        i = i + k

    return diric
    */

    //TODO: implement    
    int k;
    int totsize;
    double *alpha_arr, *val_arr;
    double *alpha_data, *val_data;
    int i, j;
    double acc, invacc;

    /*
    k           = len(alpha)
    alpha_arr   = <ndarray>PyArray_ContiguousFromObject(alpha, NPY_DOUBLE, 1, 1)
    alpha_data  = <double*>PyArray_DATA(alpha_arr)

    if size is None:
        shape = (k,)
    elif type(size) is int:
        shape = (size, k)
    else:
        shape = size + (k,)

    diric   = np.zeros(shape, np.float64)
    val_arr = <ndarray>diric
    val_data= <double*>PyArray_DATA(val_arr)

    i = 0
    totsize = PyArray_SIZE(val_arr)
    while i < totsize:
        acc = 0.0
        for j from 0 <= j < k:
            val_data[i+j]   = rk_standard_gamma(self.internal_state, alpha_data[j])
            acc             = acc + val_data[i+j]
        invacc  = 1/acc
        for j from 0 <= j < k:
            val_data[i+j]   = val_data[i+j] * invacc
        i = i + k

    return diric
    */    
}

struct PopCollection *
PopCollection_create(int num_pops, int *sizes)
{
    struct PopCollection *coll = malloc(sizeof(struct PopCollection));
    assert(coll != NULL);
    
    coll->size = num_pops;
    coll->pop_sizes = malloc(num_pops * sizeof(int));
    assert(coll->pop_sizes != NULL);
    
    int i;
    for (i = 0; i < coll->size; i++){
        *(coll->pop_sizes + i) = *(sizes + i);
    }
    
    coll->populations = malloc(coll->size * sizeof(struct Population *));
    assert(coll->populations != NULL);
    
    for (i = 0; i < coll->size; i++){
        *(coll->populations + i) = Population_create(sizes[i]);
    }
    
    return coll;
}

struct PopCollection *
PopCollection_clone(struct PopCollection *original)
{
    assert(original != NULL);
    struct PopCollection *clon = PopCollection_create(original->size, original->pop_sizes);
    
    return clon;
}

void
PopCollection_destroy(struct PopCollection *coll){
    if (coll != NULL){
        int i;
        for (i = 0; i < coll->size; i++){
            Population_destroy(*(coll->populations + i));
        }
        free(coll->pop_sizes);
        free(coll->populations);
        free(coll);
    }
}

int 
PopCollection_equal(struct PopCollection *coll1, struct PopCollection *coll2, double effective_zero)
{
    assert(coll1 != NULL);
    assert(coll2 != NULL);
    assert(coll1->size == coll2->size);
    
    int i;
    for (i = 0; i < coll1->size; i++){
        if (!Population_equal(*(coll1->populations + i), *(coll2->populations + i), effective_zero)){
            return 0;
        }
    }
    
    return 1;
}

void
PopCollection_copy(struct PopCollection *target, struct PopCollection *source)
{
    assert(source != NULL);
    assert(target != NULL);
    assert(source->size == target->size);
    
    int i;
    for (i = 0; i < source->size; i++){
        Population_copy(*(target->populations + i), *(source->populations + i));
    }
}

void
PopCollection_randomize(struct PopCollection *coll)
{
    assert(coll != NULL);
    
    int i;
    for (i = 0; i < coll->size; i++){
        Population_randomize(*(coll->populations + i));
    }
}
