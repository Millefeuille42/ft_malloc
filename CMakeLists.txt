cmake_minimum_required(VERSION 3.10)
project(malloc C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall -Werror -Wextra -pedantic")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Werror -Wextra -pedantic")

set(SOURCE_MALLOC
		cmd/main.c
		cmd/malloc.h
		cmd/ft_malloc.h
		cmd/malloc.c
		cmd/free.c
		cmd/realloc.c
)

add_executable(malloc ${SOURCE_MALLOC})
target_link_libraries(malloc LINK_PUBLIC)