//
// Created by millefeuille on 4/27/23.
//

#include "ft_malloc.h"

void *allocate(void *addr, size_t size) {
	return mmap(addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

size_t compute_aligned_size(size_t size) {
	return size + (8 - (size % 8));
}
