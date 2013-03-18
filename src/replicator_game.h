#ifndef REPLICATOR_SIM_GAME_H
#define REPLICATOR_SIM_GAME_H

typedef double *(*payoff_function)(int players, int *strategy_profile);

struct PopCollection;

struct StrategyProfiles {
    int count;
    int size;
    int *types;
    int **profiles;
    int ***player_strategy_profiles;
};

struct StrategyProfiles * StrategyProfiles_create(int players, int *types);
void StrategyProfiles_destroy(struct StrategyProfiles *sprofs);

struct Game {
    int populations;
    int players;
    int *types;
    payoff_function payoffs;
};

struct Game * Game_create(int players, int populations, int *types, payoff_function payoffs);
void Game_destroy(struct Game *game);
struct StrategyProfiles * Game_StrategyProfiles_create(struct Game *game);

struct PopCollection * Game_PopCollection_create(struct Game *game);

#endif
