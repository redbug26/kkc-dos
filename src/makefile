################

# -o3 pour maximum optimisation

CC = gcc
COFLAGS = -c -g -O3 -fno-exceptions -funsigned-char -Iinclude -Iinclude/kkt -DLINUX
CFLAGS =  -lncurses

OBJS = kkmain.o idf.o hard.o win.o gestion.o view.o driver.o ficidf.o copie.o delete.o search.o util.o reddies.o dgpdos.o macro.o kkedit.o english.o wired.o

SETUP_OBJS = kksetup.o hard.o idf.o reddies.o ficidf.o dgpdos.o macro.o english.o


all : kkmain

kkmain: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

kkmain.o: kkmain.cc
	$(CC) $(COFLAGS) $*.cc -o $@

kksetup: $(SETUP_OBJS)
	$(CC) $(CFLAGS) $(SETUP_OBJS) -o $@

kksetup.o: kksetup.cc
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

util.o: util.cc
	$(CC) $(COFLAGS) $*.cc -o $@

reddies.o: reddies.cc
	$(CC) $(COFLAGS) $*.cc -o $@

dgpdos.o: dgpdos.cc
	$(CC) $(COFLAGS) $*.cc -o $@

macro.o: macro.cc
	$(CC) $(COFLAGS) $*.cc -o $@

kkedit.o: kkedit.cc
	$(CC) $(COFLAGS) $*.cc -o $@

english.o: english.cc
	$(CC) $(COFLAGS) $*.cc -o $@

wired.o: wired.cc
	$(CC) $(COFLAGS) $*.cc -o $@
 
