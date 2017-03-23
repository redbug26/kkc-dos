################

# -o3 pour maximum optimisation

CC = gcc
CFLAGS = -s -O3 -funsigned-char -fpack-struct -Iinclude -Iinclude/kkt -DLINUX
COFLAGS = -funsigned-char -fpack-struct -Iinclude -Iinclude/kkt -DLINUX

OBJS = kkmain.o idf.o hard.o win.o gestion.o view.o driver.o ficidf.o copie.o delete.o search.o kkfct.o util.o reddies.o cygdos.o macro.o edit.o

all : kkc

kkc : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

kkmain.o: kkmain.cc
	$(CC) $(COFLAGS) $*.cc -o $@

idf.o: idf.cc
	$(CC) $(COFLAGS) $*.cc -o $@

hard.o: hard.cc
	$(CC) $(COFLAGS) $*.cc -o $@

win.o: win.cc
	$(CC) $(COFLAGS) $*.cc -o $@

gestion.o: gestion.cc
	$(CC) $(COFLAGS) $*.cc -o $@

view.o: view.cc
	$(CC) $(COFLAGS) $*.cc -o $@

driver.o: driver.cc
	$(CC) $(COFLAGS) $*.cc -o $@

ficidf.o: ficidf.cc
	$(CC) $(COFLAGS) $*.cc -o $@

copie.o: copie.cc
	$(CC) $(COFLAGS) $*.cc -o $@

delete.o: delete.cc
	$(CC) $(COFLAGS) $*.cc -o $@

search.o: search.cc
	$(CC) $(COFLAGS) $*.cc -o $@

kkfct.o: kkfct.cc
	$(CC) $(COFLAGS) $*.cc -o $@

util.o: util.cc
	$(CC) $(COFLAGS) $*.cc -o $@

reddies.o: reddies.cc
	$(CC) $(COFLAGS) $*.cc -o $@

cygdos.o: cygdos.cc
	$(CC) $(COFLAGS) $*.cc -o $@

macro.o: macro.cc
	$(CC) $(COFLAGS) $*.cc -o $@

edit.o: edit.cc
	$(CC) $(COFLAGS) $*.cc -o $@


