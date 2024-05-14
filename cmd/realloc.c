//
// Created by millefeuille on 4/25/23.
//

#include "ft_malloc.h"

void *reallocate_chunk(chunk_ptr chunk, size_t real_size) {
	// TODO this needs a buffer to really de-frag memory
	void *new_ptr = __malloc(real_size);
	if (!new_ptr)
		return NULL;
	ft_memcpy(new_ptr, head_to_mem(chunk), chunk->real_size);
	__free(head_to_mem(chunk));
	return new_ptr;
}

void *reallocate_fitted_chunk(chunk_ptr chunk, size_t real_size, size_t size) {
	chunk_ptr ret = allocate(chunk, size);
	if (!ret || errno == ENOMEM)
		return NULL;
	if (ret == chunk) {
		ret->_size = size;
		ret->real_size = real_size;
		return head_to_mem(ret);
	}

	if (get_chunk_size(chunk) > real_size)
		ft_memcpy(head_to_mem(ret), head_to_mem(chunk), get_chunk_size(chunk));
	else
		ft_memcpy(head_to_mem(ret), head_to_mem(chunk), real_size);

	ret->prev = chunk->prev;
	ret->next = chunk->next;
	if (ret->prev)
		ret->prev->next = ret;
	else
		ft_malloc_manager.large_allocs = ret;
	if (ret->next)
		ret->next->prev = ret;
	ret->_size = size;
	ret->real_size = real_size;
	munmap(chunk, get_chunk_size(chunk) + sizeof(chunk_header));

	return head_to_mem(ret);
}

void *__realloc(void *ptr, size_t size) {
	if (!ptr)
		return __malloc(size);

	size_t real_size = size;
	size = compute_aligned_size(size);
	chunk_ptr chunk = mem_to_head(ptr);
	if (get_chunk_size(chunk) == size)
		return ptr;

	zone_ptr zone = find_zone_of_chunk(chunk);
	if (!zone)
		return reallocate_fitted_chunk(chunk, real_size, size);
	if (get_chunk_size(chunk) > size) {
		if (!chunk->next)
			chunk->_size = size;
		chunk->real_size = real_size;
		return ptr;
	}

	if (!chunk->next) {
		if (size > get_zone_space_left_from_chunk(zone, chunk))
			return reallocate_chunk(chunk, real_size);
		chunk->_size = size;
		chunk->real_size = real_size;
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
		chunk->real_size = real_size;
		return ptr;
	}

	return reallocate_chunk(chunk, real_size);
}

#ifdef MALLOC_THREADSAFE
void *realloc(void *ptr, size_t size) {
	ft_putstr("locking\n");
	pthread_mutex_lock(&ft_malloc_manager.lock);
	void *ret = __realloc(ptr, size);
	ft_putstr("unlocking\n");
	pthread_mutex_unlock(&ft_malloc_manager.lock);
	return ret;
}
#else
void *realloc(void *ptr, size_t size) { return __realloc(ptr, size); }
#endif
