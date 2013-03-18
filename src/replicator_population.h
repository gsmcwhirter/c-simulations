#ifndef REPLICATOR_SIM_POP_H
#define REPLICATOR_SIM_POP_H

struct Population {
    int size;
    double *proportions;
};

struct Population * Population_create(int size);
void Population_destroy(struct Population *pop);
int Population_equal(struct Population *pop1, struct Population *pop2, double effective_zero);
void Population_copy(struct Population *target, struct Population *source);
void Population_randomize(struct Population *pop);

struct PopCollection {
    int size;
    int *pop_sizes;
    struct Population **populations;
};

struct PopCollection * PopCollection_create(int num_pops, int *sizes);
struct PopCollection * PopCollection_clone(struct PopCollection *original);
void PopCollection_destroy(struct PopCollection *coll);
int PopCollection_equal(struct PopCollection *coll1, struct PopCollection *coll2, double effective_zero);
void PopCollection_copy(struct PopCollection *target, struct PopCollection *source);
void PopCollection_randomize(struct PopCollection *coll);

#endif
