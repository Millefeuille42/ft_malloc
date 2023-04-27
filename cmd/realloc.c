//
// Created by millefeuille on 4/25/23.
//

#include "ft_malloc.h"

void *reallocate_chunk(void *ptr, size_t real_size) {
	void *new_ptr = malloc(real_size);
	if (!new_ptr)
		return NULL;
	// TODO memcopy(ptr, new_ptr);
	free(ptr);
	return new_ptr;
}

void *realloc(void *ptr, size_t size) {
	if (!ptr)
		return malloc(size);

	size_t real_size = size;
	size = compute_aligned_size(size);
	chunk_ptr chunk = mem_to_head(ptr);
	if (get_chunk_size(chunk) == size)
		return ptr;

	zone_ptr zone = find_zone_of_chunk(chunk);
	if (!zone) {
		// TODO To be done later
		return NULL; // WRONG
	}
	if (get_chunk_size(chunk) > size) {
		if (!chunk->next)
			chunk->_size = size;
		chunk->_size_add = real_size;
		return ptr;
	}
	if (!chunk->next) {
		if (size > get_zone_space_left_from_chunk(zone, chunk))
			return reallocate_chunk(ptr, real_size);
		chunk->_size = size;
		chunk->_size_add = real_size;
		set_chunk_busy(chunk);
		return chunk;
	}
	if (!is_chunk_free(chunk->next))
		return reallocate_chunk(ptr, real_size);

	size_t free_size = get_chunk_size(chunk);
	chunk_ptr current = chunk;
	for (; current && is_chunk_free(current) && free_size < size; current = current->next)
		free_size += get_chunk_size(current) + sizeof(chunk_header);
	if (free_size >= size) {
		// TODO Join and consume next free chunks
		return NULL; // WRONG
	}
	return reallocate_chunk(ptr, real_size);
}
