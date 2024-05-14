//
// Created by millefeuille on 4/25/23.
//

#include "ft_malloc.h"

void __free(void *ptr) {
	if (!ptr)
		return;
	chunk_ptr chunk = mem_to_head(ptr);
	set_chunk_free(chunk);
	if (chunk->real_size > get_chunk_size(chunk) || chunk->_size == 0 || chunk->real_size == 0)
		return;

	if (get_chunk_size(chunk) > ft_malloc_manager.small_max_size) {
		if (chunk->prev)
			chunk->prev->next = chunk->next;
		else
			ft_malloc_manager.large_allocs = chunk->next;
		if (chunk->next)
			chunk->next->prev = chunk->prev;
		munmap(chunk, get_chunk_size(chunk) + sizeof(chunk_header));
		return;
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
		if (zone == ft_malloc_manager.tiny_zones)
			ft_malloc_manager.tiny_zones = NULL;
		if (zone == ft_malloc_manager.small_zones)
			ft_malloc_manager.small_zones = NULL;
	} else
		zone->prev->next = zone->next;
	size_t size = zone->_size;
	*zone = (zone_header) {.prev = NULL, .next = NULL, ._size = 0, .real_size = 0};
	munmap(zone, size);
}

#ifdef MALLOC_THREADSAFE
void free(void *ptr) {
	ft_putstr("locking\n");
	pthread_mutex_lock(&ft_malloc_manager.lock);
	__free(ptr);
	ft_putstr("unlocking\n");
	pthread_mutex_unlock(&ft_malloc_manager.lock);
}
#else
void free(void *ptr) { __free(ptr); }
#endif
