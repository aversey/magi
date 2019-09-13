# Debug mode:
# DEBUG   = yes
# Error logging (writes to stderr):
# ERRLOG  = yes

CC      = gcc
LIB     = libcgi.a

CFLAGS  = -xc -ansi -pedantic -Wall
ifeq '$(DEBUG)' 'yes'
CFLAGS += -g -O0
else
CFLAGS += -O3
endif
ifeq '$(ERRLOG)' 'yes'
CFLAGS += -D ERRLOG
endif

SRC_DIR = src
SRC     = $(wildcard $(SRC_DIR)/*.c)
OBJ     = $(SRC:.c=.o)


default: $(LIB)

ifneq "clean" "$(MAKECMDGOALS)"
-include deps.mk
endif

deps.mk: $(SRC)
	$(CC) -MM $^ > $@

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(OBJ)
	ar rcs $@ $^

clean:
	rm -f $(OBJ) $(LIB) deps.mk
