//
// Created by millefeuille on 4/26/23.
//

#include "ft_malloc.h"

int is_zone_small(zone_ptr zone) {
	return is_chunk_free(zone);
}

void set_zone_small(zone_ptr zone) {
	set_chunk_free(zone);
}

void set_zone_tiny(zone_ptr zone) {
	set_chunk_busy(zone);
}

size_t get_zone_chunk_size(zone_ptr zone) {
	return is_zone_small(zone) ? manager.small_chunk_size : manager.tiny_chunk_size;
}

size_t get_zone_max_size(zone_ptr zone) {
	return is_zone_small(zone) ? manager.small_max_size : manager.tiny_max_size;
}

size_t get_zone_size(zone_ptr zone) {
	return get_chunk_size(zone);
}

chunk_ptr get_zone_first_chunk(zone_ptr zone) {
	return head_to_mem(zone);
}

zone_ptr get_zone_end(zone_ptr zone) {
	return (zone_ptr) ((char *) zone + get_zone_size(zone));
}

int is_chunk_in_zone(zone_ptr zone, chunk_ptr chunk) {
	return chunk >= get_zone_first_chunk(zone) && chunk <= get_zone_end(zone);
}

size_t get_zone_space_left_from_chunk(zone_ptr zone, chunk_ptr chunk) {
	size_t end_offset = (char *) get_chunk_end(chunk) - (char *) zone;
	return get_zone_size(zone) - end_offset;
}

chunk_ptr zone_malloc(zone_ptr zone, size_t size, size_t real_size) {
	if (size > zone->_size || size > get_zone_max_size(zone))
		return NULL;
	chunk_ptr current = get_zone_first_chunk(zone);
	if (!current->_size) {
		current->next = NULL;
		current->prev = NULL;
		current->_size = size;
		current->real_size = real_size;
		set_chunk_busy(current);
		return current;
	}

	for (; current->next; current = current->next) {
		if (is_chunk_free(current) && get_chunk_size(current) >= size) {
			set_chunk_busy(current);
			return current;
		}
	}
	if (sizeof(chunk_header) + size > get_zone_space_left_from_chunk(zone, current))
		return NULL;

	current->next = get_chunk_end(current);
	current->next->next = NULL;
	current->next->prev = current;
	current->next->_size = size;
	current->next->real_size = real_size;
	set_chunk_busy(current->next);

	return current->next;
}

zone_ptr new_zone(int small) {
	size_t size = manager.tiny_size;
	if (small)
		size = manager.small_size;
	zone_ptr ret = allocate(NULL, size);
	if (!ret || errno == ENOMEM)
		return NULL;
	ret->next = NULL;
	ret->_size = size;
	small ? set_zone_small(ret) : set_zone_tiny(ret);
	return ret;
}
