#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "simulations/urnlearning_urns.h"
#include "simulations/urnlearning_game.h"
#include "simulations/randomkit.h"

urngame_t *
UrnGame_create(unsigned int num_players, unsigned int *num_urns, unsigned int **types, double ***initial_counts, urn_interaction func)
{
    assert(num_urns != NULL);
    assert(types != NULL);
    
    unsigned int i, j;
    
    urngame_t *urngame = malloc(sizeof(urngame_t));
    assert(urngame != NULL);
    
    urngame->num_players = num_players;
    urngame->types = malloc(urngame->num_players * sizeof(unsigned int *));
    assert(urngame->types != NULL);
    
    for (i = 0; i < num_players; i++){
        *(urngame->types + i) = malloc(*(num_urns + i) * sizeof(unsigned int));
        assert(*(urngame->types + i) != NULL);
        for (j = 0; j < *(num_urns + i); j++){
            *(*(urngame->types + i) + j) = *(*(types + i) + j); 
        }
    }
    
    if (func != NULL){
        urngame->interaction_function = func;    
    }
    else {
        urngame->interaction_function = default_urnlearning_interaction;
    }
    urngame->player_urns = malloc(urngame->num_players * sizeof(urncollection_t *));
    assert(urngame->player_urns != NULL);
    
    for (i = 0; i < urngame->num_players; i++){
        *(urngame->player_urns + i) = UrnCollection_create(*(num_urns + i), *(types + i), (initial_counts == NULL) ? NULL : *(initial_counts + i));
    }
    
    return urngame;
}

void
UrnGame_destroy(urngame_t *urngame)
{
    unsigned int i;
    if (urngame != NULL){
        for (i = 0; i < urngame->num_players; i++){
            UrnCollection_destroy(*(urngame->player_urns + i));
            
            free(*(urngame->types + i));
        }
        free(urngame->types);
        free(urngame->player_urns);
        free(urngame);
    }
}

unsigned int * 
default_urnlearning_interaction(unsigned int players, urncollection_t **player_urns, rk_state rand_state)
{
    unsigned int * state_action_profile = malloc((players + 1) * sizeof(unsigned int));
    
    unsigned int i;
    unsigned int state;
    state = (unsigned int)rk_interval((*(player_urns + 0))->num_urns, &rand_state);
    unsigned int last_action = state;
    for (i = 0; i < players; i++){
        *(state_action_profile + i) = Urn_select(*((*(player_urns + i))->urns + last_action), rand_state);
        last_action = *(state_action_profile + i); 
    }
    
    #ifndef NDEBUG
    printf("Interaction:\n");
    printf("\tState: %i\n", state);
    printf("\tActions:");
    for (i = 0; i < players; i++){
        printf("  %i", *(state_action_profile + i));
    }
    printf("\n");
    #endif
    
    *(state_action_profile + players) = state;
    
    return state_action_profile;
}
