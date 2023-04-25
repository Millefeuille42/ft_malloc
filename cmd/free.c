//
// Created by millefeuille on 4/25/23.
//

#include "ft_malloc.h"

void free_mem(void *ptr) {
	(void) ptr;
}

zone_registry *get_zone_of_address(void *ptr) {
	zone_registry *zone = manager.tiny_zones;
	for (; zone; zone = zone->next)
		if (zone->start >= ptr && zone->start <= (void *)((char *)zone->start + manager.tiny_zone_size))
			return zone;
	zone = manager.medium_zones;
	for (; zone; zone = zone->next)
		if (zone->start >= ptr && zone->start <= (void *)((char *)zone->start + manager.medium_zone_size))
			return zone;
	return NULL;
}

void free(void *ptr) {
	if (!ptr)
		return;
	zone_registry *zone = get_zone_of_address(ptr);
	if (!zone) {
		// unmap
		return;
	}
	for (registry_entry *entry = zone->entries; entry; entry = entry->next) {
		if (entry->start == ptr) {
			entry->free = true;
			if (entry == zone->entries)
				munmap(zone->start, zone->size);
			return;
		}
	}
}
