#include "minunit.h"
#include <dlfcn.h>
#include "simulations/urnlearning_urns.h"
#include "simulations/urnlearning_game.h"
#include "simulations/urnlearning_simulation.h"

#define UNUSED(x) (void)(x)

char *
test_shared_library()
{
    void *handle = dlopen("./build/liburnlearning.so", RTLD_NOW);
    char *dlerr = dlerror();
    mu_assert(dlerr == NULL, "Error opening the library to test.");
    mu_assert(handle != NULL, "Failed to open the library to test.");
    
    int rc = dlclose(handle);
    mu_assert(rc == 0, "Failed to close lib.");
    
    return NULL;
}

char *
test_urn_create_destroy()
{
    
    return NULL;
}

char *
test_urncollection_create_destroy()
{
    
    return NULL;
}

char *
test_urngame_create_destroy()
{
    
    return NULL;
}

char *
all_tests() 
{
    mu_suite_start();

    mu_run_test(test_shared_library);
    mu_run_test(test_urn_create_destroy);
    mu_run_test(test_urncollection_create_destroy);
    mu_run_test(test_urngame_create_destroy);

    return NULL;
}

RUN_TESTS(all_tests);
