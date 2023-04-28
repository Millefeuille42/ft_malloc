//
// Created by millefeuille on 4/25/23.
//

#ifndef MALLOC_H
# define MALLOC_H

# include <unistd.h>

void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void show_alloc_mem(void);

#endif //MALLOC_H
