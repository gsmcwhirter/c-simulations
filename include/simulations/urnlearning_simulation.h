#ifndef URNLEARNING_SIM_H
#define URNLEARNING_SIM_H

#include "simulations/urnlearning_urns.h"
#include "simulations/urnlearning_game.h"
#include "simulations/randomkit.h"

typedef double ** (*payoff_function)(unsigned int players, unsigned int **types, unsigned int * state_action_profile);

void urnlearning_dynamics(urngame_t *urngame, unsigned int max_iterations, payoff_function payoffs);

#endif
