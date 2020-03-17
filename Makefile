# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Compilation Options
# Debug mode [yes/no] (allowing to debug the library via gdb):
DEBUG   ?= no
# Specify build directory:
BUILD   ?= build
# Optional modules (remove unwanted ones):
MODULES ?= cgi fastcgi loadfile urlenc
# Specify your favourite C compiler here:
CC      ?= gcc


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
OBJ      = $(foreach name,$(NAMES),$(BUILD)/$(SRC_DIR)/$(name).o)

EXDIR    = examples
EXSRC    = $(wildcard $(EXDIR)/*.c)
EXNAMES  = $(notdir $(EXSRC:.c=))
EXAMPLES = $(foreach ex,$(EXNAMES),$(BUILD)/$(EXDIR)/$(ex))


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Targets
.PHONY: default clean examples

# 'make' produces library by default:
default: $(BUILD)/$(LIB)

examples: $(EXAMPLES)

# Cleaning means removing everything automatically produced:
clean:
	rm -rf $(BUILD)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Compilation
# Compile object files from corresponding source and header:
$(BUILD)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@

# Packing object files into library:
$(BUILD)/$(LIB): $(OBJ)
	ar rcs $@ $^

# Compile executables from corresponding sources and library:
$(BUILD)/$(EXDIR)/%: $(EXDIR)/%.c $(BUILD)/$(LIB)
	$(CC) $(CFLAGS) -I include $< -L$(BUILD) -lmagi -o $@


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Dependencies
$(BUILD)/deps.mk: $(SRC) $(EXSRC)
	mkdir $(BUILD) $(BUILD)/$(SRC_DIR) $(BUILD)/$(EXDIR); echo '' > $@
	for t in $(NAMES); do                 \
	$(CC) -I $(INC_DIR)                   \
	      -MT $(BUILD)/$(SRC_DIR)/$${t}.o \
	      -MM $(SRC_DIR)/$${t}.c >> $@;   \
	done
	for t in $(EXNAMES); do             \
	$(CC) -I include                    \
	      -MT $(BUILD)/$(EXDIR)/$${t}.o \
	      -MM $(EXDIR)/$${t}.c >> $@;   \
	done

ifneq (clean, $(MAKECMDGOALS))
-include $(BUILD)/deps.mk
endif
