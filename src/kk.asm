;-----------------------------------------------------------------------
;- Shell For Ketchup Killers Commander by RedBug/Ketchup^Pulpe         -
;-----------------------------------------------------------------------

.286

UseKey                 = 0

b equ byte ptr
w equ word ptr
d equ dword ptr
q equ qword ptr
s equ short
f equ far


;== Code ===============================================================

code segment para 'CODE'                  ;D‚finition du code de segment

assume cs:code, ds:code, ss:stackseg

exec proc far
    push cs
    pop ds

    call ComsPec
    call InitPath

    call setfree                          ;Lib‚rer la m‚moire inutilis‚e

    mov ax,3560h
    int 21h              ;--- renvoit es:bx ----------------------------
    mov w[int_00h],bx
    mov w[int_00h+2],es

    mov dx,offset et_int00h
    mov ax,2560h
    int 21h

    mov ax,offset sav_ecran_buf
    mov w[sav_ecran],ax
    mov w[sav_ecran+2],ds

    call kkmaind

    mov  dx,offset prgname                      ;Offset nom de programme
    mov  si,offset prgpara                ;Offset de ligne d'instruction
    call exeprg                                    ;Appeler le programme

boucle:

;--- Appel commande systeme --------------------------------------------
;-----------------------------------------------------------------------
    call System      ; Pr‚pare l'execution du programme voulu par KKMAIN
    cmp al,'#'
    je @case23
    cmp al,'@'
    je @case40
    jmp fin

@case23:
;--- Case pour appel normal d'un programme -----------------------------
    mov dx,offset prgname                       ;Offset nom de programme
    mov si,offset prgparas                ;Offset de ligne d'instruction
    call exeprg                                    ;Appeler le programme
    jmp @break


@case40:
;--- Case avec sauvegarde des touches ----------------------------------
    mov ax,3509h
    int 21h              ;--- renvoit es:bx ----------------------------
    mov w[int_09h],bx
    mov w[int_09h+2],es

    mov dx,offset et_int09h
    mov ax,2509h
    int 21h

    mov dx,offset prgname                       ;Offset nom de programme
    mov si,offset prgparas                ;Offset de ligne d'instruction
    call exeprg                                    ;Appeler le programme

    push ds
    mov dx,cs:w[int_09h]
    mov ds,cs:w[int_09h+2]
    mov ax,2509h
    int 21h              ;--- a besoin de ds:dx ------------------------
    pop ds

    jmp @break

;--- Rappel de KKMAIN --------------------------------------------------
;-----------------------------------------------------------------------
@break:
    call kkmain                ; Pr‚pare l'‚x‚cution de KKMAIN.EXE 69690

    mov dx,offset prgname                       ;Offset nom de programme
    mov si,offset prgpara                 ;Offset de ligne d'instruction
    call exeprg                                    ;Appeler le programme

    jmp boucle

;--- Fin du programme --------------------------------------------------
fin:
    mov dx,cs:w[int_00h]
    mov ds,cs:w[int_00h+2]
    mov ax,2560h
    int 21h              ;--- a besoin de ds:dx ------------------------

    mov ax,4C00h                        ; FIN programme par fonction DOS
    int 21h                          ;en transmettant le code d'erreur 0

exec endp


;-- SETFREE: Lib‚rer la place m‚moire inutilis‚e -----------------------
;-- Entr‚e  : ES = Adresse du PSP
;-- Sortie  : Aucune
;-- Registres : AX, BX, CL et FLAGS sont modifi‚s
;-- Infos   :   Comme le segment de pile est toujours le dernier segment
;--            dans un fichier EXE, ES:0000 d‚signe le d‚but et SS:SP la
;--                  fin du programme en m‚moire. C'est ainsi qu'on peut
;--                                    calculer la longueur du programme
;-----------------------------------------------------------------------
setfree proc near
    mov bx,ss             ;Calculer d'abord la diff‚rence entre les deux
    mov ax,es              ;adresses de segment. Cela donne le nombre de
    sub bx,ax                    ;paragraphes du PSP au d‚but de la pile

    mov ax,sp              ;Comme le pointeur de pile se trouve … la fin
    add ax,15                   ;du segment de pile, son contenu indique
    mov cl,4                                     ;la longueur de la pile
    shr ax,cl
    add bx,ax                          ;Ajouter … la longueur ant‚rieure

    mov ah,4ah                    ;Transmettre la nouvelle taille au DOS
    int 21h

    ret                                             ;Retour … l'appelant
setfree endp

;-- EXEPRG: Appeler un autre programme ---------------------------------
;-- Entr‚e  : DS:DX = Adresse du nom de programme
;--           DS:SI = Adresse de la ligne d'instruction
;-- Sortie  : Carry-Flag = 1 : Erreur (AX = code d'erreur)
;-- Registres :                          Seuls AX et FLAGS sont modifi‚s
;-- Infos     :    Le nom de programme et la ligne d'instruction doivent
;--                 figurer sous forme de chaŒnes ASCII termin‚es par le
;--                                                         code ASCII 0

exeprg           proc near

          ;-- Transmettre la ligne d'instruction sp‚cifi‚e -------------
          ;-- dans un buffer en comptant les caractŠres ----------------

    push bx                              ;Sauvegarder tous les registres
    push cx                                  ;d‚truits par l'appel d'une
    push dx                                           ;fonction EXEC DOS
    push di
    push si
    push bp
    push ds
    push es

    mov  di,offset comline+1              ;Car. dans ligne d'instruction
    push cs                                              ;CS sur la pile
    pop es                                  ;Retirer pour placer dans ES
    xor bl,bl                         ;Fixer le compteur de carac. sur 0
copypara:
    lodsb                                             ;Lire un caractŠre
    or al,al                                   ;Est-ce le code NUL (Fin)
    je fincopie                                     ;OUI --> assez copi‚
    stosb                            ;Sauvegarder dans le nouveau buffer
    inc bl                          ;Augmenter le compteur de caractŠres
    cmp bl,126                                         ;Maximum atteint?
    jne copypara                                      ;NON --> continuer

fincopie:
    mov cs:comline,bl               ;Sauvegarder le nombre de caractŠres
    mov byte ptr es:[di],13                ;Terminer ligne d'instruction

    mov cs:rangss,ss              ;SS et SP doivent ˆtre stock‚s dans le
    mov cs:rangsp,sp                                    ;segment de code

    mov bx,offset parblock          ;ES:BX d‚signe le bloc de paramŠtres

    mov ax,4B00h               ;Num‚ro de fonction pour la fonction EXEC
    int 21h                                     ;Appeler fonction du DOS

    cli                     ;D‚sactiver temporairement les interruptions
    mov ss,cs:rangss              ;R‚tablir anciennes valeurs segment de
    mov sp,cs:rangsp                        ;pile et du pointeur de pile
    sti                             ;Activer … nouveau les interruptions

    pop es                                   ;Retirer … nouveau tous les
    pop ds                                         ;registres de la pile
    pop bp
    pop si
    pop di
    pop dx
    pop cx
    pop bx

    jc exeend                                       ;Erreur? OUI --> Fin
    mov ah,4dh                    ;Pas d'erreur, Ex‚cuter le code de fin
    int 21h                                                ;du programme

exeend:
    ret                                             ;Retour … l'appelant

          ;-- Ces variables ne peuvent ˆtre appel‚s qu'… travers CS ----

rangss    dw ?                   ;Re‡oit SS pendant l'appel du programme
rangsp    dw ?                   ;Re‡oit SP pendant l'appel du programme

;parblock  equ this word       ;Bloc de paramŠtres pour la fonction EXEC

parblock  dw 0                                ;Mˆme bloc d'environnement
          dw offset comline ;Adresses d'offset et de segment de la ligne
          dw seg code                                         ;convertie
          dd 0                               ;Pas de donn‚es dans PSP #1
          dd 0                               ;Pas de donn‚es dans PSP #2

comline   db 256 dup (?)        ;Re‡oit la ligne d'instruction convertie

exeprg endp

;-----------------------------------------------------------------------
; copie la chaine que KKMAIN veut executer dans prgsys -----------------
; renvoit al: caractere de controle                                    -
;-----------------------------------------------------------------------
System proc near
    push es
    push ds

    push ds
    pop es

    push 0BB00h
    pop ds

    xor si,si              ;--- source ---------------------------------
    mov al,ds:[si]
    inc si

    mov di,offset prgsys   ;--- destination ----------------------------

    mov cx,256
    rep movsb

    pop ds
    pop es
    ret
System endp

;-----------------------------------------------------------------------
; Copie kkmainp dans prgtrue + [prgpos]                                -
;-----------------------------------------------------------------------
kkmain proc near
    pusha
    push es

    push ds
    pop es

    mov si,offset kkmainp
    mov di,offset prgtrue
    add di,[prgpos]
    mov cx,256
    sub cx,[prgpos]
    rep movsb

    pop es
    popa
    ret
kkmain endp

;-----------------------------------------------------------------------
; Copie kkmainp1 dans prgtrue + [prgpos]                               -
;-----------------------------------------------------------------------
kkmaind proc near
    pusha
    push es

    push ds
    pop es

    mov si,offset kkmainp1
    mov di,offset prgtrue
    add di,[prgpos]
    mov cx,256
    sub cx,[prgpos]
    rep movsb

    pop es
    popa
    ret
kkmaind endp


;-----------------------------------------------------------------------
;-----------------------------------------------------------------------
ComsPec proc near
    pusha
    push es
    push ds

    mov ax,cs
    mov ds,ax

    mov ah,62h
    int 21h
    mov es,bx                                       ; get segment of PSP

    mov si,2Ch
    mov ax,es:[si]                          ; get segment of environment

    mov es,ax
    xor ax,ax
    xor di,di

Com0:
    mov si,offset scomspec
Com1:
    lodsb                                                     ; al=ds:si
    cmp al,0
    je Com4
    scasb
    je Com1
Com2:
    mov al,0
    mov cx,1024
    repnz scasb
    jmp Com0

Com4:
    push ds

    push di
    pop si

    push es
    pop ds

    pop es

    mov di,offset prgname

    mov cx,256
    rep movsb                                              ; es:di=ds:si

    push es
    pop ds

    pop ds
    pop es
    popa
    ret
ComsPec endp


;-----------------------------------------------------------------------
;-----------------------------------------------------------------------
InitPath proc near
    pusha
    push es
    push ds

    mov ah,62h
    int 21h
    mov es,bx                                       ; get segment of PSP

    mov si,2Ch
    mov ax,es:[si]                     ; adresse de la ligne de commande
    push ax

    mov es,ax
    xor ax,ax
    mov bx,ax
    mov di,ax
    mov cx,7FFFh
    cld
oui:
    repnz scasb
    inc bx
    cmp es:[di],al
    jnz oui

    or ch,80h               ; marjorie
    neg cx

    add cx,2

    push ds
    pop es
    mov di,offset prgtrue

    pop ds
    mov si,cx

    mov cx,256
    rep movsb

    mov di,offset prgtrue
    xor ax,ax
    mov cx,7FFFh
    repnz scasb

    sub di,2

    std
    mov al,'\'
    mov cx,7FFFh
    repnz scasb                                                  ; es:di
    inc di
    inc di

    mov es:byte ptr[di],0                     ; prgtrue contient la path
    sub di,offset prgtrue
    mov es:[prgpos],di

    cld

    pop ds
    pop es
    popa
    ret
InitPath endp


;=== Gestion des touches ===============================================
et_int09h:
    push ax
    push bx
    push cx
    push dx
    push di
    push si
    push es
    push ds
    pushf

    mov es,cs:[sav_ecran+2]
    mov di,cs:[sav_ecran]
    in al,60h
    add di,cs:[taille_buf]

    inc cs:word ptr[taille_buf]

    mov es:byte ptr[di],al

    cmp cs:word ptr[taille_buf],1995 ;--Taille buffer > espace libre ?--
    jne continue
    mov cs:word ptr[taille_buf],0  ; Buffer=0

continue:
    popf
    pop ds
    pop es
    pop si
    pop di
    pop dx
    pop cx
    pop bx
    pop ax

    jmp cs:dword ptr[int_09h]


et_int00h:
    jmp cs:dword ptr[int_00h]

sav_ecran      dw 0,0
taillemax      dw 2048
taille_buf     dw 0
sav_ecran_buf  db 2048 dup (0FFh)

;== Donn‚es ============================================================


; gestion touche
int_09h   dw 0,0
int_00h   dw 0,0

; fin gestion touche

prgname   db 256 dup (0)                    ; Nom du programme … appeler
prgpara   db "/c "
prgtrue   db 256 dup (0)           ; Les paramŠtres sont transmis au Prg
                                                       ; toujours KKMAIN

prgpos    dw 0                                     ; longueur de la path

prgparas  db "/c "
prgsys    db 256 dup (0)

kkmainp   db "kkmain 69690",0
kkmainp1  db "kkmain 6969",0

Scomspec  db "COMSPEC=",0


;-----------------------------------------------------------------------
; Affiche une chaine                                                   -
; affiche DX                                                           -
;-----------------------------------------------------------------------
Displaye proc near
    pusha
    push ds
    push es

    pusha

    mov ah,40h
    mov bx,1
    mov cx,320

    int 21H

    mov bx,dx

    push es
    pop ds

    popa

    mov ax,[bx+4]
    mov es,ax
    mov dx,[bx+2]
    inc dx

    push es
    pop ds

    mov ah,40h
    mov bx,1
    mov cx,80h
    int 21h

    xor ax,ax
    int 16h

    pop es
    pop ds
    popa
    ret
Displaye endp

;== Stack ==============================================================

          ;--- Le segment de pile est … placer ici … la fin du fichier -
          ;--- pour qu'il se trouve derriŠre le segment de donn‚es -----
          ;--- et de code aprŠs chargement du programme ----------------

stackseg  segment para stack 'STACK'      ;D‚finition du segment de pile

    dw 256 dup (?)                 ;La taille de la pile est de 256 mots

stackseg  ends                                   ;Fin du segment de pile

;== Fin ================================================================

code ends                                        ;Fin du segment de code

end  exec                                  ;Commencer l'ex‚cution … EXEC
