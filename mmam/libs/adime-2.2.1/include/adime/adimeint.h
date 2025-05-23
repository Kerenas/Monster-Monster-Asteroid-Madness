/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   adimeint.h:
   Internal declarations for the Adime library.

   By Sven Sandberg
*/
#ifndef _ADIME_INCLUDE_ADIMEINT_H_
#define _ADIME_INCLUDE_ADIMEINT_H_


#ifndef _ADIME_INCLUDE_ADIME_H_
   #error you must include adime.h first.
#endif



/* Information for one object. */
typedef struct _ADIME_DIALOGF_OBJECT {
   _ADIME_DIALOGF_FORMAT *type;  /* Pointer into the `_dialogf_formats' list. */
   int num_dialogs;        /* Number of dialog objects created. */
   DIALOG *dialogs;        /* Pointer to first object. */
   int again;              /* Flag telling if we should do another pass. */
   int bottom;             /* Coordinate of bottommost object. */

   char *desc;
   char *modifiers;
   void *args;
   struct _ADIME_DIALOGF_OBJECT *next;
} _ADIME_DIALOGF_OBJECT;



/* Information set up by `dialogf_init()', needed by `dialogf_run()'. */
typedef struct _ADIME_DIALOGF_DIALOG
{
   /* Actual dialog. */
   DIALOG *dialog;

   int need_final_pass;
   int more_passes;
   int return_value;

   _ADIME_DIALOGF_OBJECT *fo;
   int top, bottom;
   int edit_w;

   /* Array of all objects. */
   _ADIME_DIALOGF_OBJECT *objects;
} _ADIME_DIALOGF_DIALOG;



ADIME_VAR(int, _adime_inited);
#if ALLEGRO_DATE >= 20050314
#define ADIME_ADD_EXIT_FUNC(FUNC) _add_exit_func(FUNC, #FUNC)
#else
#define ADIME_ADD_EXIT_FUNC(FUNC) _add_exit_func(FUNC)
#endif



/* Format and callback function for default ok/cancel buttons. */
#define ADIME_OK_CANCEL_BUTTON_FORMAT \
   "%buttonrow[&OK;ALT+O;CTRL+O;ENTER,&Cancel;ALT+C;CTRL+C;ESC]"
ADIME_FUNC(int, _adime_ok_cancel_button_callback, (DIALOG *d, int b));



/* The five parts into which one call to dialogf() is divided. */
ADIME_FUNC(_ADIME_DIALOGF_DIALOG *, _adime_dialogf_start, (ADIME_CONST char *title, int x_pos, int y_pos, int edit_w, ADIME_CONST char *format, adime_va_list args));
ADIME_FUNC(void, _adime_dialogf_count_objects, (_ADIME_DIALOGF_DIALOG *dd, ADIME_CONST char *format, adime_va_list args));
ADIME_FUNC(void, _adime_dialogf_create_objects, (_ADIME_DIALOGF_DIALOG *dd));
ADIME_FUNC(int, _adime_dialogf_run, (_ADIME_DIALOGF_DIALOG *dd));
ADIME_FUNC(void, _adime_dialogf_store_results, (_ADIME_DIALOGF_DIALOG *dd));
ADIME_FUNC(void, _adime_dialogf_reset_dialog, (_ADIME_DIALOGF_DIALOG *dd));
ADIME_FUNC(void, _adime_dialogf_end, (_ADIME_DIALOGF_DIALOG *dd));



/* A list of all formats. */
ADIME_VAR(_ADIME_DIALOGF_FORMAT *, _adime_dialogf_formats);

/* Registers a new format specifier. */
ADIME_FUNC(void, _adime_register_dialogf_format, (
 char *specifier,
 int handle_desc,
 ADIME_METHOD(int, count_dialog, (ADIME_CONST char *desc, ADIME_CONST char *modifiers, adime_va_list args, void **saved_args)),
 ADIME_METHOD(int, create_dialog, (DIALOG *dialog, ADIME_CONST char *desc, ADIME_CONST char *modifiers, void *args)),
 ADIME_METHOD(void, store_dialog, (DIALOG *object)),
 ADIME_METHOD(void, reset_dialog, (DIALOG *object)),
 ADIME_METHOD(void, destroy_dialog, (DIALOG *object))));

#define _ADIME_BMP  (adime_bmp == NULL ? screen : adime_bmp)
#define _ADIME_FONT (adime_font == NULL ? font : adime_font)
#define _ADIME_TITLE_FONT (adime_title_font == NULL ? font : adime_title_font)
#define _ADIME_BUTTON_FONT (adime_button_font == NULL ? font : adime_button_font)
#define _ADIME_BMP_W (adime_bmp == NULL ? SCREEN_W : adime_bmp->w)
#define _ADIME_BMP_H (adime_bmp == NULL ? SCREEN_H : adime_bmp->w)
#define _ADIME_COLOR(name) \
   ( \
      (adime_##name##_rgb.filler == 0) ? \
         (makecol(adime_##name##_rgb.r, \
                  adime_##name##_rgb.g, \
                  adime_##name##_rgb.b)) \
      : \
         (adime_##name##_rgb.r) \
   )

#define _ADIME_TEXT_COLOR                _ADIME_COLOR(text)
#define _ADIME_DISABLED_TEXT_COLOR       _ADIME_COLOR(disabled_text)
#define _ADIME_ERROR_TEXT_COLOR          _ADIME_COLOR(error_text)
#define _ADIME_BORDER_COLOR              _ADIME_COLOR(border)
#define _ADIME_BACKGROUND_COLOR          _ADIME_COLOR(background)
#define _ADIME_EDIT_FIELD_COLOR          _ADIME_COLOR(edit_field)
#define _ADIME_BUTTON_COLOR              _ADIME_COLOR(button)
#define _ADIME_DARK_SHADOW_COLOR         _ADIME_COLOR(dark_shadow)
#define _ADIME_SHADOW_COLOR              _ADIME_COLOR(shadow)
#define _ADIME_HIGHLIGHT_COLOR           _ADIME_COLOR(highlight)
#define _ADIME_LIGHT_HIGHLIGHT_COLOR     _ADIME_COLOR(light_highlight)
#define _ADIME_TITLE_TEXT_COLOR          _ADIME_COLOR(title_text)
#define _ADIME_TITLE_BACKGROUND_COLOR    _ADIME_COLOR(title_background)
#define _ADIME_TITLE_SHADOW_COLOR        _ADIME_COLOR(title_shadow)
#define _ADIME_TITLE_HIGHLIGHT_COLOR     _ADIME_COLOR(title_highlight)


/* Some information that the creation functions may need. */
ADIME_VAR(_ADIME_DIALOGF_DIALOG *, _adime_dialog);
ADIME_VAR(int, _adime_dialogf_final_pass);
ADIME_VAR(int, _adime_dialogf_pass_index);

/* Helper function for getting the adimensions of serveral lines of text. */
ADIME_FUNC(void, _adime_multiline_text_size, (ADIME_CONST FONT *f, ADIME_CONST char *text, int *w, int *h));

/* Helper for creating a description text. */
ADIME_FUNC(void, _adime_dialogf_create_desc, (DIALOG *d, int y, ADIME_CONST char *desc));

/* Helpers for getting arguments from argument lists. */
#define _ADIME_ARG_COMMA     (0x0001)
#define _ADIME_ARG_END       (0x0002)
#define _ADIME_ARG_NONDIGIT  (0x0004)
#define _ADIME_ARG_DIGIT     (0x0008)
#define _ADIME_ARG_READ      (0x0010)
ADIME_FUNC(int, _adime_get_int_arg, (ADIME_CONST char **arg, int *out_p));
ADIME_FUNC(int, _adime_get_double_arg, (ADIME_CONST char **arg, double *out_p));
ADIME_FUNC(int, _adime_get_string_arg, (ADIME_CONST char **arg, char **out_p));
ADIME_FUNC(int, _adime_get_strlist_arg, (ADIME_CONST char **arg_text, int *out_num, char ***out_list));

/* Helpers for clamping the result of %cint & co. */
ADIME_FUNC(double, _adime_clamp_calc, (DIALOG *d, double min, double max, int round));
ADIME_FUNC(double, _adime_clamp_plain, (DIALOG *d, double min, double max, int round));

/* Helper routines for creating number objects. */
ADIME_FUNC(int, _adime_create_pinteger, (DIALOG *d, ADIME_CONST char *modifiers, void *args, double min_val, double max_val));
ADIME_FUNC(int, _adime_create_pfloatingpoint, (DIALOG *d, ADIME_CONST char *modifiers, void *args, int is_double));

/* Returns nonzero if x is NaN, zero otherwise. */
ADIME_FUNC(int, _adime_isnan, (double x));


#ifdef DEBUGMODE
   #define _ADIME_ASSERT(cond, msg) \
      ( \
         (cond) ? \
            (void)0 \
         : \
         ( \
            al_trace("_ADIME_ASSERT() failed in %s.%d:\n", \
                     __FILE__, __LINE__), \
            al_trace("The condition was: (%s)\n", #cond), \
            al_trace msg , \
            al_assert(__FILE__, __LINE__), \
            (void)0 \
         ) \
      )
#else
   #define _ADIME_ASSERT(cond, msg) ((void)0)
#endif

#define _ADIME_ASSERT_ARG_END(name) \
   _ADIME_ASSERT(state & _ADIME_ARG_END, \
                 ("Error in \"%s%%" name "[%s]\": " \
                  "Garbage found at end of modifier (']' expected).", \
                  desc, modifiers));


#define _ADIME_MALLOC(p, nelems, TYPE) \
   ( \
      (p) = (TYPE *)malloc((nelems) * sizeof(TYPE)), \
      (!(p) ? \
         (void)exit(255) \
      : \
         (void)0 \
      ), \
      (p) \
   )
#define _ADIME_REALLOC(p, nelems, TYPE) \
   ( \
      (p) = (TYPE *)realloc((p), (nelems) * sizeof(TYPE)) \
      (!(p) ? \
         (void)exit(255) \
      : \
         (void) \
      ), \
      (p) \
   )


#define _ADIME_UNUSED_ARG(arg)  (void)arg;



#endif
