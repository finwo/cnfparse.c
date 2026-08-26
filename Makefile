BIN?=cnfparse_test

LIBS:=
SRC:=
INCLUDES:=
INCLUDES+=-I src

LDFLAGS:=
CFLAGS?=-Wall -s -O2

include lib/.dep/config.mk

SRC+=src/cnfparse.c
SRC+=test/main.c

OBJ:=$(SRC:.c=.o)
OBJ:=$(OBJ:.cc=.o)

CFLAGS+=$(INCLUDES)

LDFLAGS+=$(CFLAGS)

.PHONY: default
default: $(BIN) README.md

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

.PHONY: clean
clean:
	rm -rf $(OBJ)

README.md: src/cnfparse.h
	stddoc < src/cnfparse.h > README.md
