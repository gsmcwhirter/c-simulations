.. urnlearning_game

Urn-Learning Games
==================

Types
-----

	.. c:type:: unsigned int * (*urn_interaction)(unsigned int players, urncollection_t **player_urns, rk_state *random_state)

	.. c:type:: urngame_t

	.. c:type:: struct UrnGame

		.. c:member:: unsigned int Urngame.num_players

		.. c:member:: unsigned int** Urngame.types

		.. c:member:: urncollection_t** Urngame.player_urns

		.. c:member:: urn_interaction Urngame.interaction_function

Functions
---------

	.. c:function:: urngame_t * UrnGame_create(unsigned int players, unsigned int *num_urns, unsigned int **types, double ***initial_counts, urn_interaction func)

	.. c:function:: void UrnGame_destroy(urngame_t *urngame)

	.. c:function:: unsigned int * default_urnlearning_interaction(unsigned int players, urncollection_t **player_urns, rk_state *rand_state_ptr)

	.. c:function:: void UrnGame_copy(urngame_t *source, urngame_t *target)

	.. c:function:: urngame_t * UrnGame_clone(urngame_t *urngame)