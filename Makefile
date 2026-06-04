CC      ?= tcc
CFLAGS  ?= -Wall -Wextra -Wpedantic -std=c99 -O2
PICFLAGS ?= -fPIC

AR      ?= ar
ARFLAGS ?= rcs

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

TARGET_A  := libsmallc.a
TARGET_SO := libsmallc.so

HEADERS := \
	hashmap/hashmap.h \
	memory/memory.h \
	print/print.h \
	string/string.h \
	types/types.h \
	vector/vector.h

.PHONY: all clean install uninstall

all: $(TARGET_A) $(TARGET_SO)

$(TARGET_A): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(TARGET_SO): CFLAGS += $(PICFLAGS)
$(TARGET_SO): $(OBJ)
	$(CC) -shared -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -I. -c $< -o $@

install: $(TARGET_A) $(TARGET_SO)
	install -d $(DESTDIR)$(LIBDIR)
	install -d $(DESTDIR)$(INCDIR)

	install -m 644 $(TARGET_A)  $(DESTDIR)$(LIBDIR)/
	install -m 755 $(TARGET_SO) $(DESTDIR)$(LIBDIR)/

	for h in $(HEADERS); do \
		install -m 644 $$h $(DESTDIR)$(INCDIR)/; \
	done

uninstall:
	rm -f $(DESTDIR)$(LIBDIR)/$(TARGET_A)
	rm -f $(DESTDIR)$(LIBDIR)/$(TARGET_SO)

	for h in $(notdir $(HEADERS)); do \
		rm -f $(DESTDIR)$(INCDIR)/$$h; \
	done

	rmdir --ignore-fail-on-non-empty $(DESTDIR)$(INCDIR)

clean:
	rm -f $(OBJ) $(TARGET_A) $(TARGET_SO)
