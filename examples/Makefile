# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Compilation Options
# Debug mode (allowing to debug the examples via gdb):
# DEBUG    = yes
# Specify your favourite C compiler here (e.g. tcc):
CC       = gcc


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Preparations
EXAMPLES = append cookie upload echo

# Compile under the most strict conditions:
CFLAGS   = -xc -ansi -pedantic -Wall
# Debug and optimisation are not compatible:
ifeq '$(DEBUG)' 'yes'
CFLAGS  += -g -O0
else
CFLAGS  += -O3
endif

# Including magi library headers and setting linker to use it:
INCLUDE  = -I ../include
LFLAGS   = -L.. -lmagi

# Specification of library file to produce it, if not provided:
MAGI     = ../libmagi.a


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Targets
# 'make' produces all examples by default:
default: $(EXAMPLES)

# Cleaning means removing everything automatically produced:
clean:
	rm -f $(EXAMPLES)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Compilation
# Compile executables from corresponding sources and library:
%: %.c $(MAGI)
	$(CC) $(CFLAGS) $(INCLUDE) $< $(LFLAGS) -o $@

# Run make for library to produce it:
$(MAGI):
	cd ..; $(MAKE)
