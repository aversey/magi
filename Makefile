# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Compilation Options
# Debug mode (uncoment to be able to debug the library via gdb):
# DEBUG   = yes
# Specify your favourite C compiler here (e.g. tcc):
CC      = gcc


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Preparations
LIB     = libmagi.a

# Compile under the most strict conditions:
CFLAGS  = -xc -ansi -pedantic -Wall
# Debug and optimisation are not compatible:
ifeq '$(DEBUG)' 'yes'
CFLAGS += -g -O0
else
CFLAGS += -O3
endif

# Object files listing:
SRC_DIR = src
SRC     = $(wildcard $(SRC_DIR)/*.c)
OBJ     = $(SRC:.c=.o)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Targets
# 'make' will produce library by default:
default: $(LIB)

# Cleaning means removing everything automatically produced:
clean:
	rm -f $(OBJ) $(LIB) deps.mk


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Compilation
# Compile object files from corresponding source and header:
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

# Packing object files into library:
$(LIB): $(OBJ)
	ar rcs $@ $^


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Dependencies
# Generating dependencies description file:
deps.mk: $(SRC)
	$(CC) -MM $^ > $@

# While cleaning we need to remove it, not include:
ifneq "clean" "$(MAKECMDGOALS)"
-include deps.mk
endif
