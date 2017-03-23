################

# -o3 pour maximum optimisation

CC = gcc
CFLAGS = -c -s -O3 -funsigned-char -fpack-struct -Iinclude -Iinclude/kkt -DLINUX -Wno-c++11-compat-deprecated-writable-strings -Wno-invalid-source-encoding 
LFLAGS = -lncurses -lstdc++ -ltinfo
#CFLAGS = -funsigned-char -fpack-struct -Iinclude -Iinclude/kkt -DLINUX -Wno-c++11-compat-deprecated-writable-strings

OBJS = kkmain.o idf.o hard.o win.o gestion.o view.o driver.o ficidf.o copie.o delete.o search.o util.o reddies.o cygdos.o macro.o edit.o english.o wired.o

#CPFLAGS = -s -O3 -m486 -funsigned-char -fpack-struct -Iinclude -Iinclude/kkt -DLINUX -lncurses

all: kkc

kkedit: kkedit.cc hard.cc
	$(CC) $(CFLAGS) -DTEST kkedit.cc hard.cc -o $@

kkc : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $@

kkmain.o: kkmain.cc
	$(CC) $(CFLAGS) $*.cc -o $@

idf.o: idf.cc
	$(CC) $(CFLAGS) $*.cc -o $@

hard.o: hard.cc
	$(CC) $(CFLAGS) $*.cc -o $@

win.o: win.cc
	$(CC) $(CFLAGS) $*.cc -o $@

gestion.o: gestion.cc
	$(CC) $(CFLAGS) $*.cc -o $@

view.o: view.cc
	$(CC) $(CFLAGS) $*.cc -o $@

driver.o: driver.cc
	$(CC) $(CFLAGS) $*.cc -o $@

ficidf.o: ficidf.cc
	$(CC) $(CFLAGS) $*.cc -o $@

copie.o: copie.cc
	$(CC) $(CFLAGS) $*.cc -o $@

delete.o: delete.cc
	$(CC) $(CFLAGS) $*.cc -o $@

search.o: search.cc
	$(CC) $(CFLAGS) $*.cc -o $@

kkfct.o: kkfct.cc
	$(CC) $(CFLAGS) $*.cc -o $@

util.o: util.cc
	$(CC) $(CFLAGS) $*.cc -o $@

reddies.o: reddies.cc
	$(CC) $(CFLAGS) $*.cc -o $@

cygdos.o: cygdos.cc
	$(CC) $(CFLAGS) $*.cc -o $@

macro.o: macro.cc
	$(CC) $(CFLAGS) $*.cc -o $@

edit.o: edit.cc
	$(CC) $(CFLAGS) $*.cc -o $@

english.o: english.cc
	$(CC) $(CFLAGS) $*.cc -o $@

wired.o: wired.cc
	$(CC) $(CFLAGS) $*.cc -o $@
