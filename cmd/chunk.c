//
// Created by millefeuille on 4/26/23.
//

# include "ft_malloc.h"

int is_chunk_free(chunk_ptr chunk) {
	return (int) (chunk->_size % 2);
}

void set_chunk_busy(chunk_ptr chunk) {
	if (is_chunk_free(chunk))
		chunk->_size += 1;
}

void set_chunk_free(chunk_ptr chunk) {
	if (!is_chunk_free(chunk))
		chunk->_size -= 1;
}

chunk_ptr get_chunk_end(chunk_ptr chunk) {
	return (chunk_ptr) ((char *) chunk + get_chunk_full_size(chunk));
}

size_t get_chunk_size(chunk_ptr chunk) {
	return is_chunk_free(chunk) ? chunk->_size + 1 : chunk->_size;
}

size_t get_chunk_full_size(chunk_ptr chunk) {
	return get_chunk_size(chunk) + sizeof(chunk_header);
}

void *head_to_mem(chunk_ptr header) {
	return (char *)header + sizeof(chunk_header);
}

chunk_ptr mem_to_head(void *ptr) {
	return (chunk_ptr) ((char *) ptr - sizeof(chunk_header));
}
