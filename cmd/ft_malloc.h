//
// Created by millefeuille on 4/25/23.
//

#ifndef FT_LS_FT_MALLOC_H
# define FT_LS_FT_MALLOC_H

# pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"

# include "malloc.h"
# include <sys/mman.h>
# include <sys/resource.h>
# include <unistd.h>
# include <stdio.h>
# include <errno.h>

typedef unsigned long size_t;

typedef struct s_chunk_header {
	size_t _size;					// Size of chunk / zone. If size is odd, chunk is free / zone is small and real size is += 1
	size_t _size_add;				// Real size of chunk / unused on zone
	struct s_chunk_header *next;	// Pointer to next chunk / zone
	struct s_chunk_header *prev;	// Pointer to prev chunk / zone
} chunk_header;						// Total size 8 - 16 bytes

// Since zone and chunks do not behave the same (zone._size is header + data, chunk._size is data), different names are set for convenience
typedef chunk_header zone_header;

typedef chunk_header* chunk_ptr;
typedef zone_header* zone_ptr;

typedef struct s_memory_manager {
	size_t tiny_size;
	size_t tiny_chunk_size;
	size_t tiny_max_size;
	size_t small_size;
	size_t small_chunk_size;
	size_t small_max_size;
	zone_ptr tiny_zones;
	zone_ptr small_zones;
	chunk_ptr large_allocs;
} memory_manager;

extern memory_manager manager;

void init_manager(void);

void *allocate(void *addr, size_t size);
size_t compute_aligned_size(size_t size);

int is_chunk_free(chunk_ptr chunk);
void set_chunk_busy(chunk_ptr chunk);
void set_chunk_free(chunk_ptr chunk);
chunk_ptr get_chunk_end(chunk_ptr chunk);
size_t get_chunk_size(chunk_ptr chunk);
size_t get_chunk_full_size(chunk_ptr chunk);
void *head_to_mem(chunk_ptr chunk);
chunk_ptr mem_to_head(void *ptr);
zone_ptr find_zone_of_chunk(chunk_ptr chunk);

int is_zone_small(zone_ptr zone);
void set_zone_small(zone_ptr zone);
void set_zone_tiny(zone_ptr zone);
size_t get_zone_chunk_size(zone_ptr zone);
size_t get_zone_max_size(zone_ptr zone);
size_t get_zone_size(zone_ptr zone);
size_t get_zone_space_left_from_chunk(zone_ptr zone, chunk_ptr chunk);
chunk_ptr get_zone_first_chunk(zone_ptr zone);
zone_ptr get_zone_end(zone_ptr zone);
int is_chunk_in_zone(zone_ptr zone, chunk_ptr chunk);
chunk_ptr zone_malloc(zone_ptr zone, size_t size, size_t real_size);
zone_ptr new_zone(int small);

#endif //FT_LS_FT_MALLOC_H
