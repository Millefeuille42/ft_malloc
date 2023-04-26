//
// Created by millefeuille on 4/25/23.
//

#include "ft_malloc.h"

int main(void) {
	//for (int i = 0; i < 100; i++)
	//	malloc(34);

	char *test = malloc(2);
	char *test2 = malloc(2);
	test[0] = 'a';
	test[1] = '\0';
	test2[0] = 'b';
	test2[1] = '\0';

	//show_alloc_mem();

	printf("%p = %s\n", (void *) test, test);
	printf("%p = %s\n", (void *) test2, test2);
	free(test);
	free(test2);
}
