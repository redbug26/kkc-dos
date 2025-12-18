#!/bin/bash
# Script pour déboguer kkc avec lldb

cd "$(dirname "$0")"

# Compile d'abord
make

echo "==================================================================="
echo "Débogage de kkc avec lldb"
echo "==================================================================="
echo ""
echo "Commandes lldb utiles :"
echo "  r          - Lance l'application (run)"
echo "  c          - Continue après un crash (continue)"
echo "  bt         - Affiche la pile d'appels (backtrace)"
echo "  frame info - Info sur la frame courante"
echo "  p variable - Affiche la valeur d'une variable (print)"
echo "  q          - Quitte lldb (quit)"
echo ""
echo "==================================================================="
echo ""

lldb ./kkc
