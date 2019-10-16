# Uncomment following to enable debug mode:
# DEBUG    = yes

CC       = gcc
EXAMPLES = append cookie upload echo

CFLAGS   = -xc -ansi -pedantic -Wall
ifeq '$(DEBUG)' 'yes'
CFLAGS  += -g -O0
else
CFLAGS  += -O3
endif

INCLUDE  = -I ../src
LFLAGS   = -L.. -lmagi

MAGI     = ../libmagi.a


default: $(EXAMPLES)

$(MAGI):
	cd ..; $(MAKE)

%: %.c $(MAGI)
	$(CC) $(CFLAGS) $(INCLUDE) $< $(LFLAGS) -o $@

clean:
	rm -f $(EXAMPLES)
