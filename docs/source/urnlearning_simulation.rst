.. urnlearning_simulation

Urn-Learning Simulations
========================

Types
-----

	.. c:type:: double ** (*payoff_function)(unsigned int players, unsigned int **types, unsigned int * state_action_profile)

Functions
---------

	.. c:function:: void urnlearning_dynamics(urngame_t *urngame, unsigned long max_iterations, payoff_function payoffs)