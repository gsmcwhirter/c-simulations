#include "minunit.h"
#include <dlfcn.h>
#include <math.h>
#include <assert.h>
#include "replicator_dynamics/replicator_population.h"
#include "replicator_dynamics/replicator_game.h"
#include "replicator_dynamics/replicator_simulation.h"

#define UNUSED(x) (void)(x)

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

double *
dummy_payoffs(int players, int *profile)
{
    double * payoffs = malloc(players * sizeof(double));
    int i;
    for (i = 0; i < players; i++){
        *(payoffs + i) = 2 * *(profile + i);
    }
    
    return payoffs;
}

char *
test_game_create_destroy()
{
    game_t *game1, *game2, *game3;
    
    int types1[] = {3};
    int types2[] = {4};
    int types3[] = {2, 4};
    
    int profile1[] = {0};
    int profile23[] = {0, 0};
    
    double *poffs1, *poffs2, *poffs3;
    
    int i;
    
    game1 = Game_create(1, 1, types1, dummy_payoffs);
    mu_assert(game1 != NULL, "GC returned NULL.");
    mu_assert(game1->players == 1, "G1 players were wrong.");
    mu_assert(game1->populations == 1, "G1 populations were wrong.");
    mu_assert(game1->types != NULL, "G1 types were NULL.");
    for (i = 0; i < game1->players; i++){
        mu_assert(*(game1->types + i) == *types1, "G1 types were wrong.");
    }
    mu_assert(game1->payoffs != NULL, "G1 payoffs were NULL.");
    
    poffs1 = game1->payoffs(1, profile1);
    for (i = 0; i < game1->players; i++){
        mu_assert(*(poffs1 + i) == 2 * *(profile1 + i), "G1 payoffs were wrong.");
    }
    free(poffs1);
    
    game2 = Game_create(2, 1, types2, dummy_payoffs);
    mu_assert(game2 != NULL, "GC returned NULL.");
    mu_assert(game2->players == 2, "G2 players were wrong.");
    mu_assert(game2->populations == 1, "G2 populations were wrong.");
    mu_assert(game2->types != NULL, "G2 types were NULL.");
    for (i = 0; i < game2->players; i++){
        mu_assert(*(game2->types + i) == *types2, "G2 types were wrong.");
    }
    mu_assert(game2->payoffs != NULL, "G2 payoffs were NULL.");
    
    poffs2 = game2->payoffs(2, profile23);
    for (i = 0; i < game2->players; i++){
        mu_assert(*(poffs2 + i) == 2 * *(profile23 + i), "G2 payoffs were wrong.");
    }
    free(poffs2);
    
    game3 = Game_create(2, 2, types3, dummy_payoffs);
    mu_assert(game3 != NULL, "GC returned NULL.");
    mu_assert(game3->players == 2, "G3 players were wrong.");
    mu_assert(game3->populations == 2, "G3 populations were wrong.");
    mu_assert(game3->types != NULL, "G3 types were NULL.");
    for (i = 0; i < game3->players; i++){
        mu_assert(*(game3->types + i) == *(types3 + i), "G3 types were wrong.");
    }
    mu_assert(game3->payoffs != NULL, "G3 payoffs were NULL.");
    
    poffs3 = game3->payoffs(2, profile23);
    for (i = 0; i < game3->players; i++){
        mu_assert(*(poffs3 + i) == 2 * *(profile23 + i), "G3 payoffs were wrong.");
    }
    free(poffs3);
    
    Game_destroy(game1);
    Game_destroy(game2);
    Game_destroy(game3);
    
    return NULL;
}

char *
test_game_strategyprofiles()
{
    int types[] = {2, 3, 4};
    int i;
    game_t * game = Game_create(3, 3, types, dummy_payoffs);
    strategyprofiles_t * sprofs = Game_StrategyProfiles_create(game);
    mu_assert(sprofs != NULL, "GSPC returned NULL.");
    mu_assert(sprofs->count == 24, "sprofs count was wrong.");
    mu_assert(sprofs->size == 3, "sprofs size was wrong.");
    for (i = 0; i < sprofs->size; i++){
        mu_assert(*(sprofs->types + i) == *(types + i), "sprofs types were wrong.");
    }
    
    StrategyProfiles_destroy(sprofs);
    Game_destroy(game);
    
    return NULL;
}

char *
test_game_popcollection()
{
    int types[] = {2, 3, 4};
    int i;
    game_t * game = Game_create(3, 3, types, dummy_payoffs);
    popcollection_t * pcoll = Game_PopCollection_create(game);
    mu_assert(pcoll != NULL, "GPCC returned NULL.");
    mu_assert(pcoll->size == 3, "pcoll size was wrong.");
    mu_assert(pcoll->pop_sizes != NULL, "pcoll pop_sizes were NULL.");
    for (i = 0; i < pcoll->size; i++){
        mu_assert(*(pcoll->pop_sizes + i) == *(types + i), "pcoll pop_sizes were wrong.");
    }
    mu_assert(pcoll->populations != NULL, "pcoll populations were NULL.");
    
    PopCollection_destroy(pcoll);
    
    Game_destroy(game);
    
    return NULL;
}

void
dummy_callback(game_t * game, int generation, popcollection_t *popc)
{
    printf("game players: %i\n", game->players);
    printf("generation: %i\n", generation);
    printf("populations: %i\n", popc->size);
}

double *
pd_payoffs(int players, int * profile){
    double payoffs[2][2] = {
        {3, 0},
        {4, 1}
    };
    
    assert(players == 2);
    assert(*(profile) == 0 || *(profile) == 1);
    assert(*(profile + 1) == 0 || *(profile + 1) == 1);
    
    double * act_payoffs = malloc(players * sizeof(double));
    
    *(act_payoffs) = payoffs[*(profile)][*(profile + 1)];
    *(act_payoffs + 1) = payoffs[*(profile + 1)][*(profile)]; 
    
    return act_payoffs;
}

char *
test_payoffcache_create_destroy()
{
    int i, j, k;
    int types[] = {2, 2};
    double correct_cache[][2] = {
        {3, 3},
        {0, 4},
        {4, 0},
        {1, 1}
    };
    game_t *game = Game_create(2, 2, types, pd_payoffs);
    strategyprofiles_t * sprofs = Game_StrategyProfiles_create(game);
    
    payoffcache_t *caches[2];
    caches[0] = PayoffCache_create(game, NULL);
    caches[1] = PayoffCache_create(game, sprofs);
    
    for (i = 0; i < 2; i++){
        mu_assert((caches[i])->count == 4, "Cache size is wrong.");
        mu_assert((caches[i])->payoff_cache != NULL, "Cache payoffs were null.");
        for (j = 0; j < 4; j++){
            for (k = 0; k < 2; k++){
                mu_assert(*(*((caches[i])->payoff_cache + j) + k) == correct_cache[j][k], "Cache payoffs were wrong.");    
            }
        }
    }
    
    PayoffCache_destroy(caches[0]);
    PayoffCache_destroy(caches[1]);
    StrategyProfiles_destroy(sprofs);
    Game_destroy(game);
    
    return NULL;
}

char *
test_earned_payoff_1pop()
{
    int types[] = {2, 2};
    game_t *game = Game_create(2, 1, types, pd_payoffs);
    strategyprofiles_t * profiles = Game_StrategyProfiles_create(game);
    payoffcache_t *cache = PayoffCache_create(game, profiles);
    popcollection_t *popc = Game_PopCollection_create(game);
    
    *((*(popc->populations))->proportions + 0) = 1;
    *((*(popc->populations))->proportions + 1) = 0;
    
    mu_assert(earned_payoff(0, 0, popc, profiles, cache) == 3.0, "Earned payoff 1 pl 0 str 0 was wrong.");
    mu_assert(earned_payoff(0, 1, popc, profiles, cache) == 4.0, "Earned payoff 1 pl 0 str 1 was wrong.");
    mu_assert(earned_payoff(1, 0, popc, profiles, cache) == 3.0, "Earned payoff 1 pl 1 str 0 was wrong.");
    mu_assert(earned_payoff(1, 1, popc, profiles, cache) == 4.0, "Earned payoff 1 pl 1 str 1 was wrong.");
    
    *((*(popc->populations))->proportions + 0) = 0.5;
    *((*(popc->populations))->proportions + 1) = 0.5;
    
    mu_assert(earned_payoff(0, 0, popc, profiles, cache) == 1.5, "Earned payoff 2 pl 0 str 0 was wrong.");
    mu_assert(earned_payoff(0, 1, popc, profiles, cache) == 2.5, "Earned payoff 2 pl 0 str 1 was wrong.");
    mu_assert(earned_payoff(1, 0, popc, profiles, cache) == 1.5, "Earned payoff 2 pl 1 str 0 was wrong.");
    mu_assert(earned_payoff(1, 1, popc, profiles, cache) == 2.5, "Earned payoff 2 pl 1 str 1 was wrong.");
    
    Game_destroy(game);
    StrategyProfiles_destroy(profiles);
    PayoffCache_destroy(cache);
    PopCollection_destroy(popc);
    
    return NULL;
}

char *
test_earned_payoff_2pop()
{
    int i;
    int types[] = {2, 2};
    game_t *game = Game_create(2, 2, types, pd_payoffs);
    strategyprofiles_t * profiles = Game_StrategyProfiles_create(game);
    payoffcache_t *cache = PayoffCache_create(game, profiles);
    popcollection_t *popc = Game_PopCollection_create(game);
    
    for (i = 0; i < 2; i++){
        *((*(popc->populations + i))->proportions + 0) = 1;
        *((*(popc->populations + i))->proportions + 1) = 0;
        
    }
    
    mu_assert(earned_payoff(0, 0, popc, profiles, cache) == 3.0, "Earned payoff 1 pl 0 str 0 was wrong.");
    mu_assert(earned_payoff(0, 1, popc, profiles, cache) == 4.0, "Earned payoff 1 pl 0 str 1 was wrong.");
    mu_assert(earned_payoff(1, 0, popc, profiles, cache) == 3.0, "Earned payoff 1 pl 1 str 0 was wrong.");
    mu_assert(earned_payoff(1, 1, popc, profiles, cache) == 4.0, "Earned payoff 1 pl 1 str 1 was wrong.");   
    
    for (i = 0; i < 2; i++){
        *((*(popc->populations + i))->proportions + 0) = 0.5;
        *((*(popc->populations + i))->proportions + 1) = 0.5;
    }
    
    mu_assert(earned_payoff(0, 0, popc, profiles, cache) == 1.5, "Earned payoff 2 pl 0 str 0 was wrong.");
    mu_assert(earned_payoff(0, 1, popc, profiles, cache) == 2.5, "Earned payoff 2 pl 0 str 1 was wrong.");
    mu_assert(earned_payoff(1, 0, popc, profiles, cache) == 1.5, "Earned payoff 2 pl 1 str 0 was wrong.");
    mu_assert(earned_payoff(1, 1, popc, profiles, cache) == 2.5, "Earned payoff 2 pl 1 str 1 was wrong.");
    
    Game_destroy(game);
    StrategyProfiles_destroy(profiles);
    PayoffCache_destroy(cache);
    PopCollection_destroy(popc);
    
    return NULL;
}

char *
test_average_earned_payoff_1pop()
{
    int types[] = {2, 2};
    game_t *game = Game_create(2, 1, types, pd_payoffs);
    strategyprofiles_t * profiles = Game_StrategyProfiles_create(game);
    payoffcache_t *cache = PayoffCache_create(game, profiles);
    popcollection_t *popc = Game_PopCollection_create(game);
    
    *((*(popc->populations))->proportions + 0) = 1;
    *((*(popc->populations))->proportions + 1) = 0;
    
    mu_assert(average_earned_payoff(0, popc, profiles, cache) == 3.0, "Average Earned payoff 1 was wrong.");
    
    *((*(popc->populations))->proportions + 0) = 0.5;
    *((*(popc->populations))->proportions + 1) = 0.5;
    
    mu_assert(average_earned_payoff(0, popc, profiles, cache) == 2.0, "Average Earned payoff 2 was wrong.");
    
    Game_destroy(game);
    StrategyProfiles_destroy(profiles);
    PayoffCache_destroy(cache);
    PopCollection_destroy(popc);
    
    return NULL;
}

char *
test_average_earned_payoff_2pop()
{
    int types[] = {2, 2};
    game_t *game = Game_create(2, 2, types, pd_payoffs);
    strategyprofiles_t * profiles = Game_StrategyProfiles_create(game);
    payoffcache_t *cache = PayoffCache_create(game, profiles);
    popcollection_t *popc = Game_PopCollection_create(game);
    
    *((*(popc->populations))->proportions + 0) = 1;
    *((*(popc->populations))->proportions + 1) = 0;
    
    *((*(popc->populations + 1))->proportions + 0) = 1;
    *((*(popc->populations + 1))->proportions + 1) = 0;
    
    mu_assert(average_earned_payoff(0, popc, profiles, cache) == 3.0, "Average Earned payoff 1-1 was wrong.");
    mu_assert(average_earned_payoff(1, popc, profiles, cache) == 3.0, "Average Earned payoff 1-2 was wrong.");
    
    *((*(popc->populations))->proportions + 0) = 0.5;
    *((*(popc->populations))->proportions + 1) = 0.5;
    
    *((*(popc->populations + 1))->proportions + 0) = 0.5;
    *((*(popc->populations + 1))->proportions + 1) = 0.5;
    
    mu_assert(average_earned_payoff(0, popc, profiles, cache) == 2.0, "Average Earned payoff 2-1 was wrong.");
    mu_assert(average_earned_payoff(1, popc, profiles, cache) == 2.0, "Average Earned payoff 2-2 was wrong.");
    
    Game_destroy(game);
    StrategyProfiles_destroy(profiles);
    PayoffCache_destroy(cache);
    PopCollection_destroy(popc);
    
    return NULL;
}

char *
test_update_population_proportions_1pop()
{
    int types[] = {2, 2};
    game_t *game = Game_create(2, 1, types, pd_payoffs);
    strategyprofiles_t * profiles = Game_StrategyProfiles_create(game);
    payoffcache_t *cache = PayoffCache_create(game, profiles);
    popcollection_t *popc = Game_PopCollection_create(game);
    popcollection_t *popc2 = PopCollection_clone(popc);
    
    
    *((*(popc->populations))->proportions + 0) = 0;
    *((*(popc->populations))->proportions + 1) = 1;
    
    *((*(popc2->populations))->proportions + 0) = 0;
    *((*(popc2->populations))->proportions + 1) = 0;
    
    update_population_proportions(0, 0, *(popc2->populations), popc, profiles, cache);
    
    mu_assert(*((*(popc2->populations))->proportions + 0) == 0, "Updating static population 0 was wrong.");
    mu_assert(*((*(popc2->populations))->proportions + 1) == 1, "Updating static population 1 was wrong.");
    
    *((*(popc->populations))->proportions + 0) = 0.5;
    *((*(popc->populations))->proportions + 1) = 0.5;
    
    *((*(popc2->populations))->proportions + 0) = 0;
    *((*(popc2->populations))->proportions + 1) = 0;
    
    update_population_proportions(0, 0, *(popc2->populations), popc, profiles, cache);
    
    mu_assert(*((*(popc2->populations))->proportions + 0) == 0.375, "Updating dynamic population 0 was wrong.");
    mu_assert(*((*(popc2->populations))->proportions + 1) == 0.625, "Updating dynamic population 1 was wrong.");
    
    Game_destroy(game);
    StrategyProfiles_destroy(profiles);
    PayoffCache_destroy(cache);
    PopCollection_destroy(popc);
    PopCollection_destroy(popc2);
    
    return NULL;
}

char *
test_update_population_proportions_2pop()
{
    int i;
    int types[] = {2, 2};
    game_t *game = Game_create(2, 2, types, pd_payoffs);
    strategyprofiles_t * profiles = Game_StrategyProfiles_create(game);
    payoffcache_t *cache = PayoffCache_create(game, profiles);
    popcollection_t *popc = Game_PopCollection_create(game);
    popcollection_t *popc2 = PopCollection_clone(popc);
    
    for (i = 0; i < 2; i++){
        *((*(popc->populations + i))->proportions + 0) = 0;
        *((*(popc->populations + i))->proportions + 1) = 1;
        
        *((*(popc2->populations + i))->proportions + 0) = 0;
        *((*(popc2->populations + i))->proportions + 1) = 0;
    }
    
    for (i = 0; i < 2; i++){    
        update_population_proportions(0, i, *(popc2->populations + i), popc, profiles, cache);
        
        mu_assert(*((*(popc2->populations + i))->proportions + 0) == 0, "Updating static population 0 was wrong.");
        mu_assert(*((*(popc2->populations + i))->proportions + 1) == 1, "Updating static population 1 was wrong.");
    }
    
    for (i = 0; i < 2; i++){
        *((*(popc->populations + i))->proportions + 0) = 0.5;
        *((*(popc->populations + i))->proportions + 1) = 0.5;
        
        *((*(popc2->populations + i))->proportions + 0) = 0;
        *((*(popc2->populations + i))->proportions + 1) = 0;
     }
     
     for (i = 0; i < 2; i++){   
        update_population_proportions(0, i, *(popc2->populations + i), popc, profiles, cache);
        
        mu_assert(*((*(popc2->populations + i))->proportions + 0) == 0.375, "Updating dynamic population 0 was wrong.");
        mu_assert(*((*(popc2->populations + i))->proportions + 1) == 0.625, "Updating dynamic population 1 was wrong.");
    }
    
    Game_destroy(game);
    StrategyProfiles_destroy(profiles);
    PayoffCache_destroy(cache);
    PopCollection_destroy(popc);
    PopCollection_destroy(popc2);
    
    return NULL;
}

int gencounter;

void
count_generations(game_t *game, int generation, popcollection_t *generation_pop)
{
    UNUSED(game);
    UNUSED(generation);
    UNUSED(generation_pop);
    gencounter++;
}

char *
test_replicator_dynamics_1pop()
{
    int types[] = {2, 2};
    game_t *game = Game_create(2, 1, types, pd_payoffs);
    popcollection_t *popc = Game_PopCollection_create(game);
    PopCollection_randomize(popc);
    
    popcollection_t *endpop1 = replicator_dynamics(game, popc, 0, effective_zero, 0, NULL);
    mu_assert(endpop1 != NULL, "RD 1 returned null.");
    mu_assert(fabs(0 - *((*(endpop1->populations))->proportions)) < effective_zero, "RD 1 returned wrong population 0.");
    mu_assert(fabs(1 - *((*(endpop1->populations))->proportions + 1)) < effective_zero, "RD 1 returned wrong population 1.");
    
    gencounter = 0;
    *((*(popc->populations))->proportions) = 0.5;
    *((*(popc->populations))->proportions + 1) = 0.5;
    popcollection_t *endpop2 = replicator_dynamics(game, popc, 0, effective_zero, 1, count_generations);
    mu_assert(endpop2 != NULL, "RD 2 returned NULL.");
    mu_assert(gencounter == 1, "RD 2 took too many generations.");
    mu_assert(fabs(*((*(endpop2->populations))->proportions) - 0.375) < effective_zero, "RD 2 returned wrong population 0.");
    mu_assert(fabs(*((*(endpop2->populations))->proportions + 1) - 0.625) < effective_zero, "RD 2 returned wrong population 1.");
    
    popcollection_t *endpop3 = replicator_dynamics(game, NULL, 0, effective_zero, 0, NULL);
    mu_assert(endpop3 != NULL, "RD 3 returned null.");
    mu_assert(fabs(*((*(endpop3->populations))->proportions) - 0) < effective_zero, "RD 3 returned wrong population 0.");
    mu_assert(fabs(*((*(endpop3->populations))->proportions + 1) - 1.0) < effective_zero, "RD 3 returned wrong population 1.");
    
    Game_destroy(game);
    PopCollection_destroy(popc);
    PopCollection_destroy(endpop1);
    PopCollection_destroy(endpop2);
    PopCollection_destroy(endpop3);
    
    return NULL;
}

char *
test_replicator_dynamics_2pop()
{
    int types[] = {2, 2};
    game_t *game = Game_create(2, 2, types, pd_payoffs);
    popcollection_t *popc = Game_PopCollection_create(game);
    PopCollection_randomize(popc);
    
    popcollection_t *endpop1 = replicator_dynamics(game, popc, 0, effective_zero, 0, NULL);
    mu_assert(endpop1 != NULL, "RD 1 returned null.");
    mu_assert(fabs(0 - *((*(endpop1->populations))->proportions)) < effective_zero, "RD 1 returned wrong population 0-0.");
    mu_assert(fabs(1 - *((*(endpop1->populations))->proportions + 1)) < effective_zero, "RD 1 returned wrong population 0-1.");
    mu_assert(fabs(0 - *((*(endpop1->populations + 1))->proportions)) < effective_zero, "RD 1 returned wrong population 1-0.");
    mu_assert(fabs(1 - *((*(endpop1->populations + 1))->proportions + 1)) < effective_zero, "RD 1 returned wrong population 1-1.");
    
    gencounter = 0;
    *((*(popc->populations))->proportions) = 0.5;
    *((*(popc->populations))->proportions + 1) = 0.5;
    *((*(popc->populations + 1))->proportions) = 0.5;
    *((*(popc->populations + 1))->proportions + 1) = 0.5;
    popcollection_t *endpop2 = replicator_dynamics(game, popc, 0, effective_zero, 1, count_generations);
    mu_assert(endpop2 != NULL, "RD 2 returned NULL.");
    mu_assert(gencounter == 1, "RD 2 took too many generations.");
    mu_assert(fabs(*((*(endpop2->populations))->proportions) - 0.375) < effective_zero, "RD 2 returned wrong population 0-0.");
    mu_assert(fabs(*((*(endpop2->populations))->proportions + 1) - 0.625) < effective_zero, "RD 2 returned wrong population 0-1.");
    mu_assert(fabs(*((*(endpop2->populations + 1))->proportions) - 0.375) < effective_zero, "RD 2 returned wrong population 1-0.");
    mu_assert(fabs(*((*(endpop2->populations + 1))->proportions + 1) - 0.625) < effective_zero, "RD 2 returned wrong population 1-1.");
    
    popcollection_t *endpop3 = replicator_dynamics(game, NULL, 0, effective_zero, 0, NULL);
    mu_assert(endpop3 != NULL, "RD 3 returned null.");
    mu_assert(fabs(*((*(endpop3->populations))->proportions) - 0) < effective_zero, "RD 3 returned wrong population 0-0.");
    mu_assert(fabs(*((*(endpop3->populations))->proportions + 1) - 1.0) < effective_zero, "RD 3 returned wrong population 0-1.");
    mu_assert(fabs(*((*(endpop3->populations + 1))->proportions) - 0) < effective_zero, "RD 3 returned wrong population 1-0.");
    mu_assert(fabs(*((*(endpop3->populations + 1))->proportions + 1) - 1.0) < effective_zero, "RD 3 returned wrong population 1-1.");
    
    Game_destroy(game);
    PopCollection_destroy(popc);
    PopCollection_destroy(endpop1);
    PopCollection_destroy(endpop2);
    PopCollection_destroy(endpop3);
    
    return NULL;
}

char *
test_serialization_pop()
{
    FILE *file = fopen("tests/pop.serial", "w");
    assert(file != NULL);
    
    population_t *pop = Population_create(2);
    *(pop->proportions) = 0.5;
    *(pop->proportions + 1) = 0.5;
    
    Population_serialize(pop, file);
    Population_destroy(pop);
    fclose(file);
    
    FILE *file2 = fopen("tests/pop.serial", "r");
    population_t * pop2 = Population_deserialize(file2);
    fclose(file2);
    mu_assert(pop2 != NULL, "Deserialization failed.");
    mu_assert(pop2->size == 2, "Deserialized size was wrong.");
    mu_assert(*(pop2->proportions) == 0.5, "Deserialized proportions were wrong.");
    mu_assert(*(pop2->proportions + 1) == 0.5, "Deserialized proportions were wrong.");
    
    Population_destroy(pop2);
    
    return NULL;
}

char *
test_serialization_coll()
{
    FILE *file = fopen("tests/coll.serial", "w");
    assert(file != NULL);
    
    int types[] = {3, 2};
    popcollection_t *coll = PopCollection_create(2, types);
    *((*(coll->populations + 0))->proportions + 0) = 0.125;
    *((*(coll->populations + 0))->proportions + 1) = 0.25;
    *((*(coll->populations + 0))->proportions + 2) = 0.625;
    
    *((*(coll->populations + 1))->proportions + 0) = 0.33;
    *((*(coll->populations + 1))->proportions + 1) = 0.67;
    PopCollection_serialize(coll, file);
    PopCollection_destroy(coll);
    fclose(file);
    
    FILE *file2 = fopen("tests/coll.serial", "r");
    popcollection_t *coll2 = PopCollection_deserialize(file2);
    fclose(file2);
    mu_assert(coll2 != NULL, "Deserialization failed.");
    mu_assert(coll2->size == 2, "Deserialized size was wrong.");
    mu_assert(coll2->pop_sizes != NULL, "Deserialized pop_sizes were NULL.");
    mu_assert(*(coll2->pop_sizes) == 3, "Deserialized pop_sizes were wrong (0).");
    mu_assert(*(coll2->pop_sizes + 1) == 2, "Deserialized pop_sizes were wrong (0).");
    mu_assert(coll2->populations != NULL, "Deserialized populations was NULL.");
    mu_assert(*((*(coll2->populations + 0))->proportions + 0) == 0.125, "Deserialized populations were wrong.");
    mu_assert(*((*(coll2->populations + 0))->proportions + 1) == 0.25, "Deserialized populations were wrong.");
    mu_assert(*((*(coll2->populations + 0))->proportions + 2) == 0.625, "Deserialized populations were wrong.");
    mu_assert(*((*(coll2->populations + 1))->proportions + 0) == 0.33, "Deserialized populations were wrong.");
    mu_assert(*((*(coll2->populations + 1))->proportions + 1) == 0.67, "Deserialized populations were wrong.");
    
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
    
    mu_run_test(test_payoffcache_create_destroy);
    mu_run_test(test_earned_payoff_1pop);
    mu_run_test(test_earned_payoff_2pop);
    mu_run_test(test_average_earned_payoff_1pop);
    mu_run_test(test_average_earned_payoff_2pop);
    mu_run_test(test_update_population_proportions_1pop);
    mu_run_test(test_update_population_proportions_2pop);
    mu_run_test(test_replicator_dynamics_1pop);
    mu_run_test(test_replicator_dynamics_2pop);
    
    mu_run_test(test_serialization_pop);
    mu_run_test(test_serialization_coll);

    return NULL;
}

RUN_TESTS(all_tests);
