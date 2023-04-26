//
// Created by millefeuille on 4/25/23.
//

#include "ft_malloc.h"

void *allocate(void *addr, size_t size) {
	return mmap(addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

chunk_ptr append_to_list(chunk_ptr start, chunk_ptr chunk) {
	if (!chunk)
		return start;
	if (!start)
		return chunk;
	chunk_ptr current = chunk;
	for (; current->next; current = current->next);
	current->next = chunk;
	return start;
}

void *malloc_in_zone(size_t size, size_t real_size) {
	if (size > manager.small_max_size) {
		chunk_ptr ret = allocate(NULL, size);
		if (!ret)
			return NULL;
		ret->next = NULL;
		ret->_size = size;
		ret->_size_add = real_size;
		set_chunk_busy(ret);
		manager.large_allocs = append_to_list(manager.large_allocs, ret);
		return ret;
	}

	zone_ptr *zone = &manager.tiny_zones;
	if (size > manager.tiny_max_size)
		zone = &manager.small_zones;

	zone_ptr current = *zone;
	for (; current; current = current->next) {
		chunk_ptr ret = zone_malloc(current, size, real_size);
		if (ret) {
			*zone = append_to_list(*zone, ret);
			return head_to_mem(ret);
		}
	}

	*zone = new_zone(size > manager.tiny_max_size);
	if (!*zone)
		return NULL;
	chunk_ptr chunk = zone_malloc(*zone, size, real_size);
	if (!chunk)
		return NULL;
	return head_to_mem(chunk);
}

size_t compute_aligned_size(size_t size) {
	return size + (8 - (size % 8));
}

void *malloc(size_t size) {
	init_manager();
	if (size <= 0)
		return NULL;
	size_t aligned_size = compute_aligned_size(size);
	return malloc_in_zone(aligned_size, size);
}
