#ifndef URNLEARNING_URNS_H
#define URNLEARNING_URNS_H

#include "simulations/randomkit.h"

struct Urn {
	unsigned int types;
	double *counts;
	double *proportions;
};

typedef struct Urn urn_t;

urn_t * Urn_create(unsigned int types, double *initial_counts);
void Urn_destroy(urn_t * urn);
void Urn_update(urn_t *urn, double *count_updates);
unsigned int Urn_select(urn_t *urn, rk_state rand_state);
void Urn_display(urn_t * urn, char *prefix);

struct UrnCollection {
	unsigned int num_urns;
	urn_t **urns;
};

typedef struct UrnCollection urncollection_t;

urncollection_t * UrnCollection_create(unsigned int num_urns, unsigned int * types, double **initial_counts);
void UrnCollection_destroy(urncollection_t *urnc);

#endif
