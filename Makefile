# udd Makefile

CC ?= /usr/bin/cc
RM ?= /bin/rm -f
INSTALL ?= /usr/bin/install
INSTALL_DATA ?= $(INSTALL) -m 0644

PREFIX ?= $(PWD)/build
gamesdir = $(PREFIX)/games
datadir = $(PREFIX)/share/$(PROG)
localstatedir = $(PREFIX)/var/games

PROG = udd
VERSION = 5.1C-08
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

DATA = char.dat lvl.dat not.txt ins.txt
ORB_FILE = orb.txt
CHR_LOCKFILE = char.lck
LVL_LOCKFILE = lvl.lck
PAGER ?= /usr/bin/less
WIZARD ?= $(shell echo $$UID)

OPT ?= -Os # -O0 -g -Wall -Wextra
CFLAGS += $(OPT) \
	  -DFIL_CHR=\"$(PREFIX)/char.dat\" \
	  -DFIL_CHR_LK=\"$(CHR_LOCKFILE)\" \
	  -DFIL_CHR_LD=\"$(PREFIX)\" \
	  -DFIL_LVL=\"$(PREFIX)/lvl.dat\" \
	  -DFIL_LVL_LK=\"$(LVL_LOCKFILE)\" \
	  -DFIL_LVL_LD=\"$(PREFIX)\" \
	  -DFIL_NOT=\"$(PREFIX)/not.txt\" \
	  -DFIL_ORB=\"$(PREFIX)/$(ORB_FILE)\" \
	  -DFIL_INS=\"$(PREFIX)/ins.txt\" \
	  -DWIZ_UID=$(WIZARD) \
	  -DPAGER=\"$(PAGER)\"
LDFLAGS +=
LDLIBS = -lm

.PHONY: all
all: $(PROG) $(DATA)

$(PROG): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

.PHONY: clean
clean:
	$(RM) $(PROG)
	$(RM) *.o

.PHONY: install
install: all
	$(INSTALL) -d $(DESTDIR)$(PREFIX)
	$(INSTALL) $(PROG) $(DESTDIR)$(PREFIX)
	$(INSTALL) -d $(DESTDIR)$(PREFIX)
	$(INSTALL_DATA) $(DATA) $(DESTDIR)$(PREFIX)
	$(INSTALL_DATA) /dev/null $(DESTDIR)$(PREFIX)/$(ORB_FILE)

.PHONY: uninstall
uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/$(PROG)
	$(RM) $(foreach f,$(notdir $(DATA)),$(DESTDIR)$(PREFIX)/$(f))
	$(RM) $(DESTDIR)$(PREFIX)/$(ORB_FILE)
	$(RM) $(DESTDIR)$(PREFIX)/$(CHR_LOCKFILE)
	$(RM) $(DESTDIR)$(PREFIX)/$(LVL_LOCKFILE)
