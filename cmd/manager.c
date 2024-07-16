//
// Created by millefeuille on 4/26/23.
//

#include "ft_malloc.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
memory_manager ft_malloc_manager = (memory_manager){0};

__attribute__((constructor))
void construct_ft_malloc_manager(void) {
	if (ft_malloc_manager.tiny_size)
		return;
	ft_malloc_manager.tiny_size = sysconf(_SC_PAGESIZE) * sizeof(chunk_header) / 8;
	ft_malloc_manager.tiny_chunk_size = (ft_malloc_manager.tiny_size / 128);
	ft_malloc_manager.tiny_max_size = (ft_malloc_manager.tiny_chunk_size - sizeof(chunk_header));
	ft_malloc_manager.small_size = sysconf(_SC_PAGESIZE) * (1024 * (sizeof(chunk_header) / 8));
	ft_malloc_manager.small_chunk_size = (ft_malloc_manager.small_size / 128);
	ft_malloc_manager.small_max_size = (ft_malloc_manager.small_chunk_size - sizeof(chunk_header));
#ifdef MALLOC_THREADSAFE
	pthread_mutex_init(&ft_malloc_manager.lock, NULL);
	if (errno) {
		ft_fputstr("malloc: couldn't init mutex\n", 2);
		exit(1);
	}
#endif
}

__attribute__((destructor))
void destroy_ft_malloc_manager(void) {
#ifdef MALLOC_THREADSAFE
	pthread_mutex_destroy(&ft_malloc_manager.lock);
	if (errno) {
		ft_fputstr("malloc: couldn't destroy mutex\n", 2);
		exit(1);
	}
#endif
}
