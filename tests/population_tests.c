#include "minunit.h"
#include <dlfcn.h>
#include "replicator_population.h"

char *lib_file = "./build/libreplicator_simulations.so";
void *lib_handle = NULL;

char *
test_population()
{
    //Population_create
    population_t * (*pop_create)(int);
    pop_create = dlsym(lib_handle, "Population_create");
    mu_assert(pop_create != NULL, "Population_create not found.");
    
    population_t * pop = (*pop_create)(3);
    mu_assert(pop != NULL, "Population not created.");
    mu_assert(pop->size == 3, "Population doesn't have size 3.");
    mu_assert(pop->proportions != NULL, "Population proportions not initialized.");
    

    return NULL;    
}

char *
test_popcollection()
{
    
    return NULL;
}

char *
all_tests() 
{
    mu_suite_start();
    lib_handle = dlopen(lib_file, RTLD_NOW);
    mu_assert(lib_handle != NULL, "Failed to open library for testing.");
    
    mu_run_test(test_population);
    mu_run_test(test_popcollection);
    
    int rc = dlclose(lib_handle);
    mu_assert(rc == 0, "Failed to close the library after testing.");
    return NULL;
}

RUN_TESTS(all_tests);
