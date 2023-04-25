//
// Created by millefeuille on 4/25/23.
//

#include "ft_malloc.h"

void *allocate(void *addr, size_t size) {
	if (manager.total_allocations + size >= manager.limit.rlim_cur)
		return NULL;
	return mmap(addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void init(void) {
	manager.tiny_zone_size = getpagesize();
	manager.tiny_zone_max = manager.tiny_zone_size / 128;
	manager.medium_zone_size = getpagesize() * 16;
	manager.medium_zone_max = manager.medium_zone_size / 128;
	manager.total_allocations = 0;
	manager.tiny_zones = NULL;
	manager.medium_zones = NULL;
	getrlimit(RLIMIT_AS, &manager.limit);
}

zone_registry *new_zone(size_t size) {
	zone_registry *ret = allocate(NULL, sizeof(zone_registry));
	if (!ret)
		return NULL;
	*ret = (zone_registry) {
			.entries = NULL,
			.next = NULL,
			.start = allocate(NULL, size),
			.size = size
	};
	if (!ret->start) {
		free(ret);
		return NULL;
	}
	return ret;
}

registry_entry *new_entry(void *start, size_t size) {
	registry_entry *ret = allocate(NULL, sizeof(zone_registry));
	if (!ret)
		return NULL;
	*ret = (registry_entry) {
		.size = size,
		.next = NULL,
		.start = start,
		.free = true
	};
	return ret;
}

char *get_entry_end(registry_entry *entry) {
	return (char *) entry->start + entry->size;
}

void *get_next_free_entry(zone_registry *zones, size_t entry_size, size_t zone_size) {
	size_t smallest_size = entry_size;
	registry_entry *smallest_entry = NULL;

	for (zone_registry *current = zones; current; current = current->next) {
		registry_entry *entries = current->entries;
		for (; entries->next; entries = entries->next) {
			if (entries->free && entries->size >= entry_size) {
				if (entries->size == entry_size)
					return entries;
				if (entries->size < smallest_size) {
					smallest_entry = entries;
					smallest_size = entries->size;
				}
			}
		}
		if (smallest_entry)
			return smallest_entry;
		if (get_entry_end(entries) + entry_size <= (char *) current->start + zone_size){
			entries->next = new_entry(get_entry_end(entries), entry_size);
			return entries->next;
		}
		if (!current->next)
			zones = current;
	}
	zones->next = new_zone(zone_size);
	if (!zones->next)
		return NULL;
	zones->next->entries = new_entry(zones->next->start, entry_size);
	return zones->next->entries;
}

registry_entry *get_next_addr(zone_registry **zones, size_t entry_size, size_t zone_size) {
	if (!zones)
		return NULL;

	if (!*zones) {
		*zones = new_zone(zone_size);
		if (!*zones)
			return NULL;
	}

	if (!(*zones)->entries) {
		(*zones)->entries = new_entry((*zones)->start, entry_size);
		if (!(*zones)->entries)
			return NULL;
		return (*zones)->entries;
	}

	return get_next_free_entry(*zones, entry_size, zone_size);
}

int is_printable(int c) {
	return c >= 32 && c <= 126;
}

void print_memory(void *start, size_t size) {
	char *ptr = start;
	size_t printed_char = 0;
	for (size_t i = 0; i < size; i++) {
		char c = ptr[i];
		if (!c)
			c = '.';
		if (!is_printable(c))
			c = '_';
		write(1, &c, 1);
		printed_char++;
		if (printed_char > 64) {
			write(1, "\n", 1);
			printed_char = 0;
		}
	}
	write(1, "\n", 1);
}

void print_entries(registry_entry *entries) {
	for (registry_entry *current = entries; current; current = current->next) {
		if (current->free)
			continue;
		printf("\t%p-%p : %4.4zu bytes \n", current->start, (void *) ((char *) current->start + current->size), current->size);
		//print_memory(current->start, current->size);
	}
}

void print_zones(zone_registry *zones) {
	for (zone_registry *current = zones; current; current = current->next) {
		printf("%p\n", current->start);
		print_entries(current->entries);
		//print_memory(current->start, size);
	}
}

void show_alloc_mem(void) {
	if (manager.tiny_zones) {
		printf("TINY :\n");
		print_zones(manager.tiny_zones);
	}
	if (manager.medium_zones) {
		printf("\nMEDIUM :\n");
		print_zones(manager.medium_zones);
	}
}

void *malloc(size_t size) {
	if (!manager.tiny_zone_max)
		init();
	if (size <= 0)
		return NULL;
	if (size > manager.medium_zone_max)
		return allocate(NULL, size);

	registry_entry *ret = NULL;
	if (size > manager.tiny_zone_max)
		ret = get_next_addr(&manager.medium_zones, size, manager.medium_zone_size);
	else
		ret = get_next_addr(&manager.tiny_zones, size, manager.tiny_zone_size);
	if (!ret)
		return NULL;
	ret->free = false;
	return ret->start;
}
