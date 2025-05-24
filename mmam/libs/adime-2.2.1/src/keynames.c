/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   keynames.c:
   Functions for converting between Allegro scancodes and strings.

   By Sven Sandberg

   See readme.txt for more information about Adime.
*/
#include <string.h>
#include <allegro.h>

#include "adime.h"



/* Internal structure for associating scancodes to names. */
typedef struct KEY_NAME
{
   int scancode;
   char *short_name, *pretty_name;
} KEY_NAME;



/* The names were extracted from allegro.h with the following command line:

   sed -n -e "s/.*KEY_\([A-Z0-9_]*\).* /   { KEY_\1, \"\1\", \"\1\" },/p"

   This could have been used if I wanted the numbers sorted:
   sed -n -e "s/.define.*KEY_\([A-Z0-9_]*\)[ ]*\([0-9]*\)/\2 \1/p"
      < allegro.h
      | sort -g
      | sed -n -e "s/[0-9]* \([A-Z0-9_]*\)/   { KEY_\1, \"\1\", \"\1\" },/p"

   Some of the pretty names needed to be modified afterwards.
*/
static KEY_NAME adime_key_name[] =
{
   { KEY_A, "A", "A" },
   { KEY_B, "B", "B" },
   { KEY_C, "C", "C" },
   { KEY_D, "D", "D" },
   { KEY_E, "E", "E" },
   { KEY_F, "F", "F" },
   { KEY_G, "G", "G" },
   { KEY_H, "H", "H" },
   { KEY_I, "I", "I" },
   { KEY_J, "J", "J" },
   { KEY_K, "K", "K" },
   { KEY_L, "L", "L" },
   { KEY_M, "M", "M" },
   { KEY_N, "N", "N" },
   { KEY_O, "O", "O" },
   { KEY_P, "P", "P" },
   { KEY_Q, "Q", "Q" },
   { KEY_R, "R", "R" },
   { KEY_S, "S", "S" },
   { KEY_T, "T", "T" },
   { KEY_U, "U", "U" },
   { KEY_V, "V", "V" },
   { KEY_W, "W", "W" },
   { KEY_X, "X", "X" },
   { KEY_Y, "Y", "Y" },
   { KEY_Z, "Z", "Z" },
   { KEY_0, "0", "0" },
   { KEY_1, "1", "1" },
   { KEY_2, "2", "2" },
   { KEY_3, "3", "3" },
   { KEY_4, "4", "4" },
   { KEY_5, "5", "5" },
   { KEY_6, "6", "6" },
   { KEY_7, "7", "7" },
   { KEY_8, "8", "8" },
   { KEY_9, "9", "9" },
   { KEY_0_PAD, "0_PAD", "NUMERIC 0" },
   { KEY_1_PAD, "1_PAD", "NUMERIC 1" },
   { KEY_2_PAD, "2_PAD", "NUMERIC 2" },
   { KEY_3_PAD, "3_PAD", "NUMERIC 3" },
   { KEY_4_PAD, "4_PAD", "NUMERIC 4" },
   { KEY_5_PAD, "5_PAD", "NUMERIC 5" },
   { KEY_6_PAD, "6_PAD", "NUMERIC 6" },
   { KEY_7_PAD, "7_PAD", "NUMERIC 7" },
   { KEY_8_PAD, "8_PAD", "NUMERIC 8" },
   { KEY_9_PAD, "9_PAD", "NUMERIC 9" },
   { KEY_F1, "F1", "F1" },
   { KEY_F2, "F2", "F2" },
   { KEY_F3, "F3", "F3" },
   { KEY_F4, "F4", "F4" },
   { KEY_F5, "F5", "F5" },
   { KEY_F6, "F6", "F6" },
   { KEY_F7, "F7", "F7" },
   { KEY_F8, "F8", "F8" },
   { KEY_F9, "F9", "F9" },
   { KEY_F10, "F10", "F10" },
   { KEY_F11, "F11", "F11" },
   { KEY_F12, "F12", "F12" },
   { KEY_ESC, "ESC", "ESC" },
   { KEY_TILDE, "TILDE", "TILDE" },
   { KEY_MINUS, "MINUS", "MINUS" },
   { KEY_EQUALS, "EQUALS", "EQUALS" },
   { KEY_BACKSPACE, "BACKSPACE", "BACKSPACE" },
   { KEY_TAB, "TAB", "TAB" },
   { KEY_OPENBRACE, "OPENBRACE", "OPENBRACE" },
   { KEY_CLOSEBRACE, "CLOSEBRACE", "CLOSEBRACE" },
   { KEY_ENTER, "ENTER", "ENTER" },
   { KEY_COLON, "COLON", "COLON" },
   { KEY_QUOTE, "QUOTE", "QUOTE" },
   { KEY_BACKSLASH, "BACKSLASH", "BACKSLASH" },
   { KEY_BACKSLASH2, "BACKSLASH2", "BACKSLASH 2" },
   { KEY_COMMA, "COMMA", "COMMA" },
   { KEY_STOP, "STOP", "STOP" },
   { KEY_SLASH, "SLASH", "SLASH" },
   { KEY_SPACE, "SPACE", "SPACE" },
   { KEY_INSERT, "INSERT", "INSERT" },
   { KEY_DEL, "DEL", "DEL" },
   { KEY_HOME, "HOME", "HOME" },
   { KEY_END, "END", "END" },
   { KEY_PGUP, "PGUP", "PAGE UP" },
   { KEY_PGDN, "PGDN", "PAGE DOWN" },
   { KEY_LEFT, "LEFT", "LEFT" },
   { KEY_RIGHT, "RIGHT", "RIGHT" },
   { KEY_UP, "UP", "UP" },
   { KEY_DOWN, "DOWN", "DOWN" },
   { KEY_SLASH_PAD, "SLASH_PAD", "NUMERIC SLASH" },
   { KEY_ASTERISK, "ASTERISK", "ASTERISK" },
   { KEY_MINUS_PAD, "MINUS_PAD", "NUMERIC MINUS" },
   { KEY_PLUS_PAD, "PLUS_PAD", "NUMERIC PLUS" },
   { KEY_DEL_PAD, "DEL_PAD", "NUMERIC DEL" },
   { KEY_ENTER_PAD, "ENTER_PAD", "NUMERIC ENTER" },
   { KEY_PRTSCR, "PRTSCR", "PRINT SCREEN" },
   { KEY_PAUSE, "PAUSE", "PAUSE" },
   { KEY_ABNT_C1, "ABNT_C1", "ABNT_C1" },
   { KEY_YEN, "YEN", "YEN" },
   { KEY_KANA, "KANA", "KANA" },
   { KEY_CONVERT, "CONVERT", "CONVERT" },
   { KEY_NOCONVERT, "NOCONVERT", "NOCONVERT" },
   { KEY_AT, "AT", "AT" },
   { KEY_CIRCUMFLEX, "CIRCUMFLEX", "CIRCUMFLEX" },
   { KEY_COLON2, "COLON2", "COLON 2" },
   { KEY_KANJI, "KANJI", "KANJI" },
   { KEY_LSHIFT, "LSHIFT", "LEFT SHIFT" },
   { KEY_MODIFIERS, "MODIFIERS", "MODIFIERS" },
   { KEY_RSHIFT, "RSHIFT", "RIGHT SHIFT" },
   { KEY_LCONTROL, "LCONTROL", "LEFT CONTROL" },
   { KEY_RCONTROL, "RCONTROL", "RIGHT CONTROL" },
   { KEY_ALT, "ALT", "ALT" },
   { KEY_ALTGR, "ALTGR", "ALT GR" },
   { KEY_LWIN, "LWIN", "LEFT WIN" },
   { KEY_RWIN, "RWIN", "RIGHT WIN" },
   { KEY_MENU, "MENU", "MENU" },
   { KEY_SCRLOCK, "SCRLOCK", "SCROLL LOCK" },
   { KEY_NUMLOCK, "NUMLOCK", "NUM LOCK" },
   { KEY_CAPSLOCK, "CAPSLOCK", "CAPS LOCK" },
   { KEY_MAX, "MAX", "MAX" },
   { 0, NULL, NULL }
};



static KEY_NAME adime_flag_name[] =
{
   { KB_SHIFT_FLAG, "SHIFT", "SHIFT" },
   { KB_CTRL_FLAG, "CTRL", "CONTROL" },
   { KB_ALT_FLAG, "ALT", "ALT" },
   { KB_LWIN_FLAG, "LWIN", "LEFT WINDOW" },
   { KB_RWIN_FLAG, "RWIN", "RIGHT WINDOW" },
   { KB_MENU_FLAG, "MENU", "MENU" },
   { KB_SCROLOCK_FLAG, "SCROLOCK", "SCROLL LOCK" },
   { KB_NUMLOCK_FLAG, "NUMLOCK", "NUM LOCK" },
   { KB_CAPSLOCK_FLAG, "CAPSLOCK", "CAPSLOCK" },
   { KB_INALTSEQ_FLAG, "INALTSEQ", "IN ALT SEQUENCE" },
   { KB_ACCENT1_FLAG, "ACCENT1", "ACCENT 1" },
   { KB_ACCENT2_FLAG, "ACCENT2", "ACCENT 2" },
   { KB_ACCENT3_FLAG, "ACCENT3", "ACCENT 3" },
   { KB_ACCENT4_FLAG, "ACCENT4", "ACCENT 4" },
   { 0, NULL, NULL }
};



static char *number_to_name(KEY_NAME *table, int scancode, char *buf,
                            int pretty)
{
   char *temp;
   int i;
   char *name;

   for (i = 0; table[i].short_name != NULL; i++) {
      if (table[i].scancode == scancode) {
         if (pretty)
            name = table[i].pretty_name;
         else
            name = table[i].short_name;
         temp = uconvert(name, U_ASCII, buf, U_CURRENT, 1000);
         if (temp == name)
            temp = ustrcpy(buf, temp);
         return temp;
      }
   }

   return NULL;
}



/* adime_scancode_to_short_name:
   Given an Allegro scancode, returns a short name for the key. The short
   name is the same as the Allegro macro name except the KEY_ prefix. Left
   shift is for example "LSHIFT".
*/
char *adime_scancode_to_short_name(int scancode, char *buf)
{
   return number_to_name(adime_key_name, scancode, buf, FALSE);
}



/* adime_scancode_to_pretty_name:
   Given an Allegro scancode, returns a descriptive name for the key. For
   example, the descriptive name for left shift is "LEFT SHIFT" rather than
   "LSHIFT", which is its short name (because its Allegro scancode is called
   KEY_LSHIFT).
*/
char *adime_scancode_to_pretty_name(int scancode, char *buf)
{
   return number_to_name(adime_key_name, scancode, buf, TRUE);
}



/* adime_short_name_to_scancode:
   Given a short name of a key as ascii, returns the corresponding Allegro
   scancode, or -1 if the name wasn't found.
*/
int adime_short_name_to_scancode(ADIME_CONST char *short_name)
{
   char buf[64];
   int i;

   uconvert_toascii(short_name, buf);
   for (i = 0; adime_key_name[i].short_name; i++)
      if (strcmp(adime_key_name[i].short_name, short_name) == 0)
         return adime_key_name[i].scancode;

   return -1;
}



/* adime_keyflag_to_pretty_name:
   Given an Allegro shift flag (one of the KB_* flags), returns a descriptive
   name for the key flag.
*/
char *adime_keyflag_to_pretty_name(int scancode, char *buf)
{
   return number_to_name(adime_flag_name, scancode, buf, TRUE);
}



/* adime_keyflag_to_short_name:
   Given an Allegro shift flag (one of the KB_* flags), returns a short name
   for the key flag.
*/
char *adime_keyflag_to_short_name(int scancode, char *buf)
{
   return number_to_name(adime_flag_name, scancode, buf, FALSE);
}



/* adime_short_name_to_keyflag:
   Given a short name of a key as ascii, returns the corresponding Allegro
   scancode, or -1 if the name wasn't found.
*/
int adime_short_name_to_keyflag(ADIME_CONST char *short_name)
{
   char buf[64];
   int i;

   uconvert_toascii(short_name, buf);
   for (i = 0; adime_flag_name[i].short_name; i++)
      if (strcmp(adime_flag_name[i].short_name, short_name) == 0)
         return adime_flag_name[i].scancode;

   return -1;
}
