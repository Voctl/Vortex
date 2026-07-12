CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic -O2 -flto -D_DEFAULT_SOURCE
LDFLAGS =
PREFIX  = /usr/local

SRCDIR  = src
BUILDDIR = build
TARGET  = vortex

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))
INCS = -Iinclude -I$(SRCDIR)

.PHONY: all clean install uninstall

all: $(BUILDDIR) $(TARGET)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILDDIR) $(TARGET)

install: $(TARGET)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)
