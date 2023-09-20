//
// Created by millefeuille on 4/26/23.
//

#include "ft_malloc.h"

memory_manager manager = (memory_manager){0};

__attribute__((constructor))
void construct_manager(void) {
	if (manager.tiny_size)
		return;
	manager.tiny_size = getpagesize() * sizeof(chunk_header) / 8;
	manager.tiny_chunk_size = (manager.tiny_size / 128);
	manager.tiny_max_size = (manager.tiny_chunk_size - sizeof(chunk_header));
	manager.small_size = getpagesize() * (1024 * (sizeof(chunk_header) / 8));
	manager.small_chunk_size = (manager.small_size / 128);
	manager.small_max_size = (manager.small_chunk_size - sizeof(chunk_header));
#ifdef MALLOC_THREADSAFE
	pthread_mutex_init(&manager.lock, NULL);
	if (errno) {
		ft_fputstr("malloc: couldn't init mutex\n", 2);
		exit(1);
	}
#endif
}

__attribute__((destructor))
void destroy_manager(void) {
#ifdef MALLOC_THREADSAFE
	pthread_mutex_destroy(&manager.lock);
	if (errno) {
		ft_fputstr("malloc: couldn't destroy mutex\n", 2);
		exit(1);
	}
#endif
}
