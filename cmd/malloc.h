//
// Created by millefeuille on 4/25/23.
//

#ifndef MALLOC_MALLOC_H
# define MALLOC_MALLOC_H

# include <unistd.h>

void *malloc(size_t size);
void show_alloc_mem(void);
void free(void *ptr);

#endif //MALLOC_MALLOC_H
