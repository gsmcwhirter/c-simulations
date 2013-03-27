#include "minunit.h"
#include <dlfcn.h>

char *
test_library()
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
all_tests() 
{
    mu_suite_start();

    mu_run_test(test_library);

    return NULL;
}

RUN_TESTS(all_tests);
