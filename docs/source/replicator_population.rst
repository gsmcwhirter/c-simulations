.. replicator_population

Replicator Populations
======================

Types
-----

	.. c:type:: population_t

	.. c:type:: popcollection_t

	.. c:type:: struct Population

		.. c:member:: int Population.size

		.. c:member:: double* Population.proportions

	.. c:type:: struct PopCollection

		.. c:member:: int PopCollection.size

		.. c:member:: int* PopCollection.pop_sizes

		.. c:member:: popcollection_t** PopCollection.populations


Functions
---------

Population
++++++++++

	.. c:function:: population_t * Population_create(int size)

	.. c:function:: void Population_destroy(population_t *pop)

	.. c:function:: int Population_equal(population_t *pop1, population_t *pop2, double effective_zero)

	.. c:function:: void Population_copy(population_t *target, population_t *source)

	.. c:function:: void Population_randomize(population_t *pop)

	.. c:function:: void Population_serialize(population_t *pop, FILE * target_file)

	.. c:function:: population_t * Population_deserialize(FILE * source_file)

PopCollection
+++++++++++++

	.. c:function:: popcollection_t * PopCollection_create(int num_pops, int *sizes)

	.. c:function:: popcollection_t * PopCollection_clone(popcollection_t *original)

	.. c:function:: void PopCollection_destroy(popcollection_t *coll)

	.. c:function:: int PopCollection_equal(popcollection_t *coll1, popcollection_t *coll2, double effective_zero)

	.. c:function:: void PopCollection_copy(popcollection_t *target, popcollection_t *source)

	.. c:function:: void PopCollection_randomize(popcollection_t *coll)

	.. c:function:: void PopCollection_serialize(popcollection_t *coll, FILE * target_file)

	.. c:function:: popcollection_t * PopCollection_deserialize(FILE * source_file)