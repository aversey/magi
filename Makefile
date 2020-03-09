# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Compilation Options
# Debug mode (allowing to debug the library via gdb):
# DEBUG   = yes
# Optional modules (remove unwanted ones):
MODULES = cgi fastcgi loadfile urlenc
# Specify your favourite C compiler here:
CC      = gcc


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Preparations
LIB     = libmagi.a

# Compile under the most strict conditions:
CFLAGS  = -xc -ansi -pedantic -Wall -Wextra
# Debug and optimisation are not compatible:
ifeq '$(DEBUG)' 'yes'
CFLAGS += -g -O0
else
CFLAGS += -O3
endif

# Interfacial files to compile:
INTER   = cookie error file param request response $(MODULES)

# Object files listing:
INC_DIR = include/magi
SRC_DIR = src
INTER_H = $(foreach name,$(INTER),$(INC_DIR)/$(name).h)
INTER_C = $(foreach name,$(INTER),$(SRC_DIR)/$(name).c)
INNER_H = $(wildcard $(SRC_DIR)/*.h)
INNER_C = $(INNER_H:.h=.c)
SRC     = $(INTER_C) $(INNER_C)
OBJ     = $(SRC:.c=.o)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Targets
# 'make' produces library by default:
default: $(LIB)

# Cleaning means removing everything automatically produced:
clean:
	rm -f $(OBJ) $(LIB)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Compilation
# Compile object files from corresponding source and header:
%.o: %.c
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@

# Packing object files into library:
$(LIB): $(OBJ)
	ar rcs $@ $^
