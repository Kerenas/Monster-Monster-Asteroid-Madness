/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   adime.h:
   External declarations for the Adime library.

   By Sven Sandberg
*/

#ifndef _ADIME_INCLUDE_ADIME_H_
#define _ADIME_INCLUDE_ADIME_H_

/* For `INT_MIN'. */
#include <limits.h>
/* For `struct FONT'. */
#include <allegro.h>
/* For `va_list'. */
#include <stdarg.h>

#ifdef __cplusplus
   extern "C" {
#endif

/* note: version number and date are patched by fixver.sh */
#define ADIME_VERSION_MAJOR      2
#define ADIME_VERSION_MINOR      2
#define ADIME_VERSION_PATCH      1
#define ADIME_DATE               20050806    /* yyyymmdd */


/* These are really useful for debugging! */
#ifdef _ADIME_DEVELOPING
   #define _TR(x, c) (al_trace("%s=%" c "  @%s.%d\n", #x, x, __FILE__, __LINE__))
   #define Q      (al_trace("%s.%d\n", __FILE__, __LINE__))
   #define I(x)   _TR(x, "d")
   #define P(x)   _TR(x, "p")
   #define R(x)   (al_trace("%s=(%d.%d.%d)  @%s.%d\n", #x, (x).r, (x).g, (x).b, __FILE__, __LINE__))
   #define S(x)   _TR(x, "s")
#endif
/* The memory debugger is very helpful in emergency cases. */
#ifdef _ADIME_FORTIFY
   #undef FORTIFY
   #define FORTIFY
   #include "fortify2.h"
#endif


#include "adime/adimecfg.h"



/******************************************/
/************ Dialog Functions ************/
/******************************************/

/* Because of weirdnesses in the C language, we have to wrap all `va_list's
   in a struct before using them.
*/
typedef struct {
   va_list ap;
} *adime_va_list;
#define adime_va_start(_ap, format) \
   ( \
      (_ap) = malloc(sizeof(adime_va_list)), \
      (_ap) ? (void)0 : exit(1), \
      va_start(_ap->ap, format) \
   )
#define adime_va_arg(_ap, TYPE) \
   ( \
      va_arg(_ap->ap, TYPE) \
   )
#define adime_va_end(_ap) \
   ( \
      free(_ap) \
   )

/* Setup and setdown. */
ADIME_FUNC(int, adime_init, (void));
ADIME_FUNC(void, adime_exit, (void));

/* Input dialogs. */
ADIME_FUNC(int, adime_dialogf, (ADIME_CONST char *title, int x, int y, int edit_w, ADIME_CONST char *format, ...));
ADIME_FUNC(int, adime_vdialogf, (ADIME_CONST char *title, int x, int y, int edit_w, ADIME_CONST char *format, adime_va_list args));
ADIME_FUNC(int, adime_lowlevel_dialogf, (ADIME_CONST char *title, int x, int y, int edit_w, ADIME_CONST char *format, ...));
ADIME_FUNC(int, adime_lowlevel_vdialogf, (ADIME_CONST char *title, int x, int y, int edit_w, ADIME_CONST char *format, adime_va_list args));

/* Message boxes. */
#if 0 /* This is not finished yet. */
#define ADIME_MESSAGE_BOX_MAX_TEXT_LENGTH (8192)
ADIME_FUNC(int, adime_message_box, (ADIME_CONST char *title, ADIME_CONST char *buttons, ADIME_CONST char *format, ...));
ADIME_FUNC(int, adime_vmessage_box, (ADIME_CONST char *title, ADIME_CONST char *buttons, ADIME_CONST char *format, adime_va_list args));
#endif /* if 0 */

/* Variations of Allegro's dialog functions. */
ADIME_FUNC(int, adime_do_dialog_find_focus_object, (DIALOG *dialog, int focus_obj));
ADIME_FUNC(DIALOG_PLAYER *, adime_init_dialog_find_focus_object, (DIALOG *dialog, int focus_obj));

/* Dialog appearance. */
ADIME_VAR(RGB, adime_text_rgb);
ADIME_VAR(RGB, adime_disabled_text_rgb);
ADIME_VAR(RGB, adime_error_text_rgb);
ADIME_VAR(RGB, adime_border_rgb);
ADIME_VAR(RGB, adime_background_rgb);
ADIME_VAR(RGB, adime_edit_field_rgb);
ADIME_VAR(RGB, adime_button_rgb);
ADIME_VAR(RGB, adime_dark_shadow_rgb);
ADIME_VAR(RGB, adime_shadow_rgb);
ADIME_VAR(RGB, adime_highlight_rgb);
ADIME_VAR(RGB, adime_light_highlight_rgb);
ADIME_VAR(RGB, adime_title_text_rgb);
ADIME_VAR(RGB, adime_title_background_rgb);
ADIME_VAR(RGB, adime_title_shadow_rgb);
ADIME_VAR(RGB, adime_title_highlight_rgb);

ADIME_VAR(BITMAP *, adime_bmp);
ADIME_VAR(FONT *, adime_font);
ADIME_VAR(FONT *, adime_title_font);
ADIME_VAR(FONT *, adime_button_font);

ADIME_VAR(int, adime_yield);
ADIME_VAR(int, adime_clean_screen);
ADIME_FUNCPTR(void, adime_callback, (DIALOG *d));
ADIME_VAR(int, adime_window_visible);

ADIME_VAR(int, adime_window_border_thickness);
ADIME_VAR(int, adime_window_title_spacing);
ADIME_VAR(int, adime_window_title_border_thickness);
ADIME_VAR(int, adime_window_title_internal_border_thickness);
ADIME_VAR(int, adime_window_button_spacing);
ADIME_VAR(int, adime_window_between_button_spacing);
ADIME_VAR(int, adime_window_description_spacing);
ADIME_VAR(int, adime_window_line_spacing);
ADIME_VAR(int, adime_window_button_w);
ADIME_VAR(int, adime_window_button_h);



/****************************************/
/************ Misc Functions ************/
/****************************************/

/* Helper functions for evaluating expressions. */
ADIME_FUNC(double, adime_uevaluate, (ADIME_CONST char *equation, int *error));
ADIME_FUNC(double, adime_evaluate, (ADIME_CONST char *equation, int *error, ADIME_METHOD(double, variable, (ADIME_CONST char *name))));

/* Helper functions for converting between Allegro scancodes and strings. */
ADIME_FUNC(char *, adime_scancode_to_short_name, (int scancode, char *buf));
ADIME_FUNC(int, adime_short_name_to_scancode, (ADIME_CONST char *short_name));
ADIME_FUNC(char *, adime_scancode_to_pretty_name, (int scancode, char *buf));
ADIME_FUNC(char *, adime_keyflag_to_pretty_name, (int scancode, char *buf));
ADIME_FUNC(char *, adime_keyflag_to_short_name, (int scancode, char *buf));
ADIME_FUNC(int, adime_short_name_to_keyflag, (ADIME_CONST char *short_name));

/* Helper function for drawing objects. */
ADIME_FUNC(void, adime_fill_textout, (BITMAP *bmp, ADIME_CONST FONT *f, ADIME_CONST char *text, int x, int y, int w, int fg, int bg));

/* Helper functions for printing floating point numbers. */
ADIME_FUNC(void, adime_double2string, (double x, char *buf));
ADIME_FUNC(void, adime_udouble2string, (double val, char *buf));

/* Helper functions returning the pixel width of single characters and of
   substrings. */
ADIME_FUNC(int, adime_char_width, (ADIME_CONST FONT *f, int c));
ADIME_FUNC(int, adime_substr_width, (ADIME_CONST FONT *f, ADIME_CONST char *text, int start, int length));
/* Helper function to draw only part of a string. */
ADIME_FUNC(void, adime_draw_substr, (BITMAP *bmp, ADIME_CONST FONT *f, ADIME_CONST char *text, int x, int y, int color, int start, int length));

/* Alignment constants. */
#define ADIME_ALIGN_LEFT                (INT_MIN)
#define ADIME_ALIGN_LEFT_CENTER         (INT_MIN + 1)
#define ADIME_ALIGN_LEFT_RIGHT          (INT_MIN + 2)
#define ADIME_ALIGN_CENTER              (INT_MIN + 3)
#define ADIME_ALIGN_CENTER_LEFT         (INT_MIN + 4)
#define ADIME_ALIGN_CENTER_RIGHT        (INT_MIN + 5)
#define ADIME_ALIGN_RIGHT_LEFT          (INT_MIN + 6)
#define ADIME_ALIGN_RIGHT_CENTER        (INT_MIN + 7)
#define ADIME_ALIGN_RIGHT               (INT_MIN + 8)
#define ADIME_ALIGN_WORDJUSTIFY         (INT_MIN + 9)
#define ADIME_ALIGN_WORDJUSTIFY_LEFT    (INT_MIN + 10)
#define ADIME_ALIGN_WORDJUSTIFY_CENTER  (INT_MIN + 11)
#define ADIME_ALIGN_WORDJUSTIFY_RIGHT   (INT_MIN + 12)
#define ADIME_ALIGN_CHARJUSTIFY         (INT_MIN + 13)
#define ADIME_ALIGN_CHARJUSTIFY_LEFT    (INT_MIN + 14)
#define ADIME_ALIGN_CHARJUSTIFY_CENTER  (INT_MIN + 15)
#define ADIME_ALIGN_CHARJUSTIFY_RIGHT   (INT_MIN + 16)
#define ADIME_ALIGN_TOP                 (INT_MIN + 17)
#define ADIME_ALIGN_BOTTOM              (INT_MIN + 18)
#define ADIME_ALIGN_JUSTIFY             (ADIME_ALIGN_WORDJUSTIFY)

#define ADIME_ALIGN_CENTRE              (ADIME_ALIGN_CENTER)
#define ADIME_ALIGN_CENTRE_LEFT         (ADIME_ALIGN_CENTER_LEFT)
#define ADIME_ALIGN_CENTRE_RIGHT        (ADIME_ALIGN_CENTER_RIGHT)
#define ADIME_ALIGN_LEFT_CENTRE         (ADIME_ALIGN_LEFT_CENTER)
#define ADIME_ALIGN_RIGHT_CENTRE        (ADIME_ALIGN_RIGHT_CENTER)
#define ADIME_ALIGN_WORDJUSTIFY_CENTRE  (ADIME_ALIGN_WORDJUSTIFY_CENTER)
#define ADIME_ALIGN_CHARJUSTIFY_CENTRE  (ADIME_ALIGN_CHARJUSTIFY_CENTER)

#define ADIME_REDRAW                    (INT_MIN)



/***************************************/
/************ Gui Functions ************/
/***************************************/

/* Button drawing. */
ADIME_FUNC(void, adime_draw_empty_button_frame, (BITMAP *bmp, int x1, int y1, int x2, int y2, int face, int xlight, int light, int dark, int xdark));
ADIME_FUNC(void, adime_draw_empty_button, (BITMAP *bmp, int x1, int y1, int x2, int y2, int face, int xlight, int light, int dark, int xdark));
ADIME_FUNC(void, adime_draw_text_button, (BITMAP *bmp, int x1, int y1, int x2, int y2, int face, int textcolor, int xlight, int light, int dark, int xdark, ADIME_CONST FONT *f, ADIME_CONST char *text));
ADIME_FUNC(void, adime_draw_text_button_down, (BITMAP *bmp, int x1, int y1, int x2, int y2, int face, int textcolor, int xlight, int light, int dark, int xdark, ADIME_CONST FONT *f, ADIME_CONST char *text));
ADIME_FUNC(void, adime_draw_picture_button, (BITMAP *bmp, int x1, int y1, int x2, int y2, int face, int xlight, int light, int dark, int xdark, BITMAP *sprite));
ADIME_FUNC(void, adime_draw_picture_button_down, (BITMAP *bmp, int x1, int y1, int x2, int y2, int face, int xlight, int light, int dark, int xdark, BITMAP *sprite));
/* Gui procs. */
ADIME_FUNC(int, adime_d_line_proc, (int msg, DIALOG *d, int c));
ADIME_FUNC(int, adime_d_multiline_text_proc, (int msg, DIALOG *d, int c));
ADIME_FUNC(int, adime_d_button_proc, (int msg, DIALOG *d, int c));
ADIME_FUNC(int, adime_d_edit_proc, (int msg, DIALOG *d, int c));
ADIME_FUNC(int, adime_d_text_list_proc, (int msg, DIALOG *d, int c));
ADIME_FUNC(int, adime_d_list_proc, (int msg, DIALOG *d, int c));
ADIME_FUNC(int, adime_d_check_proc, (int msg, DIALOG *d, int c));
ADIME_FUNC(int, adime_d_greyable_check_proc, (int msg, DIALOG *d, int c));
ADIME_FUNC(int, adime_d_double_calc_edit_proc, (int msg, DIALOG *d, int c));
ADIME_FUNC(int, adime_d_int_calc_edit_proc, (int msg, DIALOG *d, int c));
ADIME_FUNC(int, adime_d_calc_edit_result_proc, (int msg, DIALOG *d, int c));
ADIME_FUNC(int, adime_d_window_proc, (int msg, DIALOG *d, int c));
/* 3d file selector. */
ADIME_FUNC(int, adime_file_select, (ADIME_CONST char *message, char *path, ADIME_CONST char *ext, int size, int w, int h));


typedef struct ADIME_EDIT_NUMBER
{
   int is_signed;          /* Set for integer formats if it is signed. */
   int is_float;           /* Set if we only have float, not double. */
   double min_val, max_val;/* Range for numbers. */
   DIALOG *result_object;  /* DIALOG object to display result in. */
} ADIME_EDIT_NUMBER;

/* If this flag is set for an adime_d_edit_proc, the border will be drawn
   outside the object. */
#define ADIME_D_EXTERNAL_BORDER   (D_USER << 4)

#define ADIME_BUTTON_TOGGLE    (1)
#define ADIME_BUTTON_EXIT      (2)
#define ADIME_BUTTON_CALLBACK  (3)

#define _ADIME_MSG_PRESS  (MSG_USER + AL_ID('A','D','I','M'))



/*******************************************/
/************ dialogf() Formats ************/
/*******************************************/

/* Internal structure for each format type. */
typedef struct _ADIME_DIALOGF_FORMAT
{
   /* The format string to identify us with. */
   char *specifier;
   /* If set, the object prefers to take care of placing the description
      itself, rather than letting the dialog handler do it. Note that if this
      is set, then `dialog_count()' must also count the number of objects
      needed for the description. */
   int handle_desc;
   /* Returns the number of dialog objects to allocate. */
   ADIME_METHOD(int, count_dialog, (ADIME_CONST char *desc, ADIME_CONST char *arg_text, adime_va_list args, void **saved_args));
   /* Creates the DIALOG objects. */
   ADIME_METHOD(int, create_dialog, (DIALOG *dialog, ADIME_CONST char *desc, ADIME_CONST char *arg_text, void *args));
   /* Stores the results in the parameters. */
   ADIME_METHOD(void, store_dialog, (DIALOG *object));
   /* Resets the dialog to its default values. */
   ADIME_METHOD(void, reset_dialog, (DIALOG *object));
   /* Frees memory. */
   ADIME_METHOD(void, destroy_dialog, (DIALOG *object));

   struct _ADIME_DIALOGF_FORMAT *next;
} _ADIME_DIALOGF_FORMAT;

ADIME_ARRAY(_ADIME_DIALOGF_FORMAT, _adime_dialogf_format_list);


/* Macros for listing all formats. */
#define ADIME_BEGIN_FORMAT_LIST \
   _ADIME_DIALOGF_FORMAT _adime_dialogf_format_list[] = \
   {

#define ADIME_END_FORMAT_LIST \
      {  NULL, FALSE, NULL, NULL, \
         NULL, NULL, NULL, NULL } \
   };

#define ADIME_FORMAT_BOOL \
   {  "bool", FALSE, \
      _adime_count_pointer_and_return_1, _adime_create_bool, \
      _adime_store_bool, _adime_reset_bool, NULL, NULL },
#define ADIME_FORMAT_GREYBOOL \
   {  "greybool", FALSE, \
      _adime_count_pointer_and_return_1, _adime_create_greybool, \
      _adime_store_greybool, _adime_reset_greybool, NULL, NULL },
#define ADIME_FORMAT_BUTTON \
   {  "button", FALSE, \
      _adime_count_pointer_and_return_1, _adime_create_button, \
      NULL, NULL, NULL, NULL },
#define ADIME_FORMAT_BUTTONROW \
   {  "buttonrow", FALSE, \
      _adime_count_buttonrow, _adime_create_buttonrow, \
      NULL, NULL, _adime_destroy_buttonrow, NULL },
#define ADIME_FORMAT_STRING \
   {  "string", FALSE, \
      _adime_count_pointer_and_return_1, _adime_create_string, \
      _adime_store_string, _adime_reset_string, _adime_destroy_dp, \
      NULL },
#define ADIME_FORMAT_FILENAME \
   {  "filename", FALSE, \
      _adime_count_pointer_and_return_2, _adime_create_filename, \
      _adime_store_filename, \
      _adime_reset_filename, _adime_destroy_filename, NULL },
#define ADIME_FORMAT_VLIST \
   {  "vlist", FALSE, \
      _adime_count_vlist, _adime_create_vlist, \
      _adime_store_lists, _adime_reset_lists, \
      _adime_destroy_vlist, NULL },
#define ADIME_FORMAT_LIST \
   {  "list", FALSE, \
      _adime_count_pointer_and_return_1, _adime_create_list, \
      _adime_store_lists, _adime_reset_lists, \
      _adime_destroy_list_and_datafile, NULL },
#define ADIME_FORMAT_DATAFILE \
   {  "datafile", FALSE, \
      _adime_count_datafile, _adime_create_datafile, \
      _adime_store_datafile, _adime_reset_datafile, \
      _adime_destroy_list_and_datafile, NULL },
#define ADIME_FORMAT_WVLIST \
   {  "wvlist", FALSE, \
      _adime_count_vlist, _adime_create_wvlist, \
      _adime_store_wlists, _adime_reset_wlists, \
      _adime_destroy_wvlist, NULL }, \
   ADIME_FORMAT_VLIST
#define ADIME_FORMAT_WLIST \
   {  "wlist", FALSE, \
      _adime_count_pointer_and_return_1, _adime_create_wlist, \
      _adime_store_wlists, _adime_reset_wlists, \
      _adime_destroy_wlist_and_wdatafile, NULL }, \
   ADIME_FORMAT_VLIST
#define ADIME_FORMAT_WDATAFILE \
   {  "wdatafile", FALSE, \
      _adime_count_datafile, _adime_create_wdatafile, \
      _adime_store_wdatafile, \
      _adime_reset_wdatafile, _adime_destroy_wlist_and_wdatafile, NULL }, \
   ADIME_FORMAT_VLIST
#define ADIME_FORMAT_DIALOGF \
   {  "dialogf", FALSE, \
      _adime_count_dialogf, _adime_create_dialogfs, \
      _adime_store_dialogfs, \
      _adime_reset_dialogfs, _adime_destroy_dialogfs, NULL },
#define ADIME_FORMAT_VDIALOGF \
   {  "vdialogf", FALSE, \
      _adime_count_vdialogf, _adime_create_dialogfs, \
      _adime_store_dialogfs, \
      _adime_reset_dialogfs, _adime_destroy_dialogfs, NULL },
#define ADIME_FORMAT_INT \
   {  "int", FALSE, \
      _adime_count_pointer_and_return_2, _adime_create_int, \
      _adime_store_int, _adime_reset_int, _adime_destroy_calc, NULL },
#define ADIME_FORMAT_PINT \
   {  "pint", FALSE, \
      _adime_count_pointer_and_return_1, _adime_create_pint, \
      _adime_store_pint, _adime_reset_int, _adime_destroy_plain, NULL },
#define ADIME_FORMAT_UINT \
   {  "uint", FALSE, \
      _adime_count_pointer_and_return_2, _adime_create_uint, \
      _adime_store_uint, _adime_reset_uint, _adime_destroy_calc, NULL },
#define ADIME_FORMAT_PUINT \
   {  "puint", FALSE, \
      _adime_count_pointer_and_return_1, _adime_create_puint, \
      _adime_store_puint, _adime_reset_uint, _adime_destroy_plain, NULL },
#define ADIME_FORMAT_SHORT \
   {  "short", FALSE, \
      _adime_count_pointer_and_return_2, _adime_create_short, \
      _adime_store_short, _adime_reset_short, _adime_destroy_calc, NULL },
#define ADIME_FORMAT_PSHORT \
   {  "pshort", FALSE, \
      _adime_count_pointer_and_return_1, _adime_create_pshort, \
      _adime_store_pshort, _adime_reset_short, _adime_destroy_plain, \
      NULL },
#define ADIME_FORMAT_USHORT \
   {  "ushort", FALSE, \
      _adime_count_pointer_and_return_2, _adime_create_ushort, \
      _adime_store_ushort, _adime_reset_ushort, _adime_destroy_calc, \
      NULL },
#define ADIME_FORMAT_PUSHORT \
   {  "pushort", FALSE, \
      _adime_count_pointer_and_return_1, _adime_create_pushort, \
      _adime_store_pushort, _adime_reset_ushort, _adime_destroy_plain, \
      NULL },
#define ADIME_FORMAT_CHAR \
   {  "char", FALSE, \
      _adime_count_pointer_and_return_2, _adime_create_char, \
      _adime_store_char, _adime_reset_char, _adime_destroy_calc, NULL },
#define ADIME_FORMAT_PCHAR \
   {  "pchar", FALSE, \
      _adime_count_pointer_and_return_1, _adime_create_pchar, \
      _adime_store_pchar, _adime_reset_char, _adime_destroy_plain, NULL },
#define ADIME_FORMAT_UCHAR \
   {  "uchar", FALSE, \
      _adime_count_pointer_and_return_2, _adime_create_uchar, \
      _adime_store_uchar, _adime_reset_uchar, _adime_destroy_calc, NULL },
#define ADIME_FORMAT_PUCHAR \
   {  "puchar", FALSE, \
      _adime_count_pointer_and_return_1, _adime_create_puchar, \
      _adime_store_puchar, _adime_reset_uchar, \
      _adime_destroy_plain, NULL },
#ifdef ADIME_LONG_LONG
   #define ADIME_FORMAT_LONGLONG \
      {  "longlong", FALSE, \
         _adime_count_pointer_and_return_2, _adime_create_longlong, \
         _adime_store_longlong, _adime_reset_longlong, \
         _adime_destroy_calc, NULL },
   #define ADIME_FORMAT_PLONGLONG \
      {  "plonglong", FALSE, \
         _adime_count_pointer_and_return_1, _adime_create_plonglong, \
         _adime_store_plonglong, _adime_reset_longlong, \
         _adime_destroy_plain, NULL },
   #define ADIME_FORMAT_ULONGLONG \
      {  "ulonglong", FALSE, \
         _adime_count_pointer_and_return_2, _adime_create_ulonglong, \
         _adime_store_ulonglong, _adime_reset_ulonglong, \
         _adime_destroy_calc, NULL },
   #define ADIME_FORMAT_PULONGLONG \
      {  "pulonglong", FALSE, \
         _adime_count_pointer_and_return_1, _adime_create_pulonglong, \
         _adime_store_pulonglong, _adime_reset_ulonglong, \
         _adime_destroy_plain, NULL },
#endif /* ifdef ADIME_LONG_LONG */
#define ADIME_FORMAT_DOUBLE \
   {  "double", FALSE, \
      _adime_count_pointer_and_return_2, _adime_create_double, \
      _adime_store_double, _adime_reset_double, \
      _adime_destroy_calc, NULL },
#define ADIME_FORMAT_PDOUBLE \
   {  "pdouble", FALSE, \
      _adime_count_pointer_and_return_1, _adime_create_pdouble, \
      _adime_store_double, _adime_reset_double, \
      _adime_destroy_plain, NULL },
#define ADIME_FORMAT_FLOAT \
   {  "float", FALSE, \
      _adime_count_pointer_and_return_2, _adime_create_float, \
      _adime_store_float, _adime_reset_float, _adime_destroy_calc, \
      NULL },
#define ADIME_FORMAT_PFLOAT \
   {  "pfloat", FALSE, \
      _adime_count_pointer_and_return_1, _adime_create_pfloat, \
      _adime_store_float, _adime_reset_float, \
      _adime_destroy_plain, NULL },
#define ADIME_FORMAT_LINE \
   {  "line", TRUE, \
      _adime_count_void_and_return_2, _adime_create_line, \
      NULL, NULL, NULL, NULL },
#define ADIME_FORMAT_NOTHING \
   {  "nothing", TRUE, \
      _adime_count_void_and_return_1, _adime_create_nothing, \
      NULL, NULL, NULL, NULL },
#define ADIME_FORMAT_CHAIN \
   {  "chain", TRUE, \
      _adime_count_chain, NULL, \
      NULL, NULL, NULL, NULL },



#define _ADIME_DECLARE_COUNT(name) \
   ADIME_FUNC(int, _adime_count_##name, \
              (ADIME_CONST char *desc, ADIME_CONST char *modifiers, \
               adime_va_list args, void **saved_args))

#define _ADIME_DECLARE_CREATE(name) \
   ADIME_FUNC(int, _adime_create_##name, \
              (DIALOG *d, ADIME_CONST char *desc, \
               ADIME_CONST char *modifiers, void *args))

#define _ADIME_DECLARE_STORE(name) \
   ADIME_FUNC(void, _adime_store_##name, (DIALOG *d))

#define _ADIME_DECLARE_RESET(name) \
   ADIME_FUNC(void, _adime_reset_##name, (DIALOG *d))

#define _ADIME_DECLARE_DESTROY(name) \
   ADIME_FUNC(void, _adime_destroy_##name, (DIALOG *d))

/* Helpers common to several formats. */
_ADIME_DECLARE_COUNT(pointer_and_return_1);
_ADIME_DECLARE_COUNT(pointer_and_return_2);
_ADIME_DECLARE_COUNT(void_and_return_1);
_ADIME_DECLARE_COUNT(void_and_return_2);
_ADIME_DECLARE_DESTROY(dp);

/* The formats. */
_ADIME_DECLARE_CREATE(bool);
_ADIME_DECLARE_STORE(bool);
_ADIME_DECLARE_RESET(bool);

_ADIME_DECLARE_CREATE(greybool);
_ADIME_DECLARE_STORE(greybool);
_ADIME_DECLARE_RESET(greybool);

_ADIME_DECLARE_CREATE(button);

_ADIME_DECLARE_COUNT(buttonrow);
_ADIME_DECLARE_CREATE(buttonrow);
_ADIME_DECLARE_DESTROY(buttonrow);

_ADIME_DECLARE_CREATE(string);
_ADIME_DECLARE_STORE(string);
_ADIME_DECLARE_RESET(string);

_ADIME_DECLARE_CREATE(filename);
_ADIME_DECLARE_STORE(filename);
_ADIME_DECLARE_RESET(filename);
_ADIME_DECLARE_DESTROY(filename);

_ADIME_DECLARE_COUNT(vlist);
_ADIME_DECLARE_COUNT(datafile);
_ADIME_DECLARE_CREATE(vlist);
_ADIME_DECLARE_CREATE(list);
_ADIME_DECLARE_CREATE(datafile);
_ADIME_DECLARE_CREATE(wvlist);
_ADIME_DECLARE_CREATE(wlist);
_ADIME_DECLARE_CREATE(wdatafile);
_ADIME_DECLARE_STORE(lists);
_ADIME_DECLARE_STORE(wlists);
_ADIME_DECLARE_STORE(datafile);
_ADIME_DECLARE_STORE(wdatafile);
_ADIME_DECLARE_RESET(lists);
_ADIME_DECLARE_RESET(wlists);
_ADIME_DECLARE_RESET(datafile);
_ADIME_DECLARE_RESET(wdatafile);
_ADIME_DECLARE_DESTROY(vlist);
_ADIME_DECLARE_DESTROY(wvlist);
_ADIME_DECLARE_DESTROY(list_and_datafile);
_ADIME_DECLARE_DESTROY(wlist_and_wdatafile);

_ADIME_DECLARE_COUNT(dialogf);
_ADIME_DECLARE_COUNT(vdialogf);
_ADIME_DECLARE_CREATE(dialogfs);
_ADIME_DECLARE_STORE(dialogfs);
_ADIME_DECLARE_RESET(dialogfs);
_ADIME_DECLARE_DESTROY(dialogfs);

_ADIME_DECLARE_CREATE(nothing);

_ADIME_DECLARE_CREATE(line);

_ADIME_DECLARE_COUNT(chain);

_ADIME_DECLARE_DESTROY(calc);
_ADIME_DECLARE_DESTROY(plain);

_ADIME_DECLARE_CREATE(int);
_ADIME_DECLARE_STORE(int);
_ADIME_DECLARE_RESET(int);
_ADIME_DECLARE_CREATE(pint);
_ADIME_DECLARE_STORE(pint);

_ADIME_DECLARE_CREATE(uint);
_ADIME_DECLARE_STORE(uint);
_ADIME_DECLARE_RESET(uint);
_ADIME_DECLARE_CREATE(puint);
_ADIME_DECLARE_STORE(puint);

_ADIME_DECLARE_CREATE(short);
_ADIME_DECLARE_STORE(short);
_ADIME_DECLARE_RESET(short);
_ADIME_DECLARE_CREATE(pshort);
_ADIME_DECLARE_STORE(pshort);

_ADIME_DECLARE_CREATE(ushort);
_ADIME_DECLARE_STORE(ushort);
_ADIME_DECLARE_RESET(ushort);
_ADIME_DECLARE_CREATE(pushort);
_ADIME_DECLARE_STORE(pushort);

_ADIME_DECLARE_CREATE(char);
_ADIME_DECLARE_STORE(char);
_ADIME_DECLARE_RESET(char);
_ADIME_DECLARE_CREATE(pchar);
_ADIME_DECLARE_STORE(pchar);

_ADIME_DECLARE_CREATE(uchar);
_ADIME_DECLARE_STORE(uchar);
_ADIME_DECLARE_RESET(uchar);
_ADIME_DECLARE_CREATE(puchar);
_ADIME_DECLARE_STORE(puchar);

/* long long is a bit tricky to implement and doesn't work yet. */
#ifdef ADIME_LONG_LONG
   _ADIME_DECLARE_CREATE(longlong);
   _ADIME_DECLARE_STORE(longlong);
   _ADIME_DECLARE_RESET(longlong);
   _ADIME_DECLARE_CREATE(plonglong);
   _ADIME_DECLARE_STORE(plonglong);

   _ADIME_DECLARE_CREATE(ulonglong);
   _ADIME_DECLARE_STORE(ulonglong);
   _ADIME_DECLARE_RESET(ulonglong);
   _ADIME_DECLARE_CREATE(pulonglong);
   _ADIME_DECLARE_STORE(pulonglong);
#endif

_ADIME_DECLARE_CREATE(float);
_ADIME_DECLARE_STORE(float);
_ADIME_DECLARE_RESET(float);
_ADIME_DECLARE_CREATE(pfloat);
_ADIME_DECLARE_STORE(pfloat);

_ADIME_DECLARE_CREATE(double);
_ADIME_DECLARE_STORE(double);
_ADIME_DECLARE_RESET(double);
_ADIME_DECLARE_CREATE(pdouble);
_ADIME_DECLARE_STORE(pdouble);



/********************************************/
/************ Edit Box Functions ************/
/********************************************/

#if 0 /* This is not official yet. */

/* Clipboard. */
#define ADIME_CLIPBOARD_TYPE_EMPTY     (0)
#define ADIME_CLIPBOARD_TYPE_TEXT      (1)
#define ADIME_CLIPBOARD_TYPE_NUMBER    (2)
#define ADIME_CLIPBOARD_TYPE_BITMAP    (3)
#define ADIME_CLIPBOARD_TYPE_SAMPLE    (4)
typedef ADIME_METHOD(void, ADIME_CLIPBOARD_ITEM_DESTROYER, (void *data));
typedef struct ADIME_CLIPBOARD_ITEM    /* One thing in clipboard. */
{
   int type;                           /* One of the constants above. */
   void *data;                         /* Contents. */
   ADIME_CLIPBOARD_ITEM_DESTROYER destroy;/* Function for freeing the data.*/
   struct CLIPBOARD_ITEM *next;        /* There may be >1 thing in clipboard
                                          at a time. */
} ADIME_CLIPBOARD_ITEM;

/* A complete clipboard system. */
typedef struct ADIME_CLIPBOARD
{
   int max_history;              /* Clipboard may remember old contents. */
   int num_items;                /* Current number of remembered items. */
   ADIME_CLIPBOARD_ITEM *item;   /* Array of items. */
} ADIME_CLIPBOARD;

ADIME_FUNC(int, clipboard_has_contents, (ADIME_CONST CLIPBOARD *c));
ADIME_FUNC(int, clipboard_has_type, (ADIME_CONST ADIME_CLIPBOARD *c, int type));
ADIME_FUNC(void *, clipboard_get_data, (ADIME_CONST ADIME_CLIPBOARD *c, int type));

ADIME_FUNC(int, clipboard_put_text, (ADIME_CLIPBOARD *c, ADIME_CONST char *text));
ADIME_FUNC(int, clipboard_put_bitmap, (ADIME_CLIPBOARD *c, BITMAP *bmp));
ADIME_FUNC(int, clipboard_put_sample, (ADIME_CLIPBOARD *c, SAMPLE *s));
ADIME_FUNC(int, clipboard_put_item, (ADIME_CLIPBOARD *c, ADIME_CONST void *data, int type, ADIME_CLIPBOARD_ITEM_DESTROYER destroy));
ADIME_FUNC(int, clipboard_put_subitem, (ADIME_CLIPBOARD *c, ADIME_CONST void *data, int type, ADIME_CLIPBOARD_ITEM_DESTROYER destroy));

ADIME_FUNC(ADIME_CLIPBOARD *, create_clipboard, (void));
ADIME_FUNC(ADIME_CLIPBOARD *, create_history_clipboard, (int max_history));
ADIME_FUNC(void, destroy_clipboard, (ADIME_CLIPBOARD *c));
ADIME_FUNC(void, destroy_clipboard_item, (ADIME_CLIPBOARD_ITEM *ci));

/* Edit box. */
#define ADIME_EDIT_TYPE_TEXT              (1)
#define ADIME_EDIT_TYPE_NUM               (2)
#define ADIME_EDIT_TYPE_FILE              (3)

#define ADIME_EDIT_INSERT                 (1)
#define ADIME_EDIT_OVERWRITE              (2)
#define ADIME_EDIT_ONLY_INSERT            (TRUE)   /* Can't switch to overwrite. */
#define ADIME_EDIT_ONLY_OVERWRITE         (FALSE)  /* Can't switch to insert. */

#define ADIME_EDIT_NUM_DECIMALS           (1) /* Real numbers. */
#define ADIME_EDIT_NUM_NODECIMALS         (2) /* Integers. */
#define ADIME_EDIT_NUM_HALVES             (3) /* Using '1/2'. */
#define ADIME_EDIT_NUM_QUARTERS           (4) /* Using '1/4', '1/2' and '3/4'. */

#define ADIME_EDIT_FILE_NAME              (0x0001)
#define ADIME_EDIT_FILE_PATH              (0x0002)
#define ADIME_EDIT_FILE_DRIVE             (0x0004)
#define ADIME_EDIT_FILE_DOS               (1)
#define ADIME_EDIT_FILE_UNIX              (2)
#define ADIME_EDIT_FILE_WINDOWS           (3)
#define ADIME_EDIT_FILE_CURRENT           (4)
#define ADIME_EDIT_FILE_WILDCARDS         (TRUE)
#define ADIME_EDIT_FILE_NOWILDCARDS       (FALSE)

#define ADIME_EDIT_TEXT_SPACE             (0x0001)
#define ADIME_EDIT_TEXT_SYMBOLS           (0x0002)
#define ADIME_EDIT_TEXT_DIGITS            (0x0004)
#define ADIME_EDIT_TEXT_UPPER             (0x0008)
#define ADIME_EDIT_TEXT_LOWER             (0x0010)
#define ADIME_EDIT_TEXT_ALPHA             (ADIME_EDIT_TEXT_UPPER | ADIME_EDIT_TEXT_LOWER)
#define ADIME_EDIT_TEXT_ALL               (ADIME_EDIT_TEXT_SPACE | ADIME_EDIT_TEXT_SYMBOLS | \
                                     ADIME_EDIT_TEXT_DIGITS | ADIME_EDIT_TEXT_ALPHA)

#define ADIME_EDIT_CLIPBOARD              (3)   /* Full clipboard support. */
#define ADIME_EDIT_SELECT                 (1)   /* No clipboard, but selections. */
#define ADIME_EDIT_NOSELECT               (0)   /* Not even selections. */

struct ADIME_EDITBOX;
struct ADIME_EDITBOX_OPTIONS;

/* Function pointers to most things that can happen in an edit box. */
typedef struct ADIME_EDITBOX_FUNCTIONS
{
   ADIME_METHOD(int, edit, (struct ADIME_EDITBOX *ed, int key, int shifts));
   ADIME_METHOD(int, key_text, (struct ADIME_EDITBOX *ed, int key, int shifts));
   ADIME_METHOD(int, key_nontext, (struct ADIME_EDITBOX *ed, int key, int shifts));

   ADIME_METHOD(int, pos_move, (struct ADIME_EDITBOX *ed, int pos, int select));
   ADIME_METHOD(int, pos_left, (struct ADIME_EDITBOX *ed, int select));
   ADIME_METHOD(int, pos_right, (struct ADIME_EDITBOX *ed, int select));
   ADIME_METHOD(int, pos_word_left,
             (struct ADIME_EDITBOX *ed, int from_pos, int select));
   ADIME_METHOD(int, pos_word_right,
             (struct ADIME_EDITBOX *ed, int from_pos, int select));
   ADIME_METHOD(int, pos_home, (struct ADIME_EDITBOX *ed, int select));
   ADIME_METHOD(int, pos_end, (struct ADIME_EDITBOX *ed, int select));

   ADIME_METHOD(int, delete_left, (struct ADIME_EDITBOX *ed));
   ADIME_METHOD(int, delete_right, (struct ADIME_EDITBOX *ed));

   ADIME_METHOD(int, toggle_insertmode, (struct ADIME_EDITBOX *ed));

   ADIME_METHOD(int, select_word_at_pos, (struct ADIME_EDITBOX *ed, int pos));

   ADIME_METHOD(int, pos_at_pixel, (struct ADIME_EDITBOX *ed, int pixel));

   ADIME_METHOD(int, clip_cut, (struct ADIME_EDITBOX *ed));
   ADIME_METHOD(int, clip_copy, (struct ADIME_EDITBOX *ed));
   ADIME_METHOD(int, clip_paste, (struct ADIME_EDITBOX *ed));

   ADIME_METHOD(int, set_text, (struct ADIME_EDITBOX *ed, ADIME_CONST char *text));

   ADIME_METHOD(int, replace_text, (struct ADIME_EDITBOX *ed, ADIME_CONST char *text, int start, int length));

   ADIME_METHOD(int, num_increase, (struct ADIME_EDITBOX *ed));
   ADIME_METHOD(int, num_decrease, (struct ADIME_EDITBOX *ed));
   ADIME_METHOD(int, num_big_increase, (struct ADIME_EDITBOX *ed));
   ADIME_METHOD(int, num_big_decrease, (struct ADIME_EDITBOX *ed));

   ADIME_METHOD(struct ADIME_EDITBOX *, create, (struct ADIME_EDITBOX_OPTIONS *eo, ADIME_CONST char *text));
   ADIME_METHOD(void, destroy, (struct ADIME_EDITBOX *ed));
} ADIME_EDITBOX_FUNCTIONS;

/* Special data used by textedit. */
/* Currently unused! */
typedef struct ADIME_EDITBOX_OPTIONS_TEXT
{
   int available_chars; /* What kind of characters can be used? */
   int force_case;      /* Autoconvert to upper/lower? */
} ADIME_EDITBOX_OPTIONS_TEXT;

/* Special data used by numedit. */
/* Currently unused! */
typedef struct ADIME_EDITBOX_OPTIONS_NUM
{
   int decimaltype: 2;  /* Decimals or not? */
   int format     : 2;  /* Long format (long double, long long) or usual? */
   int _case      : 2;  /* Upper or lower case, or both? */
   int force_case : 1;  /* Autoconvert to upper/lower? */
   int plussign, minussign, zerosign;/* What characters to use. */
   int delimitor;       /* Thousand delimitor. */
   int sepdistance;     /* Distance between thousand delimitors. */
   int base;            /* Decimal, hex, oct, bin or something else? */
   int char14, char12, char34; /* What characters to use */
   long double min, max;       /* Smallest/largest possible value. */
   long double smallchange, largechange; /* Up/down arrows increment. */
   long double default_value;  /* What value to use if it's illegal. */
} ADIME_EDITBOX_OPTIONS_NUM;

/* Special data used by fileedit. */
/* Currently unused! */
typedef struct ADIME_EDITBOX_OPTIONS_FILE
{
   int system        : 2;  /* What operating system to use. */
   int wildcards     : 2;  /* Enable wildcards? */
   int _case         : 2;  /* Upper or lower case, or both? */
   int force_case    : 1;  /* Autoconvert case? */
   int drive         : 2;  /* Include drive? (DOS/windows only) */
   int directories   : 2;  /* Include directory names? */
   int filename      : 2;  /* Include filename? */
   int extension     : 2;  /* Include extension? */
   int pathdelimitor : 2;  /* Slashes or backslashes? */
} ADIME_EDITBOX_OPTIONS_FILE;

/* Properties for all kinds of edit boxes. */
typedef struct ADIME_EDITBOX_OPTIONS
{
   int type;   /* Text, numbers, filename or code? */
   union       /* Extra properties depending on the type of editbox. */
   {
      ADIME_EDITBOX_OPTIONS_TEXT text;  /* Special data belonging to textedits. */
      /* These are not implemented yet. */
      /* ADIME_EDITBOX_OPTIONS_NUM  num; */  /* Special data belonging to numedits. */
      /* ADIME_EDITBOX_OPTIONS_FILE file; */ /* Special data belonging to fileedits. */
   } data;

   int max_length;      /* Limit by number of characters. */
   int max_pixel_width; /* Limit by pixel width. */
   int max_bytes;       /* Limit by length in bytes. */
   FONT *font;          /* What font to use when calculating length. */
   /* int wrap; */      /* Use word wrap? */

   int write_protected; /* OK to change? */
   int dynamic;         /* OK to realloc? */
   int free_text_on_destroy; /* Should destroy_edit() free the text? */
   int default_bytes;   /* Bytes allocated by create_edit(). */
   int increase_bytes;  /* How much to realloc each time? */
   void *extra;         /* May point to a structure with more info. */

   int clipboard_functions;/* OK to select and use clipboard? */
   ADIME_CLIPBOARD *clipboard;   /* Used by clipboard operations. */
   ADIME_EDITBOX_FUNCTIONS *funcs;   /* Table with functions to use for editing. */
} ADIME_EDITBOX_OPTIONS;

/* The edit box struct. */
typedef struct ADIME_EDITBOX
{
   char *text;             /* The actual text. */
/* long double *val_ld; This is not implemented yet.
   long *val_l; */
   int selstart, sellength;/* Selection, measured in # of characters. */
   int pos;                /* Cursor position, measured in # of characters.*/
   int *insert;            /* Insert or overwrite mode? */

   int alloced_size;       /* How much room do we have? */
   ADIME_EDITBOX_OPTIONS *options; /* Controls the behavior of the edit box. */
} ADIME_EDITBOX;

/* Struct with information about the look of an edit box. Not yet used. */
typedef struct ADIME_GRAPHIC_EDIT
{
   int x1, y1, x2, y2;
   BITMAP *bmp;
   ADIME_EDITBOX *ed;
   int text_color, bg_color;
   int selection_text_color, selection_bg_color;
   BITMAP *bg_bitmap;
   int cursor_color;
   int cursor_frequence;
   int cursor_blink_mode;
} ADIME_GRAPHIC_EDIT;


/* Bits in return value from functions. */
#define ADIME_EDIT_CHANGE_TEXT                  (0x00000001)
#define ADIME_EDIT_CHANGE_VALUE                 (0x00000002)
#define ADIME_EDIT_CHANGE_LENGTH                (0x00000004)
#define ADIME_EDIT_CHANGE_PIXELLENGTH           (0x00000008)
#define ADIME_EDIT_CHANGE_BYTES                 (0x00000010)
#define ADIME_EDIT_CHANGE_TEXTOVERFLOWN         (0x00000020)
#define ADIME_EDIT_CHANGE_TEXTDELETED           (0x00000040)
#define ADIME_EDIT_CHANGE_TEXTOVERWRITTEN       (0x00000080)
#define ADIME_EDIT_CHANGE_TEXTLOST              (ADIME_EDIT_CHANGE_TEXTDELETED |       \
                                           ADIME_EDIT_CHANGE_TEXTOVERWRITTEN |   \
                                           ADIME_EDIT_CHANGE_TEXTOVERFLOWN)
#define ADIME_EDIT_CHANGE_CLIPBOARD_OPERATION   (0x00000100)
#define ADIME_EDIT_CHANGE_CLIPBOARD_CONTENTS    (0x00000200)
#define ADIME_EDIT_CHANGE_CLIPBOARD_LENGTH      (0x00000400)
#define ADIME_EDIT_CHANGE_CLIPBOARD_PIXELLENGTH (0x00000400)
#define ADIME_EDIT_CHANGE_CLIPBOARD_BYTES       (0x00000400)
#define ADIME_EDIT_CHANGE_POS                   (0x00000800)
#define ADIME_EDIT_CHANGE_PIXELPOS              (0x00001000)
#define ADIME_EDIT_CHANGE_BYTEPOS               (0x00002000)
#define ADIME_EDIT_CHANGE_SEL                   (0x00004000)
#define ADIME_EDIT_CHANGE_INSERT                (0x00008000)
#define ADIME_EDIT_CHANGE_BEEP                  (0x00010000)
#define ADIME_EDIT_CHANGE_SHOWCURSOR            (0x00020000)
#define ADIME_EDIT_CHANGE_WRITEPROTECTED        (0x00040000)
#define ADIME_EDIT_CHANGE_INVALID_OPERATION     (0x00080000)
#define ADIME_EDIT_CHANGE_KEYIGNORED            (0x00100000 |                    \
                                           ADIME_EDIT_CHANGE_INVALID_OPERATION)

ADIME_FUNC(int, edit_delete_sel, (ADIME_EDITBOX *ed));
ADIME_FUNC(int, edit_select_all, (ADIME_EDITBOX *ed));
ADIME_FUNC(int, install_edit, (void));
ADIME_FUNC(int, edit, (ADIME_EDITBOX *ed, int key, int shifts));
ADIME_FUNC(int, edit_set_pos, (ADIME_EDITBOX *ed, int pos));
ADIME_FUNC(int, edit_select_to_pos, (ADIME_EDITBOX *ed, int pos));
ADIME_FUNC(int, edit_set_text, (ADIME_EDITBOX *ed, ADIME_CONST char *text));
ADIME_FUNC(int, edit_cut, (ADIME_EDITBOX *ed));
ADIME_FUNC(int, edit_copy, (ADIME_EDITBOX *ed));
ADIME_FUNC(int, edit_paste, (ADIME_EDITBOX *ed));
ADIME_FUNC(int, edit_select_word_at_pos, (ADIME_EDITBOX *ed, int pos));
ADIME_FUNC(ADIME_EDITBOX *, create_edit, (ADIME_EDITBOX_OPTIONS *eo, ADIME_CONST char *text));
ADIME_FUNC(void, destroy_edit, (ADIME_EDITBOX *ed));

ADIME_FUNC(int, normal_edit, (ADIME_EDITBOX *ed, int key, int shifts));
ADIME_FUNC(int, normal_edit_key_nontext, (ADIME_EDITBOX *ed, int key, int shifts));
ADIME_FUNC(int, normal_edit_replace_text, (ADIME_EDITBOX *ed, ADIME_CONST char *text, int start, int length));
ADIME_FUNC(int, normal_edit_key_text, (ADIME_EDITBOX *ed, int c, int shifts));
ADIME_FUNC(int, normal_edit_pos_move, (ADIME_EDITBOX *ed, int pos, int select));
ADIME_FUNC(int, normal_edit_pos_left, (ADIME_EDITBOX *ed, int select));
ADIME_FUNC(int, normal_edit_pos_right, (ADIME_EDITBOX  *ed, int select));
ADIME_FUNC(int, normal_edit_pos_word_left, (ADIME_EDITBOX *ed, int from_pos, int select));
ADIME_FUNC(int, normal_edit_pos_word_right, (ADIME_EDITBOX *ed, int from_pos, int select));
ADIME_FUNC(int, normal_edit_pos_home, (ADIME_EDITBOX *ed, int select));
ADIME_FUNC(int, normal_edit_pos_end, (ADIME_EDITBOX *ed, int select));
ADIME_FUNC(int, normal_edit_delete_left, (ADIME_EDITBOX *ed));
ADIME_FUNC(int, normal_edit_delete_right, (ADIME_EDITBOX *ed));
ADIME_FUNC(int, normal_edit_clip_cut, (ADIME_EDITBOX *ed));
ADIME_FUNC(int, normal_edit_clip_paste, (ADIME_EDITBOX *ed));
ADIME_FUNC(int, normal_edit_clip_copy, (ADIME_EDITBOX *ed));
ADIME_FUNC(int, normal_edit_toggle_insertmode, (ADIME_EDITBOX *ed));
ADIME_FUNC(int, normal_edit_select_word_at_pos, (ADIME_EDITBOX *ed, int pos));
ADIME_FUNC(int, normal_edit_pos_at_pixel, (struct ADIME_EDITBOX *ed, int pixel));
ADIME_FUNC(int, normal_edit_set_text, (ADIME_EDITBOX *ed, ADIME_CONST char *text));
ADIME_FUNC(ADIME_EDITBOX *, normal_edit_create, (ADIME_EDITBOX_OPTIONS *eo, ADIME_CONST char *text));
ADIME_FUNC(void, normal_edit_destroy, (ADIME_EDITBOX *ed));
ADIME_FUNC(int, normal_edit_num_increase, (ADIME_EDITBOX *ed));
ADIME_FUNC(int, normal_edit_num_decrease, (ADIME_EDITBOX *ed));
ADIME_FUNC(int, normal_edit_num_big_increase, (ADIME_EDITBOX *ed));
ADIME_FUNC(int, normal_edit_num_big_decrease, (ADIME_EDITBOX *ed));

ADIME_FUNC(int, unix_edit_key_nontext, (ADIME_EDITBOX *ed, int key, int shifts));

ADIME_VAR(int, _edit_insert);
ADIME_VAR(int *, _edit_default_insert);

ADIME_VAR(ADIME_CLIPBOARD *, _edit_clipboard);
ADIME_VAR(ADIME_EDITBOX_OPTIONS, _normal_edit_text);
ADIME_VAR(ADIME_EDITBOX_FUNCTIONS, _normal_edit_funcs);

#endif /* if 0 */



/* End of extern "C" { */
#ifdef __cplusplus
   }
#endif

#endif
