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
# Debug and optimisation are not compatible:
ifeq '$(DEBUG)' 'yes'
CFLAGS  += -g -O0
else
CFLAGS  += -O3
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

# Flags collections:
SRCFLAGS = $(CFLAGS) -I$(INCLUDE)/magi
EXAFLAGS = $(CFLAGS) -I$(INCLUDE)
LFLAGS   = -static -L$(BUILD) -lmagi

# Build directories:
BUILDIRS = $(BUILD) $(BUILD)/$(SRCDIR) $(BUILD)/$(EXADIR)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Targets
.PHONY: default examples clean clean-nontarget clean-deps clean-all

default: $(TARGET)

examples: $(EXAMPLES)

clean: clean-nontarget
	rm -f $(TARGET)
clean-nontarget: clean-deps
	rm -f $(OBJ) $(EXAMPLES)
clean-deps:
	rm -f $(DEPS)
clean-all:
	rm -rf $(BUILD)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Compilation
# No product should be in case of cleaning:
ifneq (clean,$(MAKECMDGOAL))

# Packing object files into library:
$(TARGET): $(OBJ)
	ar -rcs $@ $^

# Compile object files from corresponding source:
$(BUILD)/%.o: %.c $(BUILDIRS)
	$(CC) $(SRCFLAGS) -c $< -o $@

# Compile executables from corresponding sources and library:
$(BUILD)/%: %.c $(TARGET) $(BUILDIRS)
	$(CC) $(EXAFLAGS) $< $(LFLAGS) -o $@

$(BUILDIRS):
	mkdir -p $@


# Including dependency files:
-include $(DEPS)
endif
