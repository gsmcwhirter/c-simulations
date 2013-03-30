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
    strategyprofiles_t *pr1, *pr2, *pr3;
    int types1[] = {2};
    int types2[] = {2, 3};
    int types3[] = {2, 2, 2};
    int i, j, k, c, pspindex;
    
    int profiles1[][1] = {
        {0},
        {1}
    };
    int profiles2[][2] = {
        {0, 0},
        {0, 1},
        {0, 2},
        {1, 0},
        {1, 1},
        {1, 2}  
    };
    int profiles3[][3] = {
        {0, 0, 0},
        {0, 0, 1},
        {0, 1, 0},
        {0, 1, 1},
        {1, 0, 0},
        {1, 0, 1},
        {1, 1, 0},
        {1, 1, 1}
    };
    
    int psps1[] = {
        0, 1  
    };
    
    int psps2[] = {
        0, 1, 2, 3, 4, 5, 
        0, 3, 1, 4, 2, 5
    };
    
    int psps3[] = {
        0, 1, 2, 3, 4, 5, 6, 7,
        0, 1, 4, 5, 2, 3, 6, 7,
        0, 2, 4, 6, 1, 3, 5, 7
    }; // strat# * player_types + k + player# * count
    
    pr1 = StrategyProfiles_create(1, types1);
    mu_assert(pr1 != NULL, "SP_create returned NULL for 1 player.");
    mu_assert(pr1->size == 1, "1 player size is wrong.");
    mu_assert(pr1->count == 2, "1 player count is wrong.");
    mu_assert(pr1->types != NULL, "1 player types are NULL.");
    mu_assert(*(pr1->types) == 2, "1 player types are wrong.");
    mu_assert(pr1->profiles != NULL, "1 player profiles are NULL.");
    for (i = 0; i < pr1->count; i++){
        mu_assert((pr1->profiles + i) != NULL, "1 player profile entry is NULL.");
        for (j = 0; j < pr1->size; j++){
            mu_assert(*(*(pr1->profiles + i) + j) == *(*(profiles1 + i) + j), "1 player profile entries are wrong.");
        }
    }
    mu_assert(pr1->player_strategy_profiles != NULL, "1 player strategy profiles are NULL.");
    for (i = 0; i < pr1->size; i++){
        mu_assert(*(pr1->player_strategy_profiles + i) != NULL, "1 player psp player entry NULL.");
        for (j = 0; j < *(pr1->types + i); j++){
            mu_assert(*(*(pr1->player_strategy_profiles + i) + j) != NULL, "1 player psp player strat entry NULL.");
            c = pr1->count / *(pr1->types + i);
            for (k = 0; k < c; k++){
                pspindex = (i * pr1->count) + j * c + k;
                mu_assert(*(*(*(pr1->player_strategy_profiles + i) + j) + k) == psps1[pspindex], "1 player psp player strat entry wrong.");
            }
        }
    }
    
    
    pr2 = StrategyProfiles_create(2, types2);
    mu_assert(pr2 != NULL, "SP_create returned NULL for 2 players.");
    mu_assert(pr2->size == 2, "2 player size is wrong.");
    mu_assert(pr2->count == 6, "2 player count is wrong.");
    mu_assert(pr2->types != NULL, "2 player types are NULL.");
    for (i = 0; i < pr2->size; i++){
        mu_assert(*(pr2->types + i) == *(types2 + i), "2 player types are wrong.");
    }
    mu_assert(pr2->profiles != NULL, "2 player profiles are NULL.");
    for (i = 0; i < pr2->count; i++){
        mu_assert((pr2->profiles + i) != NULL, "2 player profile entry is NULL.");
        for (j = 0; j < pr2->size; j++){
            mu_assert(*(*(pr2->profiles + i) + j) == *(*(profiles2 + i) + j), "2 player profile entries are wrong.");
        }
    }
    
    mu_assert(pr2->player_strategy_profiles != NULL, "2 player strategy profiles are NULL.");
    for (i = 0; i < pr2->size; i++){
        mu_assert(*(pr2->player_strategy_profiles + i) != NULL, "2 player psp player entry NULL.");
        for (j = 0; j < *(pr2->types + i); j++){
            mu_assert(*(*(pr2->player_strategy_profiles + i) + j) != NULL, "2 player psp player strat entry NULL.");
            c = pr2->count / *(pr2->types + i);
            for (k = 0; k < c; k++){
                pspindex = (i * pr2->count) + j * c + k;
                mu_assert(*(*(*(pr2->player_strategy_profiles + i) + j) + k) == psps2[pspindex], "2 player psp player strat entry wrong.");
            }
        }
    }
    
    pr3 = StrategyProfiles_create(3, types3);
    mu_assert(pr3 != NULL, "SP_create returned NULL for 3 players.");
    mu_assert(pr3->size == 3, "3 player size is wrong.");
    mu_assert(pr3->count == 8, "3 player count is wrong.");
    mu_assert(pr3->types != NULL, "3 player types are NULL.");
    for (i = 0; i < pr3->size; i++){
        mu_assert(*(pr3->types + i) == *(types3 + i), "3 player types are wrong.");
    }
    mu_assert(pr3->profiles != NULL, "3 player profiles are NULL.");
    for (i = 0; i < pr3->count; i++){
        mu_assert((pr3->profiles + i) != NULL, "3 player profile entry is NULL.");
        for (j = 0; j < pr3->size; j++){
            mu_assert(*(*(pr3->profiles + i) + j) == *(*(profiles3 + i) + j), "3 player profile entries are wrong.");
        }
    }
    
    mu_assert(pr3->player_strategy_profiles != NULL, "3 player strategy profiles are NULL.");
    for (i = 0; i < pr3->size; i++){
        mu_assert(*(pr3->player_strategy_profiles + i) != NULL, "3 player psp player entry NULL.");
        for (j = 0; j < *(pr3->types + i); j++){
            mu_assert(*(*(pr3->player_strategy_profiles + i) + j) != NULL, "3 player psp player strat entry NULL.");
            c = pr3->count / *(pr3->types + i);
            for (k = 0; k < c; k++){
                pspindex = (i * pr3->count) + j * c + k;
                mu_assert(*(*(*(pr3->player_strategy_profiles + i) + j) + k) == psps3[pspindex], "2 player psp player strat entry wrong.");
            }
        }
    }
    
    StrategyProfiles_destroy(pr1);
    StrategyProfiles_destroy(pr2);
    StrategyProfiles_destroy(pr3);
    
    return NULL;
}

char *
test_game_create_destroy()
{
    game_t *game1, *game2, *game3;
    
    int types1[] = {3};
    int types2[] = {4};
    int types3[] = {2, 4};
    
    game1 = Game_create(1, 1, types1);
    
    game2 = Game_create(2, 1, types2);
    
    game3 = Game_create(2, 2, types3);
    
    
    Game_destroy(game1);
    Game_destroy(game2);
    Game_destroy(game3);
    
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
