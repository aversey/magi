# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Compilation Options
# Debug mode (allowing to debug the library via gdb):
# DEBUG    = yes
# Specify directory to store object files:
OBJ_DIR  = build
# Optional modules (remove unwanted ones):
MODULES  = cgi fastcgi loadfile urlenc
# Examples to build with 'examples' target:
EXAMPLES = append cookie echo upload
# Specify your favourite C compiler here:
CC       = gcc


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Preparations
LIB      = libmagi.a

# Compile under the most strict conditions:
CFLAGS   = -xc -ansi -pedantic -Wall -Wextra
# Debug and optimisation are not compatible:
ifeq '$(DEBUG)' 'yes'
CFLAGS  += -g -O0
else
CFLAGS  += -O3 -static
endif

# Interfacial files to compile:
INTER    = cookie error file param request response $(MODULES)

# Object files listing:
INC_DIR  = include/magi
SRC_DIR  = src
INTER_H  = $(foreach name,$(INTER),$(INC_DIR)/$(name).h)
INTER_C  = $(foreach name,$(INTER),$(SRC_DIR)/$(name).c)
INNER_H  = $(wildcard $(SRC_DIR)/*.h)
INNER_C  = $(INNER_H:.h=.c)
SRC      = $(INTER_C) $(INNER_C)
NAMES    = $(notdir $(SRC:.c=))
OBJ      = $(foreach name,$(NAMES),$(OBJ_DIR)/$(name).o)

EXDIR    = examples
EXSRC    = $(foreach ex,$(EXAMPLES),$(EXDIR)/$(ex).c)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Targets
.PHONY: default clean examples

# 'make' produces library by default:
default: $(OBJ_DIR) $(LIB)

# Cleaning means removing everything automatically produced:
clean:
	rm -rf $(OBJ_DIR) $(LIB) $(EXAMPLES) deps.mk

examples: default $(EXAMPLES)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Compilation
# Compile object files from corresponding source and header:
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@

# Packing object files into library:
$(LIB): $(OBJ)
	ar rcs $@ $^

# Making directory for object files:
$(OBJ_DIR):
	mkdir $@

# Compile executables from corresponding sources and library:
%: $(EXDIR)/%.c
	$(CC) $(CFLAGS) -I include $< -L. -lmagi -o $@


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Dependencies
deps.mk: $(SRC)
	echo '' > deps.mk
	for t in $(NAMES); do                                                     \
	$(CC) -I $(INC_DIR) -MT $(OBJ_DIR)/$${t}.o -MM $(SRC_DIR)/$${t}.c >> $@;  \
	done

ifneq (clean, $(MAKECMDGOALS))
-include deps.mk
endif
