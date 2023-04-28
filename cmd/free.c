//
// Created by millefeuille on 4/25/23.
//

#include "ft_malloc.h"

void free(void *ptr) {
	if (!ptr)
		return;
	chunk_ptr chunk = mem_to_head(ptr);
	set_chunk_free(chunk);

	if (get_chunk_size(chunk) > manager.small_max_size) {
		if (chunk->prev)
			chunk->prev->next = chunk->next;
		else
			manager.large_allocs = chunk->next;
		if (chunk->next)
			chunk->next->prev = chunk->prev;
		munmap(chunk, get_chunk_size(chunk) + sizeof(chunk_header));
	}

	chunk_ptr prev = NULL;
	chunk_ptr first = NULL;
	for (; chunk && is_chunk_free(chunk) && !chunk->next; chunk = prev) { // Iterate backward while chunk exists and is free
		prev = chunk->prev;
		if (!prev) // If reaching first chunk of a zone, set 'first' to unmap said zone
			first = chunk;
		*chunk = (chunk_header) {.prev = NULL, .next = NULL, ._size = 0, .real_size = 0};
		if (prev)
			prev->next = NULL;
	}

	if (!first)
		return;
	zone_ptr zone = find_zone_of_chunk(first); // If reached first chunk of a zone, get zone of chunk to unmap it
	if (!zone)
		return;
	if (!zone->prev) {
		if (zone == manager.tiny_zones)
			manager.tiny_zones = NULL;
		if (zone == manager.small_zones)
			manager.small_zones = NULL;
	} else
		zone->prev->next = zone->next;
	size_t size = zone->_size;
	*zone = (zone_header) {.prev = NULL, .next = NULL, ._size = 0, .real_size = 0};
	munmap(zone, size);
}
