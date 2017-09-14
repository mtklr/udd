# udd Makefile

VERSION = 5.1C-08

INSTALL = /usr/bin/install
INSTALL_DATA = $(INSTALL) -m 0644
PREFIX ?= $(shell pwd)/build

PROG = udd
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

DATA = char.dat lvl.dat not.txt ins.txt
ORB_FILE = orb.txt
CHR_LOCKFILE = char.lck
LVL_LOCKFILE = lvl.lck
WIZARD ?= $(shell echo $$UID)

CFLAGS += -O0 -g -Wall -arch i386 \
	  -DFIL_CHR=\"$(PREFIX)/char.dat\" \
	  -DFIL_CHR_LK=\"$(CHR_LOCKFILE)\" \
	  -DFIL_CHR_LD=\"$(PREFIX)\" \
	  -DFIL_LVL=\"$(PREFIX)/lvl.dat\" \
	  -DFIL_LVL_LK=\"$(LVL_LOCKFILE)\" \
	  -DFIL_LVL_LD=\"$(PREFIX)\" \
	  -DFIL_NOT=\"$(PREFIX)/not.txt\" \
	  -DFIL_ORB=\"$(PREFIX)/$(ORB_FILE)\" \
	  -DFIL_INS=\"$(PREFIX)/ins.txt\" \
	  -DWIZ_UID=$(WIZARD)
LDFLAGS += -arch i386
LDLIBS = -lm

all: $(PROG) $(DATA)

$(PROG): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(PROG)
	$(RM) *.o

install: all
	$(INSTALL) -d $(PREFIX)
	$(INSTALL) $(PROG) $(PREFIX)
	$(INSTALL) -d $(PREFIX)
	$(INSTALL_DATA) $(DATA) $(PREFIX)
	$(INSTALL_DATA) /dev/null $(PREFIX)/$(ORB_FILE)

uninstall:
	$(RM) $(PREFIX)/$(PROG)
	$(RM) $(foreach f,$(notdir $(DATA)),$(PREFIX)/$(f))
	$(RM) $(PREFIX)/$(ORB_FILE)
	$(RM) $(PREFIX)/$(CHR_LOCKFILE)
	$(RM) $(PREFIX)/$(LVL_LOCKFILE)

.PHONY: all clean install uninstall
