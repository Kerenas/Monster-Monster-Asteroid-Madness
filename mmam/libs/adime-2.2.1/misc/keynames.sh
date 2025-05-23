sed -n -e "s/.define.*KEY_\([A-Z0-9_]*\)[ ]*\([0-9]*\)/\2 \1/p" \
 < keyboard.h \
 | sort -g \
 | sed -n -e "s/[0-9]* \([A-Z0-9_]*\)/   { KEY_\1, \"\1\", \"\1\" },/p" \
 > keynames.tmp
