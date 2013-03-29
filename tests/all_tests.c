#include "minunit.h"
#include <dlfcn.h>
#include <math.h>
#include "replicator_population.h"
#include "replicator_game.h"

double effective_zero = 0.00000001;

char *
test_shared_library()
{
    void *handle = dlopen("./build/libreplicator_simulations.so", RTLD_NOW);
    char *dlerr = dlerror();
    mu_assert(dlerr == NULL, "Error opening the library to test.");
    mu_assert(handle != NULL, "Failed to open the library to test.");
    
    int rc = dlclose(handle);
    mu_assert(rc == 0, "Failed to close lib.");
    
    return NULL;
}

char *
test_population_create_destroy()
{
    population_t * pop = Population_create(3);
    mu_assert(pop != NULL, "Population_create returned null.");
    mu_assert(pop->size == 3, "Population size is incorrect");
    mu_assert(pop->proportions != NULL, "Population proportions are null.");
    
    Population_destroy(pop);
    
    return NULL;
}

char *
test_population_randomize()
{
    population_t * pop = Population_create(3);
    Population_randomize(pop);
    
    mu_assert(pop->size == 3, "Randomize messed up the size.");
    mu_assert(pop->proportions != NULL, "Randomize messed up the proportions pointer.");
    
    double total = 0;
    int i;
    for (i = 0; i < pop->size; i++){
        mu_assert(*(pop->proportions + i) >= 0 && *(pop->proportions + i) <= 1, "Proportions entry out of bounds.");
        total = total + *(pop->proportions + i);
    }
    mu_assert(fabs(total - 1) < effective_zero, "Proportions do not approximate a total of 1.");
    
    Population_destroy(pop);
    
    return NULL;
}

char *
test_population_equal()
{
    int size = 3;
    population_t * pop1, * pop2;
    pop1 = Population_create(size);
    pop2 = Population_create(size);
    
    Population_randomize(pop1);
    
    int i;
    for (i = 0; i < pop2->size; i++){
        *(pop2->proportions + i) = 0;
    }
    
    mu_assert(Population_equal(pop1, pop1, effective_zero), "Population 1 is not equal to itself.");
    mu_assert(Population_equal(pop2, pop2, effective_zero), "Population 2 is not equal to itself.");
    mu_assert(!Population_equal(pop1, pop2, effective_zero), "Populations are equal when they shouldn't be.");
    
    Population_destroy(pop1);
    Population_destroy(pop2);
    return NULL;
}

char *
test_population_copy()
{
    int size = 3;
    population_t * pop1, * pop2;
    pop1 = Population_create(size);
    pop2 = Population_create(size);
    
    Population_randomize(pop1);
    
    int i;
    for (i = 0; i < pop2->size; i++){
        *(pop2->proportions + i) = 0;
    }
    
    mu_assert(!Population_equal(pop1, pop2, effective_zero), "Populations are equal when they shouldn't be.");
    Population_copy(pop2, pop1);
    mu_assert(Population_equal(pop1, pop2, effective_zero), "Population not copied correctly.");
    
    Population_destroy(pop1);
    Population_destroy(pop2);
    
    return NULL;
}

char *
test_popcollection_create_destroy()
{
    int sizes1[] = {4};
    int sizes2[] = {3, 2};
    int i;
    
    popcollection_t * popc1 = PopCollection_create(1, sizes1);
    mu_assert(popc1 != NULL, "Can't create 1-population collection.");
    mu_assert(popc1->size == 1, "1-pop size is wrong.");
    mu_assert(popc1->pop_sizes != NULL, "1-pop pop_sizes is NULL.");
    mu_assert(*(popc1->pop_sizes) == 4, "1-pop pop_sizes is wrong.");
    mu_assert(popc1->populations != NULL, "1-pop populations is NULL.");
    mu_assert((*(popc1->populations))->size == 4, "1-pop population size is wrong.");
    
    popcollection_t * popc2 = PopCollection_create(2, sizes2);
    mu_assert(popc2 != NULL, "Can't create 2-population collection.");
    mu_assert(popc2->size == 2, "2-pop size is wrong.");
    mu_assert(popc2->pop_sizes != NULL, "2-pop pop_sizes is NULL.");
    for (i = 0; i < 2; i++){
        mu_assert(*(popc2->pop_sizes + i) == *(sizes2 + i), "2-pop pop_sizes is wrong.");
    }
    mu_assert(popc2->populations != NULL, "2-pop populations is NULL.");
    for (i = 0; i < 2; i++){
        mu_assert((*(popc2->populations + i))->size == *(sizes2 + i), "2-pop population size is wrong.");
    }
    
    PopCollection_destroy(popc1);
    PopCollection_destroy(popc2);
        
    return NULL;
}

char *
test_popcollection_clone()
{
    int sizes[] = {3, 2, 4};
    int i;
    
    popcollection_t * popc1 = NULL, * popc2 = NULL;
    popc1 = PopCollection_create(3, sizes);
    popc2 = PopCollection_clone(popc1);
    
    mu_assert(popc2 != NULL, "Population_clone returned NULL.");
    mu_assert(popc2->size == popc1->size, "New population size mismatch.");
    mu_assert(popc2->pop_sizes != NULL, "Cloned pop_sizes is NULL.");
    for (i = 0; i < popc1->size; i++){
        mu_assert(*(popc2->pop_sizes + i) == *(popc1->pop_sizes + i), "New population pop_sizes mismatch.");
    }
    mu_assert(popc2->populations != NULL, "Cloned populations is NULL.");
    for (i = 0; i < popc1->size; i++){
        mu_assert((*(popc2->populations + i))->size == (*(popc1->populations + i))->size, "Cloned populations size mismatch.");
    }
    
    PopCollection_destroy(popc1);
    PopCollection_destroy(popc2);
    
    return NULL;
}

char *
test_popcollection_randomize()
{
    int sizes[] = {3, 2, 4};
    int i, j;

    popcollection_t * popc = PopCollection_create(3, sizes);
    PopCollection_randomize(popc);
    
    double total;
    double prop;
    
    for (i = 0; i < popc->size; i++){
        total = 0;
        for (j = 0; j < *(popc->pop_sizes + i); j++){
            prop = *((*(popc->populations + i))->proportions + j);
            mu_assert(prop >= 0 && prop <= 1, "Proportions entry out of bounds.");
            total = total + prop;
        }
        mu_assert(fabs(total - 1) < effective_zero, "Proportions do not approximate a total of 1.");
    }
    
    PopCollection_destroy(popc);
    return NULL;
}

char *
test_popcollection_equal()
{
    int sizes[] = {2, 3, 4};
    popcollection_t *popc1, *popc2;
    popc1 = PopCollection_create(3, sizes);
    popc2 = PopCollection_clone(popc1);
    
    PopCollection_randomize(popc1);
    int i, j;
    for (i = 0; i < popc2->size; i++){
        for (j = 0; j < *(popc2->pop_sizes + i); j++){
            *((*(popc2->populations + i))->proportions + j) = 0;
        }
    }
    
    mu_assert(PopCollection_equal(popc1, popc1, effective_zero), "PopC1 is not equal to itself.");
    mu_assert(PopCollection_equal(popc2, popc2, effective_zero), "PopC2 is not equal to itself.");
    mu_assert(!PopCollection_equal(popc1, popc2, effective_zero), "PopC1 and PopC2 are equal when they shouldn't be.");
    
    PopCollection_destroy(popc1);
    PopCollection_destroy(popc2);
    return NULL;
}

char *
test_popcollection_copy()
{
    int sizes[] = {2, 3, 4};
    popcollection_t *popc1, *popc2;
    popc1 = PopCollection_create(3, sizes);
    popc2 = PopCollection_clone(popc1);
    
    PopCollection_randomize(popc1);
    int i, j;
    for (i = 0; i < popc2->size; i++){
        for (j = 0; j < *(popc2->pop_sizes + i); j++){
            *((*(popc2->populations + i))->proportions + j) = 0;
        }
    }
    
    mu_assert(!PopCollection_equal(popc1, popc2, effective_zero), "PopC1 and PopC2 are equal when they shouldn't be.");
    PopCollection_copy(popc2, popc1);
    mu_assert(PopCollection_equal(popc1, popc2, effective_zero), "PopC1 and PopC2 are not equal when they should be.");
    
    PopCollection_destroy(popc1);
    PopCollection_destroy(popc2);
    return NULL;
}

char *
test_strategyprofiles_create_destroy()
{
    
    return NULL;
}

char *
test_game_create_destroy()
{

    return NULL;    
}

char *
test_game_strategyprofiles()
{
    
    return NULL;
}

char *
test_game_popcollection()
{
    
    return NULL;
}

char *
all_tests() 
{
    mu_suite_start();

    mu_run_test(test_shared_library);
    mu_run_test(test_population_create_destroy);
    mu_run_test(test_population_randomize);
    mu_run_test(test_population_equal);
    mu_run_test(test_population_copy);
    
    mu_run_test(test_popcollection_create_destroy);
    mu_run_test(test_popcollection_clone);
    mu_run_test(test_popcollection_randomize);
    mu_run_test(test_popcollection_equal);
    mu_run_test(test_popcollection_copy);
    
    mu_run_test(test_strategyprofiles_create_destroy);
    mu_run_test(test_game_create_destroy);
    mu_run_test(test_game_strategyprofiles);
    mu_run_test(test_game_popcollection);

    return NULL;
}

RUN_TESTS(all_tests);
