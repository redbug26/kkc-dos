#-----------------------------------------------------------------------#
#                                                                       #
#                     Ketchup Killers making file                       #
#                                                                       #
#                      by RedBug/Ketchup Killers                        #
#                                                                       #
#-----------------------------------------------------------------------#

CA = wasm
CC = wcc386
CL = *wlink

SYSTEM = dos4g

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
#-----------------------------------------------------------------------#

#------------------------ En phase de debugging ------------------------#
#-----------------------------------------------------------------------#

CCOPTS = -wx -5r -fpi -zdp -d__WC32__ -mf -d3 -zm -od
AOPTS = /d1
LOPTS = option stack=60000 debug all
LAOPTS = debug all

#------------------------  En phase de release  ------------------------#
#-----------------------------------------------------------------------#
#CCOPTS = -w5 -5r -fpi -zdp -d__WC32__ -mf -s
# -oneatx
# quand on mets les optimisations, on peux plus faire un shell, pq ?
#AOPTS =
#LOPTS = option stack=60000
#LAOPTS =


#-----------------------------------------------------------------------#
#----------------- Contruction de tous les executables -----------------#
#-----------------------------------------------------------------------#

all : kk.exe kkmain.exe kksetup.exe kkdesc.exe kkkkrgr.exe kkfont.exe

kkmain.exe  : kkmain.obj    idf.obj   hard.obj   win.obj gestion.obj &
                 dos.obj driver.obj ficidf.obj copie.obj  delete.obj &
              search.obj   help.obj   view.obj
             $(CL) system $(SYSTEM) $(LOPTS) name $@ file {$<}

kksetup.exe : kksetup.obj hard.obj idf.obj help.obj
             $(CL) system $(SYSTEM) $(LOPTS) name $@ file {$<}

kkdesc.exe  : kkdesc.obj idf.obj hard.obj help.obj
             $(CL) system $(SYSTEM) $(LOPTS) name $@ file {$<}

kkkkrgr.exe : kkkkrgr.obj hard.obj
             $(CL) system $(SYSTEM) $(LOPTS) name $@ file {$<}

kkfont.exe  : kkfont.obj hard.obj
             $(CL) system $(SYSTEM) $(LOPTS) name $@ file {$<}

kk.exe      : kk.obj
             $(CL) $(LAOPTS) name $@ file {$<}


.asm.obj:
         $(CA) $* $(AOPTS)
.c.obj:
         $(CC) $* $(CCOPTS)

#-----------------------------------------------------------------------#
#                                                                       #
#  This was the Ketchup Killers making file                             #
#                                                                       #
#          by RedBug/Ketchup Killers                                    #
#                                                                       #
# EMAIL: RedBug@pctrading.be                                            #
#-----------------------------------------------------------------------#