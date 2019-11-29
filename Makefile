# Debug mode:
# DEBUG   = yes

CC      = gcc
LIB     = libmagi.a

CFLAGS  = -xc -ansi -pedantic -Wall
ifeq '$(DEBUG)' 'yes'
CFLAGS += -g -O0
else
CFLAGS += -O3
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
