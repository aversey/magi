# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Compilation Options
# Debug mode (allowing to debug the examples via gdb):
# DEBUG    = yes
# Examples to build by default:
EXAMPLES = append cookie echo upload fastcgi
# Specify your favourite C compiler here (e.g. tcc):
CC       = gcc


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Preparations
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
# Specify library file to set it as a prerequisite for compilation:
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
