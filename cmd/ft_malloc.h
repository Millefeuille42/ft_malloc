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
#include <errno.h>

# define false 0
# define true 1

typedef unsigned long size_t;

typedef struct s_registry_entry {
	void *start;
	size_t size;
	int free;
	struct s_registry_entry *next;
} registry_entry;

typedef struct s_zone_registry {
	void *start;
	registry_entry *entries;
	size_t size;
	struct s_zone_registry *next;
} zone_registry;

typedef struct s_memory_manager {
	size_t tiny_zone_max;
	size_t tiny_zone_size;
	size_t medium_zone_max;
	size_t medium_zone_size;
	size_t total_allocations;
	zone_registry *tiny_zones;
	zone_registry *medium_zones;
	struct rlimit limit;
} memory_manager;

extern memory_manager manager;

#endif //FT_LS_FT_MALLOC_H
