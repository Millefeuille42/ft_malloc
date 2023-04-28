cmake_minimum_required(VERSION 3.10)
project(malloc C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall -Werror -Wextra -pedantic")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Werror -Wextra -pedantic")

set(SOURCE_MALLOC
		cmd/main.c
		cmd/malloc.c
		cmd/free.c
		cmd/realloc.c
		cmd/show_alloc_mem.c
		cmd/manager.c
		cmd/chunk.c
		cmd/zone.c
		cmd/utils.c
		cmd/malloc.h
		cmd/ft_malloc.h

		pkg/ft_print/ft_putchar.c
		pkg/ft_print/ft_putnbr.c
		pkg/ft_print/ft_putstr.c
		pkg/ft_print/ft_print.h
		pkg/ft_print/ft_putnbr_base.c
		)

add_executable(malloc ${SOURCE_MALLOC})
target_link_libraries(malloc LINK_PUBLIC)