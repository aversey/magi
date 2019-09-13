# Uncomment following to enable debug mode:
# DEBUG    = yes

CC       = gcc
EXAMPLES = append cookie upload

CFLAGS   = -xc -ansi -pedantic -Wall
ifeq '$(DEBUG)' 'yes'
CFLAGS  += -g -O0
else
CFLAGS  += -O3
endif

INCLUDE  = ../src
LFLAGS   = -L.. -lcgi


default: $(EXAMPLES)

%: %.c
	$(CC) $(CFLAGS) -I $(INCLUDE) $< $(LFLAGS) -o $@

clean:
	rm -f $(EXAMPLES)
