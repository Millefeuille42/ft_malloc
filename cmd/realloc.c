//
// Created by millefeuille on 4/25/23.
//

#include "ft_malloc.h"

void *reallocate_chunk(chunk_ptr chunk, size_t real_size) {
	void *new_ptr = malloc(real_size);
	if (!new_ptr)
		return NULL;
	ft_memcpy(head_to_mem(chunk), new_ptr, chunk->_size_add);
	free(head_to_mem(chunk));
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
		size_t old_size = chunk->_size_add;
		chunk->_size = size;
		chunk->_size_add = real_size;

		void *new_ptr = allocate(ptr, size);
		if (new_ptr == ptr)
			return ptr;

		if (old_size > real_size)
			ft_memcpy(new_ptr, ptr, old_size);
		else
			ft_memcpy(new_ptr, ptr, real_size);

		return new_ptr;
	}

	if (get_chunk_size(chunk) > size) {
		if (!chunk->next)
			chunk->_size = size;
		chunk->_size_add = real_size;
		return ptr;
	}

	if (!chunk->next) {
		if (size > get_zone_space_left_from_chunk(zone, chunk))
			return reallocate_chunk(chunk, real_size);
		chunk->_size = size;
		chunk->_size_add = real_size;
		return ptr;
	}

	if (!is_chunk_free(chunk->next))
		return reallocate_chunk(chunk, real_size);

	size_t free_size = get_chunk_size(chunk);
	chunk_ptr current = chunk;
	for (; free_size < size && current && is_chunk_free(current); current = current->next)
		free_size += get_chunk_size(current) + sizeof(chunk_header);
	if (free_size >= size) {
		chunk->next = NULL;
		chunk->_size = size;
		chunk->_size_add = real_size;
		return ptr;
	}

	return reallocate_chunk(chunk, real_size);
}
