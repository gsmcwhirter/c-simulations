#ifndef REPLICATOR_SIM_H
#define REPLICATOR_SIM_H

#include "replicator_dynamics/replicator_population.h"
#include "replicator_dynamics/replicator_game.h"


struct PayoffCache {
	int count;
	int has_cached_info;
	int free_profiles;
	payoff_function payoffs;
	strategyprofiles_t *profiles;
	double **payoff_cache;
};

typedef struct PayoffCache payoffcache_t;

payoffcache_t * PayoffCache_create(game_t *game, strategyprofiles_t *profiles, int do_cache_info);
double * PayoffCache_getPayoffs(payoffcache_t *cache, int profile_index);
void PayoffCache_destroy(payoffcache_t *cache);

typedef void (*cb_func)(game_t *game, int generation, popcollection_t *generation_pop);

popcollection_t * replicator_dynamics(game_t *game, popcollection_t *start_pops, double alpha, double effective_zero, int max_generations, int use_caching, cb_func on_generation);
double earned_payoff(int player, int strategy, popcollection_t *pops, strategyprofiles_t *profiles, payoffcache_t *payoff_cache);
double average_earned_payoff(int player, popcollection_t *pops, strategyprofiles_t *profiles, payoffcache_t *payoff_cache);
void update_population_proportions(double alpha, int player, population_t *pop, popcollection_t *curr_pops, strategyprofiles_t *profiles, payoffcache_t *payoff_cache);

#endif
