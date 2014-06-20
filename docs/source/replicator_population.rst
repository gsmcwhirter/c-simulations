.. replicator_population

Replicator Populations
======================

Types
-----

	.. c:type:: population_t

		This is a shortcut for a :c:type:`Population` struct.

	.. c:type:: popcollection_t

		This is a shortcut for a :c:type:`PopCollection` struct.

	.. c:type:: struct Population

		This struct holds the data relevant to a single replicator population

		.. c:member:: int Population.size

			This member determines how many entries are in the :c:member:`Population.proportions` array.

		.. c:member:: double* Population.proportions

			This member holds the population proportions.

	.. c:type:: struct PopCollection

		This struct holds a collection of :c:type:`population_t` structs.

		.. c:member:: int PopCollection.size

			This determines how many populations are collected.

		.. c:member:: int* PopCollection.pop_sizes

			This is an array of the sizes of each of the populations collected.

		.. c:member:: popcollection_t** PopCollection.populations

			This is an array of the populations.


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