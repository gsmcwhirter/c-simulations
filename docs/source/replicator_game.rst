.. replicator_game

Replicator Games
================

Constants
---------

	.. c:macro:: CACHE_NONE

	.. c:macro:: CACHE_PROFILES

	.. c:macro:: CACHE_PAYOFFS

	.. c:macro:: CACHE_ALL

Types
-----

	.. c:type:: cache_mask

	.. c:type:: payoff_function

	.. c:type:: strategyprofiles_t

	.. c:type:: game_t

	.. c:type:: payoffcache_t

	.. c:type:: struct StrategyProfiles

		.. c:member:: int StrategyProfiles.count

		.. c:member:: int StrategyProfiles.size

		.. c:member:: int* StrategyProfiles.types

		.. c:member:: int StrategyProfiles.has_cached_info

		.. c:member:: int** StrategyProfiles.profiles

		.. c:member:: int*** StrategyProfiles.player_strategy_profiles

	.. c:type:: struct  Game

		.. c:member:: int Game.populations

		.. c:member:: int Game.players

		.. c:member:: int* Game.types

		.. c:member:: payoff_function Game.payoffs

	.. c:type:: struct PayoffCache

		.. c:member:: int PayoffCache.count

		.. c:member:: int PayoffCache.has_cached_info

		.. c:member:: int PayoffCache.free_profiles

		.. c:member:: payoff_function PayoffCache.payoffs

		.. c:member:: strategyprofiles_t* PayoffCache.profiles

		.. c:member:: double** PayoffCache.payoff_cache

Functions
---------

StrategyProfiles
++++++++++++++++

	.. c:function:: strategyprofiles_t * StrategyProfiles_create(int players, int *types, cache_mask cache)

	.. c:function:: int * StrategyProfiles_getProfile(strategyprofiles_t *sprofs, int num)

	.. c:function:: int * StrategyProfiles_getPlayerProfile(strategyprofiles_t *sprofs, int player, int strategy, int num)

	.. c:function:: int StrategyProfiles_getPlayerProfileNumber(strategyprofiles_t *sprofs, int player, int strategy, int num)

	.. c:function:: void StrategyProfiles_destroy(strategyprofiles_t *sprofs)

Game
++++

	.. c:function:: game_t * Game_create(int players, int populations, int *types, payoff_function payoffs)

	.. c:function:: void Game_destroy(game_t *game)

	.. c:function:: strategyprofiles_t * Game_StrategyProfiles_create(game_t *game, cache_mask cache)

PayoffCache
+++++++++++

	.. c:function:: payoffcache_t * PayoffCache_create(game_t *game, strategyprofiles_t *profiles, cache_mask do_cache)

	.. c:function:: double * PayoffCache_getPayoffs(payoffcache_t *cache, int profile_index)

	.. c:function:: void PayoffCache_destroy(payoffcache_t *cache)