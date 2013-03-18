#include <assert.h>
#include <stdlib.h>
#include "replicator_population.h"
#include "replicator_game.h"
    
struct StrategyProfiles *
StrategyProfiles_create(int players, int *types){
    assert(types != NULL);
    
    struct StrategyProfiles *sprofs = malloc(sizeof(struct StrategyProfiles));
    assert(sprofs != NULL);
    
    sprofs->count = 1;
    sprofs->size = players;
    sprofs->types = malloc(players * sizeof(int));
    
    int i, j, l, m, player_types, num_entries, num_bunches, entries_per_bunch, bunch_spread, tmp, player_strat_count; 
    
    for (i = 0; i < players; i++){
        sprofs->count = *(types + i) * sprofs->count;
        *(sprofs->types + i) = *(types + i);
    }
    
    sprofs->profiles = malloc(sprofs->count * sizeof(int*));
    assert(sprofs->profiles != NULL);
    
    for (i = 0; i < sprofs->count; i++){
        *(sprofs->profiles + i) = malloc(sprofs->size * sizeof(int));
        assert(*(sprofs->profiles + i) != NULL);
    }
    
    sprofs->player_strategy_profiles = malloc(players * sizeof(int*));
    assert(sprofs->player_strategy_profiles != NULL);
    
    num_bunches = 1;
    for (i = 0; i < players; i++){
        *(sprofs->player_strategy_profiles + i) = malloc(*(types + i) * sizeof(int*));
        assert(*(sprofs->player_strategy_profiles + i) != NULL);
    
        player_types = *(types + i);
        tmp = sprofs->count / player_types;
        
        for (j = 0; j < player_types; j++){
            *(*(sprofs->player_strategy_profiles + i) + j) = malloc(tmp * sizeof(int));
            assert(*(*(sprofs->player_strategy_profiles + i) + j) != NULL);
            
            num_entries = sprofs->count / player_types;
            entries_per_bunch = num_entries / num_bunches;
            bunch_spread = sprofs->count / num_bunches;
            player_strat_count = 0;
            
            for (l = 0; l < num_bunches; l++){
                for (m = 0; m < entries_per_bunch; m++){
                    player_strat_count++;
                    *(*(sprofs->profiles + l * bunch_spread + m) + i) = j;
                    *(*(*(sprofs->player_strategy_profiles + i) + j) + player_strat_count) = l * bunch_spread + m; 
                }
            }
        }
        
        num_bunches = num_bunches * player_types;
    }
    
    return sprofs;
}

void
StrategyProfiles_destroy(struct StrategyProfiles *sprofs){
    if (sprofs != NULL){
        int i, j;
        for (i = 0; i < sprofs->count; i++){
            free(*(sprofs->profiles + i));
        }
        free(sprofs->profiles);
        
        for (i = 0; i < sprofs->size; i++){
            for (j = 0; j < *(sprofs->types + i); j++){
                free(*(*(sprofs->player_strategy_profiles + i) + j));
            }
            free(*(sprofs->player_strategy_profiles + i));
        }
        free(sprofs->player_strategy_profiles);
        
        free(sprofs);
    }
}

struct Game *
Game_create(int players, int populations, int *types, payoff_function payoffs)
{
    assert(players > 0);
    assert(populations > 0);

    struct Game *game = malloc(sizeof(struct Game));
    assert(game != NULL);
    
    assert(populations == 1 || players == populations);
    
    game->players = players;
    game->populations = populations;
    game->types = malloc(players * sizeof(int));
    assert(game->types != NULL);
    
    int i;
    
    for (i = 0; i < players; i++){
        if (populations == 1){
            *(game->types + i) = *types;
        }
        else {
            *(game->types + i) = *(types + i);
        }
    }
    
    game->payoffs = payoffs;
    return game;
}

void
Game_destroy(struct Game *game)
{
    if (game != NULL){
        free(game->types);
        free(game);
    }
}

struct StrategyProfiles *
Game_StrategyProfiles_create(struct Game *game)
{
    assert(game != NULL);
    
    struct StrategyProfiles *sprofs = StrategyProfiles_create(game->players, game->types);
    
    return sprofs;
}

struct PopCollection *
Game_PopCollection_create(struct Game *game)
{
    assert(game != NULL);
    struct PopCollection *pop = PopCollection_create(game->populations, game->types);
    assert(pop != NULL);
    
    return pop;
}
