#include <assert.h>
#include <stdlib.h>
#include "replicator_dynamics/replicator_population.h"
#include "replicator_dynamics/replicator_game.h"
    
strategyprofiles_t *
StrategyProfiles_create(int players, int *types){
    assert(types != NULL);
    
    strategyprofiles_t *sprofs = malloc(sizeof(strategyprofiles_t));
    assert(sprofs != NULL);
    
    sprofs->count = 1;
    sprofs->size = players;
    sprofs->types = malloc(players * sizeof(int));
    
    int i;
    //int j, l, m, player_types, num_repeats, num_entries_per_repeat, repeat_spread, tmp, player_strat_count; 
    
    for (i = 0; i < players; i++){
        sprofs->count = *(types + i) * sprofs->count;
        *(sprofs->types + i) = *(types + i);
    }
    
    /*
    sprofs->profiles = malloc(sprofs->count * sizeof(int*));
    assert(sprofs->profiles != NULL);
    
    for (i = 0; i < sprofs->count; i++){
        *(sprofs->profiles + i) = malloc(sprofs->size * sizeof(int));
        assert(*(sprofs->profiles + i) != NULL);
    }
    
    sprofs->player_strategy_profiles = malloc(players * sizeof(int*));
    assert(sprofs->player_strategy_profiles != NULL);
    
    num_repeats = 1;
    for (i = 0; i < sprofs->size; i++){
        *(sprofs->player_strategy_profiles + i) = malloc(*(types + i) * sizeof(int*));
        assert(*(sprofs->player_strategy_profiles + i) != NULL);
    
        player_types = *(sprofs->types + i);
        tmp = sprofs->count / player_types;
        repeat_spread = sprofs->count / num_repeats;
        num_entries_per_repeat = repeat_spread / player_types;
        
        for (j = 0; j < player_types; j++){
            *(*(sprofs->player_strategy_profiles + i) + j) = malloc(tmp * sizeof(int));
            assert(*(*(sprofs->player_strategy_profiles + i) + j) != NULL);
            
            player_strat_count = 0;
            
            for (l = 0; l < num_repeats; l++){
                for (m = 0; m < num_entries_per_repeat; m++){
                    *(*(sprofs->profiles + l * repeat_spread + m + j * num_entries_per_repeat) + i) = j;
                    *(*(*(sprofs->player_strategy_profiles + i) + j) + player_strat_count) = l * repeat_spread + m + j * num_entries_per_repeat;
                    player_strat_count++; 
                }
            }
        }
        
        num_repeats = num_repeats * player_types;
    }
    */
    
    return sprofs;
}

int *
StrategyProfiles_getProfile(strategyprofiles_t *sprofs, int num)
{
    assert(sprofs != NULL);
    int *profile = malloc(sprofs->size * sizeof(int));
    int i;
    
    int num_repeats = 1;
    int repeat_spread, num_entries_per_repeat;
    for (i = 0; i < sprofs->size; i++){
        repeat_spread = sprofs->count / num_repeats;
        num_entries_per_repeat = repeat_spread / *(sprofs->types + i);
        
        //num == l * repeat_spread + m + j * num_entries_per_repeat
        //j = (num - l * repeat_spread - m) / num_entries_per_repeat
        //set m = 0, l = 0
        *(profile + i) = (num % repeat_spread) / num_entries_per_repeat;
        
        num_repeats = num_repeats * (*(sprofs->types + i));
    }
    
    return profile;
}


int *
StrategyProfiles_getPlayerProfile(strategyprofiles_t *sprofs, int player, int strategy, int num)
{       
    return StrategyProfiles_getProfile(sprofs, StrategyProfiles_getPlayerProfileNumber(sprofs, player, strategy, num));
}

int 
StrategyProfiles_getPlayerProfileNumber(strategyprofiles_t *sprofs, int player, int strategy, int num)
{
    assert(sprofs != NULL);
    
    /*
    num = l * num_entries_per_repeat + m
    m = num % num_entries_per_repeat
    l * num_entries_per_repeat = num - m
    num_entries_per_repeat = repeat_spread / *(sprofs->types + player)
    l * repeat_spread = (num - m) * (*(sprofs->types + player))
    num2 = l * repeat_spread + m + strategy * num_entries_per_repeat
    num2 = (num - m) * (*(sprofs->types + player)) + m + strategy * num_entries_per_repeat
    num2 = (num - m) * (*(sprofs->types + player)) + m + strategy * repeat_spread / *(sprofs->types + player)
    */
    int num2;
    int m;
    int i;
    int num_repeats;
    int repeat_spread;
    int num_entries_per_repeat;
    
    num_repeats = 1;
    for (i = 0; i < player; i++){
        num_repeats = num_repeats * (*(sprofs->types + i));
    }
    repeat_spread = (sprofs->count) / num_repeats;
    num_entries_per_repeat = repeat_spread / (*(sprofs->types + player));
    
    m = num % num_entries_per_repeat;
    num2 = (num - m) * (*(sprofs->types + player)) + m + strategy * repeat_spread / (*(sprofs->types + player));
    
    return num2;
}

void
StrategyProfiles_destroy(strategyprofiles_t *sprofs){
    if (sprofs != NULL){
        /*
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
        */
        free(sprofs->types);
        
        
        
        free(sprofs);
        sprofs = NULL;
    }
}

game_t *
Game_create(int players, int populations, int *types, payoff_function payoffs)
{
    assert(players > 0);
    assert(populations > 0);

    game_t *game = malloc(sizeof(game_t));
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
Game_destroy(game_t *game)
{
    if (game != NULL){
        free(game->types);
        free(game);
        game = NULL;
    }
}

strategyprofiles_t *
Game_StrategyProfiles_create(game_t *game)
{
    assert(game != NULL);
    
    strategyprofiles_t *sprofs = StrategyProfiles_create(game->players, game->types);
    
    return sprofs;
}

popcollection_t *
Game_PopCollection_create(game_t *game)
{
    assert(game != NULL);
    popcollection_t *pop = PopCollection_create(game->populations, game->types);
    assert(pop != NULL);
    
    return pop;
}
