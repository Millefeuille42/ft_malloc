//
// Created by millefeuille on 4/25/23.
//

#include "ft_malloc.h"

zone_ptr find_zone_of_chunk(chunk_ptr chunk) {
	zone_ptr zone = manager.tiny_zones;
	if (get_chunk_size(chunk) > manager.tiny_max_size)
		zone = manager.small_zones;
	for (; zone && !is_chunk_in_zone(zone, chunk); zone = zone->next);
	return zone;
}

void free(void *ptr) {
	if (!ptr)
		return;
	chunk_ptr chunk = mem_to_head(ptr);
	set_chunk_free(chunk);

	chunk_ptr prev = NULL;
	chunk_ptr first = NULL;
	for (; chunk && is_chunk_free(chunk) && !chunk->next; chunk = prev) { // Iterate backward while chunk exists and is free
		prev = chunk->prev;
		size_t size = get_chunk_size(chunk);
		if (!prev && size <= manager.small_max_size) // If reaching first chunk of a zone, set 'first' to unmap said zone
			first = chunk;
		*chunk = (chunk_header) {.prev = NULL, .next = NULL, ._size = 0, ._size_add = 0};
		if (size > manager.small_max_size) // If it is a fitted chunk, unmap it
			munmap(chunk, size);
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
	*zone = (zone_header) {.prev = NULL, .next = NULL, ._size = 0, ._size_add = 0};
	munmap(zone, size);
}
