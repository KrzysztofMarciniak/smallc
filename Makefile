CC      ?= gcc
CFLAGS  := -Wall -Wextra -Wpedantic -std=c11 -O2
AR      := ar
ARFLAGS := rcs

PREFIX  ?= /usr/local
LIBDIR  ?= $(PREFIX)/lib
INCDIR  ?= $(PREFIX)/include/smallc

SRC := \
	hashmap/hashmap.c \
	memory/memory.c \
	print/print.c \
	string/string.c \
	vector/vector.c

OBJ := $(SRC:.c=.o)

TARGET := libsmallc.a

HEADERS := \
	hashmap/hashmap.h \
	memory/memory.h \
	print/print.h \
	string/string.h \
	types/types.h \
	vector/vector.h

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -I. -c $< -o $@

install: $(TARGET)
	install -d $(DESTDIR)$(LIBDIR)
	install -d $(DESTDIR)$(INCDIR)

	install -m 644 $(TARGET) $(DESTDIR)$(LIBDIR)/

	install -m 644 hashmap/hashmap.h $(DESTDIR)$(INCDIR)/
	install -m 644 memory/memory.h  $(DESTDIR)$(INCDIR)/
	install -m 644 print/print.h     $(DESTDIR)$(INCDIR)/
	install -m 644 string/string.h   $(DESTDIR)$(INCDIR)/
	install -m 644 types/types.h     $(DESTDIR)$(INCDIR)/
	install -m 644 vector/vector.h   $(DESTDIR)$(INCDIR)/

uninstall:
	rm -f $(DESTDIR)$(LIBDIR)/$(TARGET)

	rm -f $(DESTDIR)$(INCDIR)/hashmap.h
	rm -f $(DESTDIR)$(INCDIR)/memory.h
	rm -f $(DESTDIR)$(INCDIR)/print.h
	rm -f $(DESTDIR)$(INCDIR)/string.h
	rm -f $(DESTDIR)$(INCDIR)/types.h
	rm -f $(DESTDIR)$(INCDIR)/vector.h

	rmdir --ignore-fail-on-non-empty $(DESTDIR)$(INCDIR)

clean:
	rm -f $(OBJ) $(TARGET)
