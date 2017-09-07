#
# udd Makefile file - Master Makefile, thanks to "mkmf"
#
# 03-Sep-89, Chuck Cranor
#
# V5.1C
#

DEST	      = /tmp

HDRS	      = cbt.h \
		data.h \
		defs.h

CFLAGS	      = -g

LDFLAGS	      = -lm

LIBS	      =

LINKER	      = cc

MAKEFILE      = Makefile

OBJS	      = cbt.o \
		cbt_cast.o \
		cbt_hit.o \
		chr_file.o \
		dgn.o \
		dgn_nomove.o \
		dgn_voices.o \
		dlvl_file.o \
		lock.o \
		opr.o \
		spc.o \
		swb.o \
		swb_file.o \
		trs.o \
		trs_cobjs.o \
		unix.o \
		utl_pplot.o \
		utl_util.o

PRINT	      = pr

PROGRAM	      = udd

SRCS	      = cbt.c \
		cbt_cast.c \
		cbt_hit.c \
		chr_file.c \
		dgn.c \
		dgn_nomove.c \
		dgn_voices.c \
		dlvl_file.c \
		lock.c \
		opr.c \
		spc.c \
		swb.c \
		swb_file.c \
		trs.c \
		trs_cobjs.c \
		unix.c \
		utl_pplot.c \
		utl_util.c

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS) $(LIBS)
		@echo -n "Loading $(PROGRAM) ... "
		@$(LINKER) $(OBJS) $(LIBS) -o $(PROGRAM) $(LDFLAGS)
		@echo "done"

clean:;		@rm -f $(OBJS)

depend:;	@mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

index:;		@ctags -wx $(HDRS) $(SRCS)

install:	$(PROGRAM)
		@echo Installing $(PROGRAM) in $(DEST)
		@install -s $(PROGRAM) $(DEST)

print:;		@$(PRINT) $(HDRS) $(SRCS)

program:        $(PROGRAM)

tags:           $(HDRS) $(SRCS); @ctags $(HDRS) $(SRCS)

update:		$(DEST)/$(PROGRAM)

$(DEST)/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		@make -f $(MAKEFILE) DEST=$(DEST) install
###
cbt.o: defs.h cbt.h 
cbt_cast.o: defs.h cbt.h 
cbt_hit.o: defs.h cbt.h
chr_file.o: defs.h 
dgn.o: defs.h
dgn_nomove.o: defs.h
dlvl_file.o: defs.h 
opr.o: defs.h 
spc.o: defs.h 
swb.o: defs.h data.h 
swb_file.o: defs.h
trs.o: defs.h 
trs_cobjs.o: defs.h 
unix.o: defs.h 
utl_pplot.o: defs.h
utl_util.o: defs.h 
