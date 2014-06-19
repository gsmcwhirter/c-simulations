.. replicator_simulation

Replicator Simulations
======================

Types
-----

	.. c:type:: cb_func

Functions
---------

	.. c:function:: void replicator_dynamics_setup()

	.. c:function:: popcollection_t * replicator_dynamics(game_t *game, popcollection_t *start_pops, double alpha, double effective_zero, int max_generations, cache_mask caching, cb_func on_generation, FILE *outfile)

	.. c:function:: double earned_payoff(int player, int strategy, popcollection_t *pops, strategyprofiles_t *profiles, payoffcache_t *payoff_cache)

	.. c:function:: double average_earned_payoff(int player, popcollection_t *pops, strategyprofiles_t *profiles, payoffcache_t *payoff_cache)

	.. c:function:: void update_population_proportions(double alpha, int player, population_t *pop, popcollection_t *curr_pops, strategyprofiles_t *profiles, payoffcache_t *payoff_cache, int *threads)