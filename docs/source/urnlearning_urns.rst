.. urnlearning_urns

Urn-Learning Urns
=================

Types
-----

	.. c:type:: urn_t

	.. c:type:: urncollection_t

	.. c:type:: struct Urn

		.. c:member:: unsigned int Urn.types

		.. c:member:: double* Urn.counts

		.. c:member:: double* Urn.proportions

	.. c:type:: struct UrnCollection

		.. c:member:: unsigned int UrnCollection.num_urns

		.. c:member:: urn_t** UrnCollection.urns

Functions
---------

Urn
+++

	.. c:function:: urn_t * Urn_create(unsigned int types, double *initial_counts)

	.. c:function:: void Urn_destroy(urn_t * urn)

	.. c:function:: void Urn_update(urn_t *urn, double *count_updates)

	.. c:function:: unsigned int Urn_select(urn_t *urn, double random_draw)

	.. c:function:: unsigned int Urn_randomSelect(urn_t *urn, rk_state *rand_state_ptr)

	.. c:function:: void Urn_display(urn_t * urn, char *prefix, FILE *outfile)

	.. c:function:: urn_t * Urn_clone(urn_t *urn)

	.. c:function:: void Urn_copy(urn_t *source, urn_t *target)


UrnCollection
+++++++++++++

	.. c:function:: urncollection_t * UrnCollection_create(unsigned int num_urns, unsigned int * types, double **initial_counts)

	.. c:function:: void UrnCollection_destroy(urncollection_t *urnc)

	.. c:function:: urncollection_t * UrnCollection_clone(urncollection_t *urnc)

	.. c:function:: void UrnCollection_copy(urncollection_t *source, urncollection_t *target)