#include "minunit.h"
#include <dlfcn.h>
#include "simulations/urnlearning_urns.h"
#include "simulations/urnlearning_game.h"
#include "simulations/urnlearning_simulation.h"
#include "simulations/randomkit.h"

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
    double initial_counts[] = {1.0, 1.0, 2.0};
    urn_t * urn1 = Urn_create(3, initial_counts);
    mu_assert(urn1 != NULL, "Could not create urn 1.");
    mu_assert(urn1->types == 3, "Urn 1 types were wrong.");
    mu_assert(urn1->counts != NULL, "Urn 1 counts were NULL.");
    mu_assert(*(urn1->counts + 0) == 1.0, "Urn 1 count 0 was wrong.");
    mu_assert(*(urn1->counts + 1) == 1.0, "Urn 1 count 1 was wrong.");
    mu_assert(*(urn1->counts + 2) == 2.0, "Urn 1 count 2 was wrong.");
    mu_assert(urn1->proportions != NULL, "Urn 1 proportions were NULL.");
    mu_assert(*(urn1->proportions + 0) == 0.25, "Urn 1 proportion 0 was wrong.");
    mu_assert(*(urn1->proportions + 1) == 0.25, "Urn 1 proportion 1 was wrong.");
    mu_assert(*(urn1->proportions + 2) == 0.50, "Urn 1 proportion 2 was wrong.");
    
    urn_t * urn2 = Urn_create(4, NULL);
    mu_assert(urn2 != NULL, "Could not create urn 2.");
    mu_assert(urn2->types == 4, "Urn 2 types were wrong.");
    mu_assert(urn2->counts != NULL, "Urn 2 counts were NULL.");
    mu_assert(*(urn2->counts + 0) == 0.0, "Urn 2 count 0 was wrong.");
    mu_assert(*(urn2->counts + 1) == 0.0, "Urn 2 count 1 was wrong.");
    mu_assert(*(urn2->counts + 2) == 0.0, "Urn 2 count 2 was wrong.");
    mu_assert(*(urn2->counts + 3) == 0.0, "Urn 2 count 3 was wrong.");
    mu_assert(urn2->proportions != NULL, "Urn 2 proportions were NULL.");
    mu_assert(*(urn2->proportions + 0) == 0.0, "Urn 2 proportion 0 was wrong.");
    mu_assert(*(urn2->proportions + 1) == 0.0, "Urn 2 proportion 1 was wrong.");
    mu_assert(*(urn2->proportions + 2) == 0.0, "Urn 2 proportion 2 was wrong.");
    mu_assert(*(urn2->proportions + 3) == 0.0, "Urn 2 proportion 3 was wrong.");
    
    Urn_destroy(urn1);
    Urn_destroy(urn2);
    
    return NULL;
}

char *
test_urn_update()
{
    double initial_counts[] = {1.0, 1.0, 2.0};
    urn_t * urn1 = Urn_create(3, initial_counts);
    
    double updates1[] = {2.0, 2.0, 0.0};
    Urn_update(urn1, updates1);
    mu_assert(*(urn1->counts + 0) == 3.0, "Urn 1 count 0 was wrong.");
    mu_assert(*(urn1->counts + 1) == 3.0, "Urn 1 count 1 was wrong.");
    mu_assert(*(urn1->counts + 2) == 2.0, "Urn 1 count 2 was wrong.");
    mu_assert(*(urn1->proportions + 0) == 0.375, "Urn 1 proportion 0 was wrong.");
    mu_assert(*(urn1->proportions + 1) == 0.375, "Urn 1 proportion 1 was wrong.");
    mu_assert(*(urn1->proportions + 2) == 0.25, "Urn 1 proportion 2 was wrong.");
    
    urn_t * urn2 = Urn_create(4, NULL);
    double updates2[] = {1.0, 2.0, 3.0, 4.0};
    Urn_update(urn2, updates2);
    mu_assert(*(urn2->counts + 0) == 1.0, "Urn 2 count 0 was wrong.");
    mu_assert(*(urn2->counts + 1) == 2.0, "Urn 2 count 1 was wrong.");
    mu_assert(*(urn2->counts + 2) == 3.0, "Urn 2 count 2 was wrong.");
    mu_assert(*(urn2->counts + 3) == 4.0, "Urn 2 count 3 was wrong.");
    mu_assert(*(urn2->proportions + 0) == 0.1, "Urn 2 proportion 0 was wrong.");
    mu_assert(*(urn2->proportions + 1) == 0.2, "Urn 2 proportion 1 was wrong.");
    mu_assert(*(urn2->proportions + 2) == 0.3, "Urn 2 proportion 2 was wrong.");
    mu_assert(*(urn2->proportions + 3) == 0.4, "Urn 2 proportion 3 was wrong.");
    
    Urn_destroy(urn1);
    Urn_destroy(urn2);
    
    return NULL;
}

char *
test_urn_select()
{
    double initial[] = {1.0, 2.0, 3.0, 4.0};
    urn_t * urn = Urn_create(4, initial);
    
    unsigned int selected;
    selected = Urn_select(urn, 0.05);
    mu_assert(selected == 0, "Selection failed for 0.05");
    selected = Urn_select(urn, 0.1);
    mu_assert(selected == 0, "Selection failed for 0.1");
    selected = Urn_select(urn, 0.25);
    mu_assert(selected == 1, "Selection failed for 0.25");
    selected = Urn_select(urn, 0.301);
    mu_assert(selected == 2, "Selection failed for 0.301");
    selected = Urn_select(urn, 0.559);
    mu_assert(selected == 2, "Selection failed for 0.559");
    selected = Urn_select(urn, 0.675);
    mu_assert(selected == 3, "Selection failed for 0.675");
    selected = Urn_select(urn, 1.0);
    mu_assert(selected == 3, "Selection failed for 1.0");
    selected = Urn_select(urn, 1.1);
    mu_assert(selected == 4, "Selection failed for 1.0");
    
    Urn_destroy(urn);
    
    return NULL;
}

char *
test_urn_randomselect()
{
    double initial[] = {1.0, 2.0, 3.0, 4.0};
    urn_t * urn = Urn_create(4, initial);
    
    rk_state rand_state;
    rk_randomseed(&rand_state);
    
    int num_draws = 5;
    int i;
    unsigned int selected;
    for (i = 0; i < num_draws; i++){
        selected = Urn_randomSelect(urn, &rand_state);
        mu_assert(selected < 4, "Random urn selection out of bounds (rand_state).");
        selected = Urn_randomSelect(urn, NULL);
        mu_assert(selected < 4, "Random urn selection out of bounds (NULL).");
    }
    
    Urn_destroy(urn);
    
    return NULL;
}

char *
test_urncollection_create_destroy()
{
    unsigned int urns1 = 3;
    unsigned int types1[] = {2, 3, 4};
    double **initial_counts = malloc(urns1 * sizeof(double *));
    unsigned int i, j;
    for (i = 0; i < urns1; i++){
        *(initial_counts + i) = malloc(*(types1 + i) * sizeof(double));
        for (j = 0; j < *(types1 + i); j++){
            *(*(initial_counts + i) + j) = 1.0;
        }
    }
    
    urncollection_t * coll1 = UrnCollection_create(urns1, types1, initial_counts);
    mu_assert(coll1 != NULL, "UrnCollection 1 not created.");
    mu_assert(coll1->num_urns == urns1, "UrnCollection 1 size was wrong.");
    mu_assert(coll1->urns != NULL, "UrnCollection 1 urns were NULL.");
    for (i = 0; i < coll1->num_urns; i++){
        mu_assert(*(coll1->urns + i) != NULL, "UrnCollection 1 urn was NULL.");
        mu_assert((*(coll1->urns + i))->types == *(types1 + i), "UrnCollection 1 urn was wrong.");
        mu_assert((*(coll1->urns + i))->counts != NULL, "UrnCollection 1 urn counts were NULL.");
        mu_assert((*(coll1->urns + i))->proportions != NULL, "UrnCollection 1 urn proportions were NULL.");
        
        for (j = 0; j < (*(coll1->urns + i))->types; j++){
            mu_assert(*((*(coll1->urns + i))->counts + j) == 1.0, "UrnCollection 1 urn counts were wrong.");
            mu_assert(*((*(coll1->urns + i))->proportions + j) == (1.0 / (double)(*(types1 + i))), "UrnCollection 1 urn proportions were wrong.");
        }
    }
    
    unsigned int urns2 = 4;
    unsigned int types2[] = {2, 3, 4, 5};
    urncollection_t * coll2 = UrnCollection_create(urns2, types2, NULL);
    mu_assert(coll2 != NULL, "UrnCollection 2 not created.");
    mu_assert(coll2->num_urns == urns2, "UrnCollection 2 size was wrong.");
    mu_assert(coll2->urns != NULL, "UrnCollection 2 urns were NULL.");
    for (i = 0; i < coll2->num_urns; i++){
        mu_assert(*(coll2->urns + i) != NULL, "UrnCollection 2 urn was NULL.");
        mu_assert((*(coll2->urns + i))->types == *(types2 + i), "UrnCollection 2 urn was wrong.");
        mu_assert((*(coll2->urns + i))->counts != NULL, "UrnCollection 2 urn counts were NULL.");
        mu_assert((*(coll2->urns + i))->proportions != NULL, "UrnCollection 2 urn proportions were NULL.");
        
        for (j = 0; j < (*(coll2->urns + i))->types; j++){
            mu_assert(*((*(coll2->urns + i))->counts + j) == 0.0, "UrnCollection 2 urn counts were wrong.");
            mu_assert(*((*(coll2->urns + i))->proportions + j) == 0.0, "UrnCollection 2 urn proportions were wrong.");
        }
    }
    
    
    for (i = 0; i < urns1; i++){
        free(*(initial_counts + i));
    }
    free(initial_counts);
    UrnCollection_destroy(coll1);
    UrnCollection_destroy(coll2);
    return NULL;
}

char *
test_urngame_create_destroy()
{
    unsigned int num_players = 3;
    unsigned int urn_counts[] = {2, 3, 4};
    unsigned int **types = malloc(num_players * sizeof(unsigned int *));
    double ***initial_counts = malloc(num_players * sizeof(double **));
    unsigned int i;
    unsigned int j;
    unsigned int k;
    for (i = 0; i < num_players; i++){
        *(types + i) = malloc(*(urn_counts + i) * sizeof(unsigned int));
        *(initial_counts + i) = malloc(*(urn_counts + i) * sizeof(double *));
        for (j = 0; j < *(urn_counts + i); j++){
            *(*(types + i) + j) = j + 1;
            *(*(initial_counts + i) + j) = malloc((j + 1) * sizeof(double));
            for (k = 0; k < (j + 1); k++){
                *(*(*(initial_counts + i) + j) + k) = 1.0;
            }
        }
    }
    
    urngame_t * game1 = UrnGame_create(num_players, urn_counts, types, initial_counts, NULL);
    mu_assert(game1 != NULL, "UrnGame 1 was NULL.");
    mu_assert(game1->num_players == num_players, "UrnGame 1 players were wrong.");
    mu_assert(game1->types != NULL, "UrnGame 1 types were NULL.");
    mu_assert(game1->player_urns != NULL, "UrnGame 1 player_urns were NULL.");
    mu_assert(game1->interaction_function != NULL, "UrnGame 1 interaction_function was NULL.");
    for (i = 0; i < num_players; i++){
        mu_assert(*(game1->types + i) != NULL, "UrnGame 1 type entries were NULL.");
        mu_assert(*(game1->player_urns + i) != NULL, "UrnGame 1 player_urns entries were NULL.");
        mu_assert((*(game1->player_urns + i))->num_urns == *(urn_counts + i), "UrnGame 1 types were wrong.");
        for (j = 0; j < (*(game1->player_urns + i))->num_urns; j++){
            mu_assert(*(*(game1->types + i) + j) == j+1, "UrnGame 1 types were wrong.");
            mu_assert((*((*(game1->player_urns + i))->urns + j))->types == j+1, "UrnGame 1 types were wrong.");
            for (k = 0; k < (*((*(game1->player_urns + i))->urns + j))->types; k++){
                mu_assert(*((*((*(game1->player_urns + i))->urns + j))->counts + k) == 1.0, "UrnGame 1 counts were wrong.");
                mu_assert(*((*((*(game1->player_urns + i))->urns + j))->proportions + k) == (1.0 / (double)((*((*(game1->player_urns + i))->urns + j))->types)), "UrnGame 1 proportions were wrong.");
            }
        }
    }
    
    urngame_t * game2 = UrnGame_create(num_players, urn_counts, types, NULL, NULL);
    mu_assert(game2 != NULL, "UrnGame 2 was NULL.");
    mu_assert(game2->num_players == num_players, "UrnGame 2 players were wrong.");
    mu_assert(game2->types != NULL, "UrnGame 2 types were NULL.");
    mu_assert(game2->player_urns != NULL, "UrnGame 2 player_urns were NULL.");
    mu_assert(game2->interaction_function != NULL, "UrnGame 2 interaction_function was NULL.");
    for (i = 0; i < num_players; i++){
        mu_assert(*(game2->types + i) != NULL, "UrnGame 2 type entries were NULL.");
        mu_assert(*(game2->player_urns + i) != NULL, "UrnGame 2 player_urns entries were NULL.");
        mu_assert((*(game2->player_urns + i))->num_urns == *(urn_counts + i), "UrnGame 2 types were wrong.");
        for (j = 0; j < (*(game2->player_urns + i))->num_urns; j++){
            mu_assert(*(*(game2->types + i) + j) == j+1, "UrnGame 2 types were wrong.");
            mu_assert((*((*(game2->player_urns + i))->urns + j))->types == j+1, "UrnGame 2 types were wrong.");
            for (k = 0; k < (*((*(game2->player_urns + i))->urns + j))->types; k++){
                mu_assert(*((*((*(game2->player_urns + i))->urns + j))->counts + k) == 0.0, "UrnGame 2 counts were wrong.");
                mu_assert(*((*((*(game2->player_urns + i))->urns + j))->proportions + k) == 0.0, "UrnGame 2 proportions were wrong.");
            }
        }
    }
    
    
    for (i = 0; i < num_players; i++){
        free(*(types + i));
        for (j = 0; j < *(urn_counts + i); j++){
            free(*(*(initial_counts + i) + j));
        }
        free(*(initial_counts + i));
    }
    free(types);
    free(initial_counts);
    UrnGame_destroy(game1);
    UrnGame_destroy(game2);
    
    return NULL;
}

char *
test_default_urnlearning_interactions()
{
    
    return NULL;
}

char *
test_urn_simulation()
{
    return NULL;
}

char *
all_tests() 
{
    mu_suite_start();

    mu_run_test(test_shared_library);
    mu_run_test(test_urn_create_destroy);
    mu_run_test(test_urn_update);
    mu_run_test(test_urn_select);
    mu_run_test(test_urn_randomselect);
    
    mu_run_test(test_urncollection_create_destroy);
    
    mu_run_test(test_urngame_create_destroy);
    mu_run_test(test_default_urnlearning_interactions);
    
    mu_run_test(test_urn_simulation);

    return NULL;
}

RUN_TESTS(all_tests);
