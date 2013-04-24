#include <assert.h>
#include <stdlib.h>
#include "simulations/urnlearning_urns.h"
#include "simulations/randomkit.h"

urn_t * 
Urn_create(unsigned int types, double *initial_counts)
{
    urn_t * urn = malloc(sizeof(urn_t));
    assert(urn != NULL);

    unsigned int i;

    urn->types = types;
    urn->counts = malloc(urn->types * sizeof(double));
    assert(urn->counts != NULL);
    urn->proportions = malloc(urn->types * sizeof(double));
    assert(urn->proportions != NULL);
    
    for (i = 0; i < urn->types; i++){
        *(urn->counts + i) = 0.0;
    }
    
    if (initial_counts == NULL){
        double *initial_counts2 = malloc(urn->types * sizeof(double));
        for (i = 0; i < urn->types; i++){
            *(initial_counts2 + i) = 0.0;
        }
        Urn_update(urn, initial_counts2);
        free(initial_counts2);
    }
    else {
        Urn_update(urn, initial_counts);
    }
    
    return urn;
}

void 
Urn_destroy(urn_t * urn)
{
    if (urn != NULL){
        free(urn->counts);
        free(urn->proportions);
        free(urn);
    }
}

void 
Urn_update(urn_t *urn, double *count_updates)
{
    assert(urn != NULL);
    
    unsigned int i;
    double total = 0;
    for (i = 0; i < urn->types; i++){
        *(urn->counts + i) += *(count_updates + i);
        total = *(urn->counts + i);
    }
    
    if (total > 0){
        for (i = 0; i < urn->types; i++){
            *(urn->proportions + i) = *(urn->counts + i) / total;
        }
    }
    else {
        for (i = 0; i < urn->types; i++){
            *(urn->proportions + i) = 0.0;
        }
    }
}

unsigned int 
Urn_select(urn_t *urn, rk_state rand_state)
{
    assert(urn != NULL);
    
    double random_draw = (double)rk_double(&rand_state);
    
    unsigned int i;
    double cumulative = 0.0;
    for (i = 0; i < urn->types; i++){
        cumulative += *(urn->proportions + i);
        if (cumulative >= random_draw){
            return i;
        }
    }
    
    return i;
}

urncollection_t * 
UrnCollection_create(unsigned int num_urns, unsigned int * types, double **initial_counts)
{
    assert(types != NULL);

    urncollection_t * urnc = malloc(sizeof(urncollection_t));
    assert(urnc != NULL);
    
    urnc->num_urns = num_urns;
    urnc->urns = malloc(num_urns * sizeof(urn_t *));
    assert(urnc->urns != NULL);
    
    unsigned int i;
    for (i = 0; i < urnc->num_urns; i++){
        *(urnc->urns + i) = Urn_create(*(types + i), (initial_counts == NULL) ? NULL : *(initial_counts + i));
        assert(*(urnc->urns + i) != NULL);
    }
    
    return urnc;
}

void 
UrnCollection_destroy(urncollection_t *urnc)
{
    unsigned int i;
    if (urnc != NULL){
        for (i = 0; i < urnc->num_urns; i++){
            Urn_destroy(*(urnc->urns + i));
        }
        free(urnc->urns);
        free(urnc);
    }
}
