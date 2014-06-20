.. replicator_game

Replicator Games
================

The replicator_game.c and replicator_game.h files handle
the functionality of defining the game being played for
replicator dynamics simulations.

Constants
---------

	.. c:macro:: CACHE_NONE

		This constant indicates that nothing should be cached. It is a :c:type:`cache_mask`.

	.. c:macro:: CACHE_PROFILES

		This constant indicates that only the strategy profiles should be cached. It is a :c:type:`cache_mask`.

	.. c:macro:: CACHE_PAYOFFS

		This constant indicates that only the payoffs should be cached. It is a :c:type:`cache_mask`.

	.. c:macro:: CACHE_ALL

		This constant indicates that both the profiles and payoffs should be cached.
		It is equivalent to :c:macro:`CACHE_PROFILES` | :c:macro:`CACHE_PAYOFFS`. It is a :c:type:`cache_mask`.

Types
-----

	.. c:type:: cache_mask

		This type is an unsigned int. It is specially named to indicate that one of :c:macro:`CACHE_NONE`, :c:macro:`CACHE_PROFILES`, :c:macro:`CACHE_PAYOFFS`, or :c:macro:`CACHE_ALL` should be used.

	.. c:type:: double *(*payoff_function)(int players, int *strategy_profile)

		This type defines the signature for a payoff function that takes a strategy profile and returns an array of payoff values.

	.. c:type:: strategyprofiles_t

		This is an alias for the :c:type:`StrategyProfiles` struct.

	.. c:type:: game_t

		This is an alias for the :c:type:`Game` struct.

	.. c:type:: payoffcache_t

		This is an alias for the :c:type:`PayoffCache` struct.

	.. c:type:: struct StrategyProfiles

		This struct holds the information for strategy profiles (tuples of interaction possibility)

		.. c:member:: int StrategyProfiles.count

			This is the number of profiles the struct holds.

		.. c:member:: int StrategyProfiles.size

			This is the size of each profile.

		.. c:member:: int* StrategyProfiles.types

			This is an array of types (the number of types for player i is in the ith entry). The :c:member:`~StrategyProfiles.size` member indicates the size of this array.

		.. c:member:: int StrategyProfiles.has_cached_info

			This is a flag to indicate whether the struct has cached information stored. It is used for the :c:func:`StrategyProfiles_destroy` function.

		.. c:member:: int** StrategyProfiles.profiles

			This is an array of the possible strategy profiles. It has size stored in :c:member:`~StrategyProfiles.count` and each element has size from the :c:member:`~StrategyProfiles.size` member.

		.. c:member:: int*** StrategyProfiles.player_strategy_profiles

			This is an array of the possible strategy profiles sorted by players participating in them. It has the size defined by the :c:member:`~StrategyProfiles.size` member. The first dimension corresponds to the player. The second dimension corresponds to the number of strategies for that player (size :c:member:`~StrategyProfiles.count`/:c:member:`~StrategyProfiles.types`\[i\]). The third dimension is a list of the profiles in which that strategy participates, represented by indices referring to the :c:member:`StrategyProfiles.profiles` list. 

	.. c:type:: struct  Game

			This struct holds data about the game being played under the dynamics.

		.. c:member:: int Game.populations

			This is how many populations the game has.

		.. c:member:: int Game.players

			This is how many players there are in the game.

		.. c:member:: int* Game.types

			This is a list, for each player, how many strategies that player has.

		.. c:member:: payoff_function Game.payoffs

			This is the function that returns a payoff vector for a certain strategy profile in the game.

	.. c:type:: struct PayoffCache

		This is a struct that holds a cache of pre-calculated payoff vectors.

		.. c:member:: int PayoffCache.count

			This is how many items are in the cache.

		.. c:member:: int PayoffCache.has_cached_info

			This is a flag to indicate that the cache has information in it that should be freed.

		.. c:member:: int PayoffCache.free_profiles

			This is a flag to indicate that the profiles are cached and should be freed.

		.. c:member:: payoff_function PayoffCache.payoffs

			This is the payoff function that generates the payoffs.

		.. c:member:: strategyprofiles_t* PayoffCache.profiles

			This is the pointer to the cache of strategy profiles.

		.. c:member:: double** PayoffCache.payoff_cache

			This is the cache of payoff vectors. Each payoff vector is an array of doubles, and the collection is an array of those arrays.

Functions
---------

StrategyProfiles
++++++++++++++++

	.. c:function:: strategyprofiles_t * StrategyProfiles_create(int players, int *types, cache_mask cache)

		This creates a :c:type:`strategyprofiles_t` struct for the requisite number of :c:data:`players`.

		The :c:data:`types` parameter is a list of number of strategies that each player has.

		The :c:data:`cache` parameter is how much of the profiles and payoffs to cache.

	.. c:function:: int * StrategyProfiles_getProfile(strategyprofiles_t *sprofs, int num)

		This returns the strategy profile corresponding to the :c:data:`num`th entry in the :c:data:`sprofs` array.

	.. c:function:: int * StrategyProfiles_getPlayerProfile(strategyprofiles_t *sprofs, int player, int strategy, int num)

		This returns the :c:data:`num`th strategy profile that player :c:data:`player`'s strategy :c:data:`strategy` is involved in.

	.. c:function:: int StrategyProfiles_getPlayerProfileNumber(strategyprofiles_t *sprofs, int player, int strategy, int num)

		This returns the index in the profile list of :c:data:`sprofs` that the :c:data:`num`th strategy profile of player :c:data:`player`'s strategy :c:data:`strategy` is involved in.

	.. c:function:: void StrategyProfiles_destroy(strategyprofiles_t *sprofs)

		This frees all data associated with :c:data:`sprofs`.

Game
++++

	.. c:function:: game_t * Game_create(int players, int populations, int *types, payoff_function payoffs)

		This creates a :c:type:`game_t` struct based on the requested data.

		The number of populations must either be 1 or equal to the number of players.

		The parameter :c:data:`types` is a list of the number of strategies for each player.

		The parameter :c:data:`payoffs` is the payoff function for the game.

	.. c:function:: void Game_destroy(game_t *game)

		This frees all data associated with :c:data:`game`.

	.. c:function:: strategyprofiles_t * Game_StrategyProfiles_create(game_t *game, cache_mask cache)

		This creates a :c:type:`strategyprofiles_t` struct from the data already present in a :c:type:`game_t` struct.

PayoffCache
+++++++++++

	.. c:function:: payoffcache_t * PayoffCache_create(game_t *game, strategyprofiles_t *profiles, cache_mask do_cache)

		This creates a :c:type:`payoffcache_t` struct based on the provided information.

	.. c:function:: double * PayoffCache_getPayoffs(payoffcache_t *cache, int profile_index)

		This returns the payoffs for the cached profile index :c:data:`profile_index`.

	.. c:function:: void PayoffCache_destroy(payoffcache_t *cache)

		This frees all data associated with :c:data:`cache`.