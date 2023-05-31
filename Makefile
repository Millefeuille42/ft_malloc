######### Sources #########

SOURCES	= 		cmd/manager.c \
                cmd/malloc.c \
                cmd/realloc.c \
                cmd/zone.c \
                cmd/chunk.c \
                cmd/show_alloc_mem.c \
                cmd/utils.c \
                cmd/free.c \
                pkg/ft_print/ft_putchar.c \
                pkg/ft_print/ft_putstr.c \
                pkg/ft_print/ft_putnbr_base.c \
                pkg/ft_print/ft_putnbr.c \



HEADERS	=		cmd/ft_malloc.h \
				pkg/ft_print/ft_print.h

HEADERS_DIRECTORIES	=			cmd \

######### Details #########

NAME	=	libft_malloc.so
SHORT_NAME = libft_malloc
SOURCES_EXTENSION = c

######### Compilation #########

COMPILE		=	clang
LIB			=	ar rc
DELETE		=	rm -f

FLAGS		=	-Wall -Werror -Wextra -pedantic

######### Additional Paths #########

vpath	%.h $(HEADERS_DIRECTORIES)

# ################################### #
#        DO NOT ALTER FURTHER!        #
# ################################### #

######### Additional Paths #########

vpath	%.o $(OBJS_DIR)
vpath	%.d $(DEPS_DIR)

######### Implicit Macros #########

OBJS_DIR	= .objs/
DEPS_DIR	= .deps/

OBJS	=	$(addprefix $(OBJS_DIR), $(SOURCES:.$(SOURCES_EXTENSION)=.o))
DEPS	=	$(addprefix $(DEPS_DIR), $(SOURCES:.$(SOURCES_EXTENSION)=.d))

#########  Rules  #########

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

LIB_NAME := $(SHORT_NAME)_$(HOSTTYPE).so

all:	$(OBJS_DIR) $(DEPS_DIR) $(NAME) ## Compile project and dependencies

$(LIB_NAME): $(OBJS)
			$(LIB) $(LIB_NAME) $(OBJS)

$(NAME):	$(LIB_NAME) ## Compile project
			ln -s $(shell pwd)/$(LIB_NAME) $(NAME)

clean: clean_deps clean_objs ## Delete object files

fclean:	clean clean_bin ## Delete object files and binary

re:	fclean ## Delete object files and binary, then recompile all
			@make --no-print-directory all

help: ## Print this help
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z_-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

#########  Sub Rules  #########

objs: $(OBJS_DIR) $(DEPS_DIR) $(OBJS) ## Compile object files

clean_deps: ## Delete dependency files and directory
			$(DELETE) -r $(DEPS_DIR)
clean_objs: ## Delete object files and directory
			$(DELETE) -r $(OBJS_DIR)
clean_bin: ## Delete produced binary
			$(DELETE) $(SHORT_NAME)_$(HOSTTYPE).so
			$(DELETE) $(NAME)

#########  Implicit Rules  #########

$(OBJS_DIR):
					@mkdir -p $(OBJS_DIR)

$(DEPS_DIR):
					@mkdir -p $(DEPS_DIR)

$(OBJS_DIR)%.o:	%.$(SOURCES_EXTENSION)
			mkdir -p $(OBJS_DIR)$(dir $<)
			@mkdir -p $(DEPS_DIR)$(dir $<)
			$(COMPILE) $(FLAGS) -MMD -MP -MF $(DEPS_DIR)$*.d -c $< -o $@

.PHONY	:	all clean fclean re help

#########  Includes  #########

-include $(DEPS)
