//
// Created by millefeuille on 4/25/23.
//

#include <stdio.h>

#include "./malloc.h"
# include <sys/mman.h>
//include <stdlib.h>

int main(void) {
	void *addr = mmap(NULL, 1, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	printf("Hi, my name is %p\n", addr);
	printf("Hi, my name is %p\n", addr);

	char *test = malloc(2);
	if (test == NULL)
		return 0;
	char *test2 = malloc(2);
	if (test2 == NULL)
		return 0;
	test[0] = 'a';
	test[1] = '\0';
	test2[0] = 'b';
	test2[1] = '\0';

	show_alloc_mem();

	//printf("%p = %s\n", (void *) test, test);
	//printf("%p = %s\n", (void *) test2, test2);
	free(test);
	free(test2);
}
