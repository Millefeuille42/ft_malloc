//
// Created by millefeuille on 4/28/23.
//

#include "ft_malloc.h"

void print_address(void *ptr) {
	ft_putstr("0x");
	ft_putnbr_base((unsigned long) ptr, "0123456789abcdef", 16);
}

int is_printable(int c) {
	return c >= 32 && c <= 126;
}

void print_memory(void *start, size_t size, char *indent) {
	char *ptr = start;
	size_t printed_char = 0;
	ft_putstr(indent);
	for (size_t i = 0; i < size; i++) {
		char c = ptr[i];
		if (!c)
			c = '.';
		if (!is_printable(c))
			c = '_';
		ft_putchar(c);
		printed_char++;
		if (printed_char > 64) {
			ft_putchar('\n');
			ft_putstr(indent);
			printed_char = 0;
		}
	}
}

void print_chunk_data(char *start, size_t size) {
	print_address(start);
	ft_putstr(" - ");
	print_address(start + size);
	ft_putstr(" : ");
	ft_putnbr((int) size);
	ft_putstr(" bytes");
}

void print_chunk_header(chunk_ptr chunk) {
	print_chunk_data(head_to_mem(chunk), chunk->_size_add);
#ifdef FT_MALLOC_DEBUG
	ft_putstr(" -- (");
	print_chunk_data((char *)chunk, get_chunk_size(chunk));
	ft_putstr(")\n");
	print_memory(head_to_mem(chunk), get_chunk_size(chunk), "\t\t");
#endif
}

void print_zone_header(zone_ptr zone) {
	ft_putstr(" - size: ");
	ft_putnbr((int) zone->_size);
}

void print_chunks(chunk_ptr chunk) {
	for (; chunk; chunk = chunk->next) {
		if (is_chunk_free(chunk))
			continue;
		ft_putstr("\n");
		print_chunk_header(chunk);
		#ifdef FT_MALLOC_DEBUG

		#endif
	}
}

void print_zones(zone_ptr zone) {
	if (!zone)
		return;

	for (; zone; zone = zone->next) {
		if (zone->_size > manager.tiny_size)
			ft_putstr("SMALL : ");
		else
			ft_putstr("TINY : ");
		print_address(zone);
		#ifdef FT_MALLOC_DEBUG
			print_zone_header(zone);
		#endif
		print_chunks(get_zone_first_chunk(zone));
	}
}

void show_alloc_mem(void) {
	print_zones(manager.tiny_zones);
	print_zones(manager.small_zones);
	print_chunks(manager.large_allocs);
	ft_putchar('\n');
}
