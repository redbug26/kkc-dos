#-----------------------------------------------------------------------#
#                                                                       #
#                     Ketchup Killers making file                       #
#                                                                       #
#                      by RedBug/Ketchup Killers                        #
#                                                                       #
#-----------------------------------------------------------------------#

#.SILENT

#Verify the date of the libraries
VERLIB = 1

DEBUG = 1

CA = wasm
CC = wcc386
CP = wpp386
CL = *wlink
CB = wlib

RAR = rar

#----------------------#
# language             #
#----------------------#
LANGUAGE = english
#LANGUAGE = french

SYSTEM = pmodew
#dos32x
#pmodew
#os2v2_pm
# win95
#dos4g

ASM = tasm
ASMOPTS = -UT310 -p -ml -m9 -zi -d__WC32__ -l

kkcpath=include
kktpath=$(kkcpath)\kkt


#-----------------------------------------------------------------------#
# Maximum optimization for speed                                        #
# CCOPTS = -w3 -dDEBUG -5r -fpi -oneatx -s -zdp -d__WC32__ -zu          #
# -zu fait       tout planter                                           #
# -dDEBUG -d3    pour debugging                                         #
# -mf            pour Model FLAT                                        #
# -s             pour enlever les checks stack overflow                 #
# -zm            chaque fonction est mise dans un segment different     #
# -wx            tout les warning                                       #
# -od            disable tous les optimisations                         #
# -5r -5s        pentium register, stack calling                        #
# -et            pentium profilling                                     #
# -ou            unique adresse                                         #
# -xst           save exception                                         #
# -oneatx        pour optimization                                      #
# -fpi           pour emuler le coprocesseur                            #
# -za            pour se conformer a l'ansi c                           #
# option modname = 'Ketchup Killers Commander 1.00' &                   #
#-----------------------------------------------------------------------#

#------------------------ En phase de debugging ------------------------#
#-----------------------------------------------------------------------#
!ifdef DEBUG

CCOPTS = -wx -5r -fpc -d__WC32__ -mf -d3 -zm -od -dDEBUG -zp1 -v -i=$(kkcpath) -i=$(kktpath)
CPOPTS = -wx -5r -fpc -d__WC32__ -mf -d3 -zm -od -dDEBUG -zp1 -v -i=$(kkcpath) -i=$(kktpath)
AOPTS = /d1

LOPTS = option stack=256k debug all option c option map
LAOPTS = debug all

#------------------------  En phase de release  ------------------------#
#-----------------------------------------------------------------------#
!else

#zp1 alignement

CCOPTS = -w5 -5r -fpc -zdp -d__WC32__ -mf -s -zp1 -zm -i=$(kkcpath) -i=$(kktpath)

# -obx  (watcom 11.0) chaipuqoui
# -oneatx (prend bcp de taille)
# quand on mets les optimisations, on peux plus faire un shell, pq ?

# -zm: each function in a segment
# option el : eliminate no referenced segment

CPOPTS = -w5 -5r -fpc -zdp -d__WC32__ -mf -s -zp1 -zm -i=$(kkcpath) -i=$(kktpath)

AOPTS =

LOPTS = option stack=256k option c option el
LAOPTS =

!endif


LIB = idf.lib hard.lib kkedit.lib search.lib ficidf.lib reddies.lib &
            view.lib

DEP = kkmain.obj win.obj gestion.obj driver.obj &
      macro.obj copie.obj delete.obj util.obj watdos.obj &
      $(LANGUAGE).obj $(LIB)


#-----------------------------------------------------------------------#
#----------------- Contruction de tous les executables -----------------#
#-----------------------------------------------------------------------#

all : kk.exe kkmain.exe kksetup.exe kkdesc.exe kkkkrgr.exe kkfont.exe &
      $(LIB) readme.exe

kksrc.rar : $(DEP)
            del file.lst
            for %i in ($(DEP)) do echo %i >>file.lst
            $(RAR) a kksrc.rar @file.lst

kkmain.exe  : $(DEP)
             $(CL) system $(SYSTEM) $(LOPTS)  &
                   name $@ file {$<}

kksetup.exe : kksetup.obj hard.lib idf.lib reddies.obj ficidf.obj &
              watdos.obj macro.obj english.obj
             $(CL) system $(SYSTEM) $(LOPTS) name $@ file {$<}

kkdesc.exe  : kkdesc.obj idf.lib hard.lib
             $(CL) system $(SYSTEM) $(LOPTS) name $@ file {$<}

kkkkrgr.exe : kkkkrgr.obj hard.lib
             $(CL) system $(SYSTEM) $(LOPTS) name $@ file {$<}

kkfont.exe  : kkfont.obj hard.lib
             $(CL) system $(SYSTEM) $(LOPTS) name $@ file {$<}

kk.exe      : kk.obj
             $(CL) $(LAOPTS) name $@ file {$<}

readme.exe  : readme.obj
             $(CL) $(LAOPTS) name $@ file {$<}

view.dll : view.obj hard.lib idf.lib reddies.obj
             $(CL) system $(SYSTEM) dll $(LOPTS) name $@ file {$<}

.asm.obj:
         $(CA) $* $(AOPTS)
.c.obj:
         $(CC) $* $(CCOPTS)
.cc.obj:
         $(CP) $* $(CCOPTS)


!ifdef VERLIB

idf.lib : idf.obj
         $(CB) -c -n $@ +-$<

hard.lib : hard.obj
         $(CB) -c -n $@ +-$<

view.lib : view.obj
         $(CB) -c -n $@ +-$<

kkedit.lib : kkedit.obj
         $(CB) -c -n $@ +-$<

search.lib : search.obj
         $(CB) -c -n $@ +-$<

ficidf.lib : ficidf.obj
         $(CB) -c -n $@ +-$<

reddies.lib : reddies.obj
         $(CB) -c -n $@ +-$<
!endif


#-----------------------------------------------------------------------#
#                                                                       #
#  This was the Ketchup Killers making file                             #
#                                                                       #
#          by RedBug/Ketchup Killers                                    #
#                                                                       #
# EMAIL: RedBug@pctrading.be                                            #
#-----------------------------------------------------------------------#
