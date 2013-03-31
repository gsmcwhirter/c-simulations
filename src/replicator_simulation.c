#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "replicator_dynamics/replicator_simulation.h"
#include "replicator_dynamics/replicator_population.h"
#include "replicator_dynamics/replicator_game.h"

payoffcache_t * 
PayoffCache_create(game_t *game, strategyprofiles_t *profiles)
{
    assert(game != NULL);
    
    int i;
    
    int free_profiles = 0;
    if (profiles == NULL){
        profiles = Game_StrategyProfiles_create(game);
        free_profiles = 1;
    }
    
    payoffcache_t *cache = malloc(sizeof(payoffcache_t));
    assert(cache != NULL);
    
    cache->count = profiles->count;
    cache->payoff_cache = malloc(cache->count * sizeof(double *));
    assert(cache->payoff_cache != NULL);
    
    for (i = 0; i < profiles->count; i++){ 
        assert(*(cache->payoff_cache + i) != NULL);
        
        *(cache->payoff_cache + i) = game->payoffs(profiles->size, *(profiles->profiles + i)); 
    }
    
    if (free_profiles){
        StrategyProfiles_destroy(profiles);
    }
    
    return cache;
}

void
PayoffCache_destroy(payoffcache_t *cache){
    int i;
    if (cache != NULL){
        for (i = 0; i < cache->count; i++){
            free(*(cache->payoff_cache + i));
        }
        free(cache->payoff_cache);
        free(cache);
    }
}

popcollection_t *
replicator_dynamics(game_t *game, popcollection_t *start_pops, double alpha, double effective_zero, int max_generations, cb_func on_generation)
{
    int free_start = 0;
    assert(game != NULL);
    
    if (start_pops == NULL){
        start_pops = Game_PopCollection_create(game);
        PopCollection_randomize(start_pops);
        free_start = 1;
    }
    
    int generation = 0;
    
    popcollection_t * curr_pops = PopCollection_clone(start_pops);
    popcollection_t * next_pops = PopCollection_clone(start_pops);
    popcollection_t * end_pops = PopCollection_clone(start_pops);
    strategyprofiles_t *profiles = Game_StrategyProfiles_create(game);
    
    payoffcache_t *payoff_cache = PayoffCache_create(game, profiles);
    assert(payoff_cache != NULL);
    
    int i;
    
    PopCollection_copy(next_pops, start_pops);
    
    do {
        generation++;
        PopCollection_copy(curr_pops, next_pops);
        
        if (next_pops->size == 1){
            update_population_proportions(alpha, 0, *(next_pops->populations), curr_pops, profiles, payoff_cache);
        }
        else {
            for (i = 0; i < next_pops->size; i++){
                update_population_proportions(alpha, i, *(next_pops->populations + i), curr_pops, profiles, payoff_cache);
            } 
        }
        
        if (on_generation != NULL){
            on_generation(game, generation, next_pops);
        }
    } while((max_generations == 0 || generation < max_generations) && !PopCollection_equal(curr_pops, next_pops, effective_zero));
    
    PopCollection_copy(end_pops, next_pops);
    
    PopCollection_destroy(curr_pops);
    PopCollection_destroy(next_pops);
    StrategyProfiles_destroy(profiles);
    PayoffCache_destroy(payoff_cache);
    
    if (free_start){
        PopCollection_destroy(start_pops);
    }
    
    return end_pops;
}

double
earned_payoff(int player, int strategy, popcollection_t *pops, strategyprofiles_t *profiles, payoffcache_t *payoff_cache)
{
    assert(pops != NULL);
    assert(profiles != NULL);
    assert(payoff_cache != NULL);
    
    double payoff = 0;
    int num_profiles = profiles->count / *(profiles->types + player);
    int profile_index;
    int profile_number;
    double *profile_payoffs;
    int *profile;
    double profile_prob;
    int pl_i;
    for (profile_index = 0; profile_index < num_profiles; profile_index++){
        profile_prob = 1;
        profile_number = *(*(*(profiles->player_strategy_profiles + player) + strategy) + profile_index);
        profile = *(profiles->profiles + profile_number);
        profile_payoffs = *(payoff_cache->payoff_cache + profile_number);
        
        for (pl_i = 0; pl_i < profiles->size; pl_i++){
            if (pl_i != player){
                if (pops->size == 1){
                    profile_prob = *((*(pops->populations))->proportions + *(profile + pl_i)) * profile_prob;
                }
                else {
                    profile_prob = *((*(pops->populations + pl_i))->proportions + *(profile + pl_i)) * profile_prob;
                }
            }
        }
        
        payoff = payoff +  (*(profile_payoffs + player)) * profile_prob;
    }
    
    return payoff;
}

double
average_earned_payoff(int player, popcollection_t *pops, strategyprofiles_t *profiles, payoffcache_t *payoff_cache)
{
    assert(pops != NULL);
    assert(profiles != NULL);
    assert(payoff_cache != NULL);

    int strategy;
    double avg_payoff = 0;
    for (strategy = 0; strategy < *(pops->pop_sizes + player); strategy++){
        if (pops->size == 1){
            avg_payoff = avg_payoff + *((*(pops->populations))->proportions + strategy) * earned_payoff(player, strategy, pops, profiles, payoff_cache);
        }
        else {
            avg_payoff = avg_payoff + *((*(pops->populations + player))->proportions + strategy) * earned_payoff(player, strategy, pops, profiles, payoff_cache);
        }
    }
    
    return avg_payoff;
}

void 
update_population_proportions(double alpha, int player, population_t *pop, popcollection_t *curr_pops, strategyprofiles_t *profiles, payoffcache_t *payoff_cache)
{
    assert(pop != NULL);
    assert(curr_pops != NULL);
    assert(profiles != NULL);
    assert(payoff_cache != NULL);
    
    int offset;
    
    if (curr_pops->size == 1){
        offset = 0;
    }
    else {
        offset = player;
    }

    int strategy;
    for (strategy = 0; strategy < pop->size; strategy++){
        if (*((*(curr_pops->populations + offset))->proportions + strategy) != 0){
            *(pop->proportions + strategy) = (*((*(curr_pops->populations + offset))->proportions + strategy)) * (alpha + earned_payoff(player, strategy, curr_pops, profiles, payoff_cache)) / (alpha + average_earned_payoff(player, curr_pops, profiles, payoff_cache));
        }
        else {
            *(pop->proportions + strategy) = 0;
        }
    }
}
