#-----------------------------------------------------------------------#
#                                                                       #
#                     Ketchup Killers making file                       #
#                                                                       #
#                      by RedBug/Ketchup Killers                        #
#                                                                       #
#-----------------------------------------------------------------------#

#.SILENT

DEBUG = 1

CA = wasm
CC = wcc386
CL = *wlink
CB = wlib

SYSTEM = pmodew
#os2v2_pm
# win95
#dos4g

ASM = tasm
ASMOPTS = -UT310 -p -ml -m9 -zi -d__WC32__ -l

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
#-----------------------------------------------------------------------#

#------------------------ En phase de debugging ------------------------#
#-----------------------------------------------------------------------#
!ifdef DEBUG

CCOPTS = -wx -5r -fpc -zdp -d__WC32__ -mf -d3 -zm -od -dDEBUG -zp1
AOPTS = /d1
LOPTS = option stack=60000 debug all option c option map
LAOPTS = debug all

#------------------------  En phase de release  ------------------------#
#-----------------------------------------------------------------------#
!else

#zp1 alignement

CCOPTS = -w5 -5r -fpc -zdp -d__WC32__ -mf -s -zp1 -zm
# -obx  (watcom 11.0) chaipuqoui
# -oneatx (prend bcp de taille)
# quand on mets les optimisations, on peux plus faire un shell, pq ?

# -zm: each function in a segment
# option el : eliminate no referenced segment

AOPTS =
LOPTS = option stack=60000 option c option el
LAOPTS =

!endif


#-----------------------------------------------------------------------#
#----------------- Contruction de tous les executables -----------------#
#-----------------------------------------------------------------------#

all : kk.exe kkmain.exe kksetup.exe kkdesc.exe kkkkrgr.exe kkfont.exe &
      idf.lib hard.lib readme.exe
#      edit.exe

kkmain.exe  : kkmain.obj     idf.lib  hard.lib    win.obj   gestion.obj &
                view.obj  driver.obj ficidf.obj copie.obj    delete.obj &
              search.obj   kkfct.obj  util.obj reddies.obj longname.obj &
               rbdos.obj
             $(CL) system $(SYSTEM) $(LOPTS)  &
                   option modname = 'Ketchup Killers Commander 0.84' &
                   name $@ file {$<}

kksetup.exe : kksetup.obj hard.lib idf.lib reddies.obj ficidf.obj &
              kkfct.obj rbdos.obj
             $(CL) system $(SYSTEM) $(LOPTS) name $@ file {$<}

kkdesc.exe  : kkdesc.obj idf.lib hard.lib
             $(CL) system $(SYSTEM) $(LOPTS) name $@ file {$<}

kkkkrgr.exe : kkkkrgr.obj hard.lib
             $(CL) system $(SYSTEM) $(LOPTS) name $@ file {$<}

kkfont.exe  : kkfont.obj hard.lib
             $(CL) system $(SYSTEM) $(LOPTS) name $@ file {$<}

edit.exe  : edit.obj hard.lib
             $(CL) system $(SYSTEM) $(LOPTS) name $@ file {$<}

kk.exe      : kk.obj
             $(CL) $(LAOPTS) name $@ file {$<}

readme.exe  : readme.obj
             $(CL) $(LAOPTS) name $@ file {$<}

.asm.obj:
         $(CA) $* $(AOPTS)
.c.obj:
         $(CC) $* $(CCOPTS)

idf.lib : idf.obj
         $(CB) -c -n $@ +-$<

hard.lib : hard.obj
         $(CB) -c -n $@ +-$<



#-----------------------------------------------------------------------#
#                                                                       #
#  This was the Ketchup Killers making file                             #
#                                                                       #
#          by RedBug/Ketchup Killers                                    #
#                                                                       #
# EMAIL: RedBug@pctrading.be                                            #
#-----------------------------------------------------------------------#
