# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Compilation Options
# Debug mode [yes/no] (allowing to debug the library via gdb):
DEBUG   ?= no
# Optional modules (remove unwanted ones):
MODULES ?= cgi fastcgi loadfiles urlenc
# Specify your favourite C compiler here:
CC      ?= gcc


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Preparations
# Compile under the most strict conditions:
CFLAGS   = -xc -ansi -pedantic -Wall -Wextra -MMD
# Specify linker to use the library:
LFLAGS   = -L$(BUILD) -lmagi
# Debug and optimisation (as well as -static for valgrind) are not compatible:
ifeq '$(DEBUG)' 'yes'
CFLAGS  += -g -O0
else
CFLAGS  += -O3
LFLAGS  += -static
endif

# Directories definitions:
INCLUDE  = include
BUILD    = build
SRCDIR   = src
EXADIR   = examples
# Library itself:
LIB      = libmagi.a
# Modules:
EXTERNAL = cookie error file param request response session $(MODULES)
INTERNAL = cookies multipart tools urlencoded

# Default target is library:
TARGET   = $(BUILD)/$(LIB)
# Determing needed object files:
EXTER_H  = $(foreach x,$(EXTERNAL:=.h),$(INCLUDE)/magi/$(x))
EXTER_C  = $(foreach x,$(EXTERNAL:=.c),$(SRCDIR)/$(x))
INTER_H  = $(foreach i,$(INTERNAL:=.h),$(SRCDIR)/$(i))
INTER_C  = $(INTER_H:.h=.c)
SRC      = $(INTER_C) $(EXTER_C)
OBJ      = $(foreach o,$(SRC:.c=.o),$(BUILD)/$(o))
# Example executables:
EXASRC   = $(wildcard $(EXADIR)/*.c)
EXAMPLES = $(foreach x,$(EXASRC:.c=),$(BUILD)/$(x))
# Dependency files:
DEPS     = $(OBJ:.o=.d) $(EXAMPLES:=.d)

# Adding special include paths to corresponding flags:
SRCFLAGS = $(CFLAGS) -I$(INCLUDE)/magi
EXAFLAGS = $(CFLAGS) -I$(INCLUDE)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Targets
.PHONY: default examples clean

default: $(BUILD)/$(SRCDIR) $(TARGET)

examples: default $(BUILD)/$(EXADIR) $(EXAMPLES)

clean:
	rm -f $(TARGET) $(OBJ) $(EXAMPLES) $(DEPS)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Compilation
# Including dependency files:
-include $(DEPS)

# Packing object files into library:
$(TARGET): $(OBJ)
	ar -rcs $@ $^

# Compile object files from corresponding source:
$(BUILD)/%.o: %.c
	$(CC) $(SRCFLAGS) -c $< -o $@

# Compile executables from corresponding sources and library:
$(BUILD)/%: %.c $(TARGET)
	$(CC) $(EXAFLAGS) $< $(LFLAGS) -o $@

$(BUILD)/$(SRCDIR) $(BUILD)/$(EXADIR):
	mkdir -p $@
