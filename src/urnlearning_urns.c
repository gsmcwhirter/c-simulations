#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "simulations/urnlearning_urns.h"
#include "simulations/randomkit.h"

urn_t * 
Urn_create(unsigned int types, double *initial_counts)
{
    urn_t * urn = malloc(sizeof(urn_t));
    assert(urn != NULL);
    if (urn == NULL){
        exit(EXIT_FAILURE);
    }

    unsigned int i;

    urn->types = types;
    urn->counts = malloc(urn->types * sizeof(double));
    assert(urn->counts != NULL);
    if (urn->counts == NULL){
        exit(EXIT_FAILURE);
    }
    urn->proportions = malloc(urn->types * sizeof(double));
    assert(urn->proportions != NULL);
    if (urn->proportions == NULL){
        exit(EXIT_FAILURE);
    }
    
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
    if (urn == NULL){
        exit(EXIT_FAILURE);
    }
    
    #ifndef NDEBUG
    char *prefix = "    ";
    printf("Updating Urn:\n");
    printf("  Starting Values:\n");
    Urn_display(urn, prefix);
    #endif
    
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
    
    #ifndef NDEBUG
    printf("  Ending Values:\n");
    Urn_display(urn, prefix);
    #endif
}

unsigned int 
Urn_select(urn_t *urn, rk_state rand_state)
{
    assert(urn != NULL);
    if (urn == NULL){
        exit(EXIT_FAILURE);
    }
    
    double random_draw = rk_double(&rand_state);
    
    int draw_failed = 1;
    unsigned int draw_value;
    unsigned int i;
    double cumulative = 0.0;
    for (i = 0; i < urn->types; i++){
        cumulative += *(urn->proportions + i);
        if (cumulative >= random_draw){
            draw_value = i;
            draw_failed = 0;
            break;
        }
    }
    
    if (draw_failed){
        draw_value = urn->types;
    }
    
    #ifndef NDEBUG
    char *prefix = "    ";
    printf("Urn Selection:\n");
    printf("  Urn Data:\n");
    Urn_display(urn, prefix);
    printf("  Random Draw: %g\n", random_draw);
    printf("  Draw Value: %i\n", draw_value);
    #endif
    
    return draw_value;
}

void 
Urn_display(urn_t * urn, char *prefix)
{
    assert(urn != NULL);
    if (urn == NULL){
        exit(EXIT_FAILURE);
    }
    
    unsigned int i;
    printf("%sCounts:", prefix);
    for (i = 0; i < urn->types; i++){
        printf("  %g", *(urn->counts + i));
    }
    printf("\n");
    printf("%sProportions:", prefix);
    for (i = 0; i < urn->types; i++){
        printf("  %g", *(urn->proportions + i));
    }
    printf("\n");
}

urncollection_t * 
UrnCollection_create(unsigned int num_urns, unsigned int * types, double **initial_counts)
{
    assert(types != NULL);
    if (types == NULL){
        exit(EXIT_FAILURE);
    }

    urncollection_t * urnc = malloc(sizeof(urncollection_t));
    assert(urnc != NULL);
    if (urnc == NULL){
        exit(EXIT_FAILURE);
    }
    
    urnc->num_urns = num_urns;
    urnc->urns = malloc(num_urns * sizeof(urn_t *));
    assert(urnc->urns != NULL);
    if (urnc->urns == NULL){
        exit(EXIT_FAILURE);
    }
    
    unsigned int i;
    for (i = 0; i < urnc->num_urns; i++){
        *(urnc->urns + i) = Urn_create(*(types + i), (initial_counts == NULL) ? NULL : *(initial_counts + i));
        assert(*(urnc->urns + i) != NULL);
        if (*(urnc->urns + i) == NULL){
            exit(EXIT_FAILURE);
        }
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
