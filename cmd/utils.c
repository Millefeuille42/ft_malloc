//
// Created by millefeuille on 4/27/23.
//

#include "ft_malloc.h"

void *allocate(void *addr, size_t size) {
	return mmap(addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

size_t compute_aligned_size(size_t size) {
	// Correction subjects needs us to be aligned on 16 bytes, not sure of why...
	//  using sizeof(chunk_header) makes sure that this value is halved on 32bits systems
	return size + (sizeof(chunk_header) - (size % sizeof(chunk_header)));
}

void ft_memcpy (char *dest, const char *src, size_t size){
	for (size_t i = 0; i < size; i++)
		dest[i] = src[i];
}
