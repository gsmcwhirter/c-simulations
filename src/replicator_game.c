#include <assert.h>
#include <stdlib.h>
#include "replicator_dynamics/replicator_population.h"
#include "replicator_dynamics/replicator_game.h"
    
strategyprofiles_t *
StrategyProfiles_create(int players, int *types, cache_mask cache){
    assert(types != NULL);
    
    strategyprofiles_t *sprofs = malloc(sizeof(strategyprofiles_t));
    assert(sprofs != NULL);
    
    sprofs->count = 1;
    sprofs->size = players;
    sprofs->types = malloc(players * sizeof(int));
    
    int i;
    
    for (i = 0; i < players; i++){
        sprofs->count = *(types + i) * sprofs->count;
        *(sprofs->types + i) = *(types + i);
        sprofs->has_cached_info = !!(cache & CACHE_PROFILES);
        /*
        if (do_cache_info > 0){
            sprofs->has_cached_info = 1;
        }
        else {
            sprofs->has_cached_info = 0;
        }
        */
    }
    
    if (sprofs->has_cached_info == 1){
        int j, l, m, player_types, num_repeats, num_entries_per_repeat, repeat_spread, tmp, player_strat_count;

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
                        *(*(sprofs->profiles + l * repeat_spread + j * num_entries_per_repeat + m) + i) = j;
                        *(*(*(sprofs->player_strategy_profiles + i) + j) + player_strat_count) = l * repeat_spread + m + j * num_entries_per_repeat;
                        player_strat_count++; 
                    }
                }
            }
            
            num_repeats = num_repeats * player_types;
        }
    }
    else {
        sprofs->profiles = NULL;
        sprofs->player_strategy_profiles = NULL;
    }
    
    return sprofs;
}

int *
StrategyProfiles_getProfile(strategyprofiles_t *sprofs, int num)
{
    assert(sprofs != NULL);
    int *profile = malloc(sprofs->size * sizeof(int));
    int i;
    
    if (sprofs->has_cached_info){
        for (i = 0; i < sprofs->size; i++){
            *(profile + i) = *((*(sprofs->profiles + num)) + i);
        }
    }
    else {
        //printf("Called for num: %i\n", num);
        int num_repeats = 1;
        int repeat_spread, num_entries_per_repeat;
        for (i = 0; i < sprofs->size; i++){ //player
            repeat_spread = sprofs->count / num_repeats;
            num_entries_per_repeat = repeat_spread / *(sprofs->types + i);
            
            //num == l * repeat_spread + j * num_entries_per_repeat + m
            //j = (num - l * repeat_spread - m) / num_entries_per_repeat
            //set m = 0
            //j = (num - l * repeat_spread) / num_entries_per_repeat
            //l is the n-th repeat
            //l = num / repeat_spread
            //NOTE: integer division and multiplication are NOT inverses
            //j = (num - (num / repeat_spread) * repeat_spread) / num_entries_per_repeat
            
            /*
            printf("num_repeats: %i\n", num_repeats);
            printf("count: %i\n", sprofs->count);
            printf("repeat_spread: %i\n", repeat_spread);
            printf("nepr: %i\n", num_entries_per_repeat);
            printf("l: %i\n", num / repeat_spread);
            printf("j: %i\n", (num - (num / repeat_spread) * repeat_spread) / num_entries_per_repeat);
            printf("\n");
            */
            *(profile + i) = (num - (num / repeat_spread) * repeat_spread) / num_entries_per_repeat;
            
            num_repeats = num_repeats * (*(sprofs->types + i));
        }
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
    
    if (sprofs->has_cached_info){
        num2 = *((*((*(sprofs->player_strategy_profiles + player)) + strategy)) + num);
    }
    else {
        num_repeats = 1;
        for (i = 0; i < player; i++){
            num_repeats = num_repeats * (*(sprofs->types + i));
        }
        repeat_spread = (sprofs->count) / num_repeats;
        num_entries_per_repeat = repeat_spread / (*(sprofs->types + player));
        
        m = num % num_entries_per_repeat;
        num2 = (num - m) * (*(sprofs->types + player)) + m + strategy * repeat_spread / (*(sprofs->types + player));
    }
    
    return num2;
}

void
StrategyProfiles_destroy(strategyprofiles_t *sprofs){
    if (sprofs != NULL){
        if (sprofs->has_cached_info){
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
        }
        
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
Game_StrategyProfiles_create(game_t *game, cache_mask cache)
{
    assert(game != NULL);
    
    strategyprofiles_t *sprofs = StrategyProfiles_create(game->players, game->types, cache);
    
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

payoffcache_t * 
PayoffCache_create(game_t *game, strategyprofiles_t *profiles, cache_mask do_cache)
{
    assert(game != NULL);
    
    int i;
    int *profile;
    
    payoffcache_t *cache = malloc(sizeof(payoffcache_t));
    assert(cache != NULL);
    
    cache->free_profiles = 0;
    if (profiles == NULL){
        cache->profiles = Game_StrategyProfiles_create(game, do_cache);
        cache->free_profiles = 1;
    }
    else {
        cache->profiles = profiles;
    }
    
    cache->count = (cache->profiles)->count;
    cache->has_cached_info = !!(do_cache & CACHE_PAYOFFS);
    /*
    cache->has_cached_info = 0;
    if (do_cache_info > 0){
        cache->has_cached_info = 1;
    }
    */
    cache->payoffs = game->payoffs;
    
    if (cache->has_cached_info == 1){
        cache->payoff_cache = malloc(cache->count * sizeof(double *));
        assert(cache->payoff_cache != NULL);
        
        for (i = 0; i < (cache->profiles)->count; i++){ 
            assert(*(cache->payoff_cache + i) != NULL);
            
            profile = StrategyProfiles_getProfile(cache->profiles, i);
            //*(cache->payoff_cache + i) = game->payoffs(profiles->size, *(profiles->profiles + i));
            *(cache->payoff_cache + i) = game->payoffs((cache->profiles)->size, profile);
            free(profile); 
        }
    }
    else {
        cache->payoff_cache = NULL;
    }
    
    return cache;
}

double *
PayoffCache_getPayoffs(payoffcache_t *cache, int profile_index)
{
    assert(cache != NULL);
    double *payoffs;
    int *profile;
    int i;
    
    if (cache->has_cached_info){
        payoffs = malloc((cache->profiles)->size * sizeof(double));
        for (i = 0; i < (cache->profiles)->size; i++){
            *(payoffs + i) = *((*(cache->payoff_cache + profile_index)) + i);
        }
    }
    else {
        profile = StrategyProfiles_getProfile(cache->profiles, profile_index);
        payoffs = cache->payoffs((cache->profiles)->size, profile);
        free(profile);
    }
    
    return payoffs;
}

void
PayoffCache_destroy(payoffcache_t *cache){
    int i;
    if (cache != NULL){
        if (cache->has_cached_info){
            for (i = 0; i < cache->count; i++){
                free(*(cache->payoff_cache + i));
            }
            
            free(cache->payoff_cache);
        }
        
        if (cache->free_profiles == 1){
            StrategyProfiles_destroy(cache->profiles);
        }
        
        free(cache);
    }
}
