#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "replicator_dynamics/replicator_population.h"
#include "replicator_dynamics/randomkit.h"
#include "replicator_dynamics/distributions.h"

population_t *
Population_create(int size)
{
    population_t *pop = malloc(sizeof(population_t));
    assert(pop != NULL);
    
    pop->size = size;
    pop->proportions = malloc(size * sizeof(double));
    assert(pop->proportions != NULL);
    
    return pop;
}

void 
Population_destroy(population_t *pop)
{
    if (pop != NULL){
        free(pop->proportions);
        free(pop);
        pop = NULL;
    }
}

int
Population_equal(population_t *pop1, population_t *pop2, double effective_zero)
{
    assert(pop1 != NULL);
    assert(pop2 != NULL);
    assert(pop1->size == pop2->size);
    
    int i;
    for (i = 0; i < pop1->size; i++){
        if (fabs(*(pop1->proportions + i) - *(pop2->proportions + i)) > effective_zero){
            return 0;
        }
    }
    
    return 1;
}

void
Population_copy(population_t *target, population_t *source)
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
Population_randomize(population_t *pop){
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

    int i;    
    int k = pop->size;
    double acc, invacc;
    
    rk_state rand_state;
    rk_randomseed(&rand_state);
    
    double *diric = malloc(k * sizeof(double));
    
    for (i = 0; i < k; i++){
        *(diric + i) = 0;
    }
    
    i = 0;
    acc = 0.0;
    for (i = 0; i < k; i++){
        *(diric + i) = rk_standard_gamma(&rand_state, 1.0); //the 1 is alpha=1. This generates an exponential eventually.
        acc = acc + *(diric + i);
    }
    invacc = 1/acc;
    for (i = 0; i < k; i++){
        *(pop->proportions + i) = *(diric + i) * invacc;
    }
    
    free(diric);
}

const char pop_serial_prefix[] = "pop";

void
Population_serialize(population_t *pop, FILE *file)
{
    assert(pop != NULL);
    assert(file != NULL);    
    
    unsigned int written;
    written = fwrite(pop_serial_prefix, sizeof(char), 3, file);
    written += fwrite(&(pop->size), sizeof(int), 1, file);
    
    assert(written == 4);
    
    written = fwrite(pop->proportions, sizeof(double), pop->size, file);
    assert(written == pop->size);
}

population_t *
Population_deserialize(FILE *file)
{
    //assume trying to read from the file wherever the file pointer is.
    assert(file != NULL);
    
    char *prefix = malloc(sizeof(char) * 4);
    *(prefix + 3) = '\0';
    int size;
    int reads;
    
    assert(prefix != NULL);
    
    reads = fread(prefix, sizeof(char), 3, file);
    assert(reads == 3);
    assert(!strcmp(prefix, pop_serial_prefix));
    
    free(prefix);
    
    reads = fread(&size, sizeof(int), 1, file);
    assert(reads == 1);
    assert(size > 0);
    
    population_t *pop = Population_create(size);
    assert(pop != NULL);
    
    reads = fread(pop->proportions, sizeof(double), pop->size, file);
    assert(reads == pop->size);
    
    return pop;
}

popcollection_t *
PopCollection_create(int num_pops, int *sizes)
{
    popcollection_t *coll = malloc(sizeof(popcollection_t));
    assert(coll != NULL);
    
    coll->size = num_pops;
    coll->pop_sizes = malloc(num_pops * sizeof(int));
    assert(coll->pop_sizes != NULL);
    
    int i;
    for (i = 0; i < coll->size; i++){
        *(coll->pop_sizes + i) = *(sizes + i);
    }
    
    coll->populations = malloc(coll->size * sizeof(population_t *));
    assert(coll->populations != NULL);
    
    for (i = 0; i < coll->size; i++){
        *(coll->populations + i) = Population_create(sizes[i]);
    }
    
    return coll;
}

popcollection_t *
PopCollection_clone(popcollection_t *original)
{
    assert(original != NULL);
    popcollection_t *clon = PopCollection_create(original->size, original->pop_sizes);
    
    return clon;
}

void
PopCollection_destroy(popcollection_t *coll){
    if (coll != NULL){
        int i;
        for (i = 0; i < coll->size; i++){
            Population_destroy(*(coll->populations + i));
        }
        free(coll->pop_sizes);
        free(coll->populations);
        free(coll);
        coll = NULL;
    }
}

int 
PopCollection_equal(popcollection_t *coll1, popcollection_t *coll2, double effective_zero)
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
PopCollection_copy(popcollection_t *target, popcollection_t *source)
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
PopCollection_randomize(popcollection_t *coll)
{
    assert(coll != NULL);
    
    int i;
    for (i = 0; i < coll->size; i++){
        Population_randomize(*(coll->populations + i));
    }
}

const char col_serial_prefix[] = "col";

void
PopCollection_serialize(popcollection_t *coll, FILE *file)
{
    assert(coll != NULL);
    assert(file != NULL);    
    
    unsigned int written;
    written = fwrite(col_serial_prefix, sizeof(char), 3, file);
    written += fwrite(&(coll->size), sizeof(int), 1, file);
    written += fwrite(coll->pop_sizes, sizeof(int), coll->size, file);
    assert(written == (4 + coll->size));
    
    int i;
    for (i = 0; i < coll->size; i++){
        Population_serialize(*(coll->populations + i), file);
    }
}

popcollection_t *
PopCollection_deserialize(FILE *file)
{
    //assume trying to read from the file wherever the file pointer is.
    assert(file != NULL);
    
    char *prefix = malloc(sizeof(char) * 4);
    *(prefix + 3) = '\0';
    int size;
    int *types;
    int reads;
    
    assert(prefix != NULL);
    
    reads = fread(prefix, sizeof(char), 3, file);
    assert(reads == 3);
    assert(!strcmp(prefix, col_serial_prefix));
    
    free(prefix);
    
    reads = fread(&size, sizeof(int), 1, file);
    assert(reads == 1);
    assert(size > 0);
    
    types = malloc(sizeof(int) * size);
    assert(types != NULL);
    reads = fread(types, sizeof(int), size, file);
    assert(reads == size);
    
    popcollection_t * coll = PopCollection_create(size, types);
    assert(coll != NULL);
    
    free(types);
    
    population_t * tmppop = Population_create(1);
    int i;
    for (i = 0; i < coll->size; i++){
        Population_destroy(tmppop);
        tmppop = Population_deserialize(file);
        Population_copy(*(coll->populations + i), tmppop);
    }
    Population_destroy(tmppop);
    
    return coll;
}
