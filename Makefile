BIN?=cnfparse_test

LIBS:=
SRC:=
INCLUDES:=
INCLUDES+=-I src

LDFLAGS:=
CFLAGS?=-Wall -s -O2

include lib/.dep/config.mk

SRC+=src/cnfparse.c
EXAMPLE_SRC:=example.c

OBJ:=$(SRC:.c=.o)
OBJ:=$(OBJ:.cc=.o)

CFLAGS+=$(INCLUDES)

LDFLAGS+=$(CFLAGS)

TESTS:=
TESTS+=$(patsubst %.c,%,$(wildcard test/*.c))

.PHONY: default
default: $(BIN) README.md

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN): $(OBJ) $(EXAMPLE_SRC:.c=.o)
	$(CC) $(CFLAGS) $^ -o $@

$(TESTS): $(OBJ) $(TESTS:=.c)
	$(CC) $@.c $(SRC) -I test $(CFLAGS) -o $@

.PHONY: tests
tests: $(TESTS) $(OBJ)
	for test in $(TESTS); do \
		$$test ; \
	done

.PHONY: clean
clean:
	rm -rf $(OBJ) $(TESTS)

README.md: src/cnfparse.h
	stddoc < src/cnfparse.h > README.md
