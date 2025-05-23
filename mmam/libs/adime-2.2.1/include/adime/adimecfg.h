/*
   Although this file was written for Adime, it is intended to be generic
   enough that you can reuse it for any project. The only Adime-specific part
   is the names of macros, so search and replace it as you wish before you
   use it in your project.

   How to use this file:

   1. Include this file from the top of all header files.

   2. Use the macros listed below to declare any identifiers in your headers.
      Type definitions and macros should be declared/defined as usually,
      except for function pointers in structs (see METHOD()).

   VAR(type, name)
      Declare a variable.
         Example:
            VAR(int, x);
         means
            extern int x;

   ARRAY(type, name)
      Declare an array.
         Example:
            ARRAY(int, list);
         means
            extern int list[];

   FUNC(return_type, name, (arguments))
      Declare a function.
         Example:
            FUNC(int, foo, (int x, int y));
         means
            int foo(int x, int y);

   FUNCPTR(return_type, name, (arguments))
      Declare a function pointer.
         Example:
            FUNCPTR(int, foo, (int x, int y));
         means
            int (*foo)(int x, int y);

   METHOD(return_type, name, (arguments))
      Declare a function pointer when it occurs as a function argument or as
      a struct member. The difference between a method and a function pointer
      in this context is that a function pointer declares a symbol in the
      library, whereas a method merely describes a type.
         Example:
            FUNC(void, foo, (METHOD(int, bar, (int x))));
         means
            void foo(int (*bar)(int x));

         Example:
            typedef struct {
               METHOD(int, foo, (int x));
               METHOD(void, bar, (void));
            } my_struct;
         means
            typedef struct {
               int (*foo)(int x);
               void (*bar)(void);
            } my_struct;

   INLINE(return_type, name, (arguments), { code })
      Used to declare an inline function. This works smoothly even on
      platforms that don't support inline. Unfortunately the macro form puts
      some limits on the code: it must not contain commas that are not
      surrounded by parentheses. While this is usually not a problem, it can
      be hard to track the problem down when it happens. The two
      possibilities to break this rule is to use certain platform-specific
      extensions (in particular inline assembler for e.g. djgpp, IIRC), or
      to use the comma operator.
         Example:
            INLINE(int, square, (int x),
            {
               return x * x;
            })
         means
            extern inline int square(int x)
            {
               return x * x;
            }

   BEGIN_INLINE(return_type, name, (arguments))
   END_OF_INLINE()
      These provide an alternative way to declare an inline function. These
      are more flexible in that you can put commas anywhere in the code (see
      the documentation for INLINE()), but require more lines of code.
         Example:
            BEGIN_INLINE(int, square, (int x))
            #ifdef INLINE_PROVIDE_CODE
            {
               return x;
            }
            #endif
            END_OF_INLINE()
         declares an inline function that returns the square of its argument.

   LOCAL_INLINE(return_type, name, (arguments), code)
   BEGIN_LOCAL_INLINE(return_type, name, (arguments))
   END_OF_LOCAL_INLINE()
      Use these rather than the ones without LOCAL when you declare inline
      functions in source files, i.e. inline functions that will only be
      called from within the same file.

   PRINTF_FUNC(return_type, name, (arguments), string_index, vararg_index)
      Declare a function which takes a printf()-style format string.
      string_index specifies which argument is the format string, and
      vararg_index specifies which argument is the first that should be
      checked.
         Example:
            PRINTF_FUNC(int, fprintf, (FILE *f, char *format, ...), 2, 3);
         means that the second argument to `fprintf()' is the format string
         and that the first argument to check against the format string is
         argument number 3.

   CONST_FUNC(return_type, name, (arguments))
      Declare a const function. A const function function is one that doesn't
      read or write any global memory, i.e. one whose return value is
      completely determined by its arguments without dereferencing them.

   CONST
      Qualifier to constant function arguments.
         Example:
            FUNC(int, strcmp, (CONST char *s1, CONST char *s2));
         means that strcmp won't write to s1 or s2.

   Other macros:
   HAS_INLINE
      You normally don't need to bother about this. It's defined if the
      current platform supports inline functions.

   INLINE_PROVIDE_CODE
      This is defined if and only if you should provide a function body
      between BEGIN_INLINE() and END_OF_INLINE(). So you must conditionalize
      it out as in the example in the documentation for BEGIN_INLINE().

   INSTANTIATE_INLINE
      You normally don't need to bother about this. It's defined by inline.c
      so that this file knows about the special care that needs to be taken.



   The problems that make all this necessary are:

   1. Declarations need special attributes when the symbol is in a dll. One
   attribute is needed when compiling the dll and another when compiling a
   program using the dll. And of course, these attributes are invalid on
   other platforms or when compiling a static library.

   2. Inline functions are not ansi but are in practice supported by most
   modern compilers, hence it can be a good idea to use them when
   appropriate. However, different platforms want them declared in different
   ways. There is also a problem with instantiating them: On most platforms
   you need to do that separately. Hence the inline.c hack is needed.

   3. There are some other useful extensions that are available on some
   compilers but not on other, e.g., gcc can check the format string given to
   printf() at compile time and give appropriate warnings.
*/
#ifndef _ADIME_INCLUDE_ADIMECFG_H_
#define _ADIME_INCLUDE_ADIMECFG_H_



/* NOT_ADIME_STATICLINK forces shared library mode, and
   ADIME_STATICLINK forces static library mode.
   If none of them is defined, we do like Allegro.
   (This is really recommended: it wouldn't be nice having two dlls around,
   one of which needs allegro to be statically linked and the other needs it
   as a dll!)
*/
#if (defined NOT_ADIME_STATICLINK)
   #ifdef ADIME_STATICLINK
      #error Both ADIME_STATICLINK and NOT_ADIME_STATICLINK are defined.
   #endif
#elif (defined ADIME_STATICLINK)
   /* do nothing */
#elif (defined ALLEGRO_STATICLINK)
   #define ADIME_STATICLINK
#endif



/* define platform-specific declaration stuff */
#if defined ADIME_SCAN_EXPORT

   #define ADIME_VAR(type, name)                       _adime_scanex_dll_data name##_dll
   #define ADIME_ARRAY(type, name)                     _adime_scanex_dll_data name##_dll

   #define ADIME_FUNC(type, name, args)                _adime_scanex_dll_code name##_dll
   #define ADIME_FUNCPTR(type, name, args)             _adime_scanex_dll_data name##_dll

   #define ADIME_BEGIN_INLINE(type, name, args, code)  _adime_scanex_dll_code name##_dll

#elif defined __MINGW32__

   /* describe how function prototypes look to MINGW32 */
   #if (defined ADIME_STATICLINK) || (defined ADIME_SRC)
      #define _ADIME_DLL
   #else
      #define _ADIME_DLL   __declspec(dllimport)
   #endif

   #define ADIME_VAR(type, name)              extern _ADIME_DLL type name
   #define ADIME_ARRAY(type, name)            extern _ADIME_DLL type name[]
   #define ADIME_FUNC(type, name, args)       type __cdecl name args
   #define ADIME_METHOD(type, name, args)     type (*name) args
   #define ADIME_FUNCPTR(type, name, args)    extern _ADIME_DLL type (*name) args

#elif defined __BORLANDC__

   /* horrible hack that avoids too long command line in a broken shell */
   #ifdef ADIME_BCCX
      #define ADIME_DEBUGMODE 1
      #define ADIME_STATICLINK 1
   #endif 

   /* describe how function prototypes look to BCC32 */
   #if (defined ADIME_STATICLINK) || (defined ADIME_SRC)
      #define _ADIME_DLL
   #else
      #define _ADIME_DLL   __declspec(dllimport)
   #endif

   #define ADIME_VAR(type, name)              extern _ADIME_DLL type name
   #define ADIME_ARRAY(type, name)            extern _ADIME_DLL type name[]
   #define ADIME_FUNC(type, name, args)       _ADIME_DLL type __cdecl name args
   #define ADIME_METHOD(type, name, args)     type (__cdecl *name) args
   #define ADIME_FUNCPTR(type, name, args)    extern _ADIME_DLL type (__cdecl *name) args

   #define ADIME_BEGIN_INLINE(type, name, args)        extern __inline type __cdecl name args
   #define ADIME_BEGIN_LOCAL_INLINE(type, name, args)   __inline type name args

   #define ADIME_CONST const

#elif defined _MSC_VER

   #pragma warning (disable: 4200 4244 4305)

   /* describe how function prototypes look to MSVC */
   #if (defined ADIME_STATICLINK) || (defined ADIME_SRC)
      #define _ADIME_DLL
   #else
      #define _ADIME_DLL   __declspec(dllimport)
   #endif

   #define ADIME_VAR(type, name)              extern _ADIME_DLL type name
   #define ADIME_ARRAY(type, name)            extern _ADIME_DLL type name[]
   #define ADIME_FUNC(type, name, args)       _ADIME_DLL type __cdecl name args
   #define ADIME_METHOD(type, name, args)     type (__cdecl *name) args
   #define ADIME_FUNCPTR(type, name, args)    extern _ADIME_DLL type (__cdecl *name) args

   #define ADIME_BEGIN_INLINE(type, name, args, code) \
      __inline _ADIME_DLL type __cdecl name args
   #define ADIME_END_OF_INLINE(type, name, args, code) \
      void *_force_instantiate_##name = name;

   #define ADIME_BEGIN_LOCAL_INLINE(type, name, args) \
      __inline type name args

   #define ADIME_CONST const

#elif defined __WATCOMC__

   #ifndef __SW_3S
      #error Adime only supports stack based calling convention
   #endif

   #ifndef __SW_S
      #error Stack overflow checking must be disabled
   #endif

   #pragma disable_message (120 201)

#elif defined DJGPP

   /* Nothing special needs to be done. */

#elif defined __unix__

   #ifdef __QNX__
        /* Nothing special needs to be done. */
   #else
   /* Include configuration generated by configure script.  */
   /*??#include "alunixac.h"*/
   #endif

#elif defined __BEOS__

   /* Nothing special needs to be done. */

#elif defined __APPLE__

   /* Nothing special needs to be done. */

#else

   #error unknown platform

#endif



/* special definitions for the GCC compiler, independent of machine/OS */
#ifdef __GNUC__
   #ifndef ADIME_BEGIN_INLINE
      #define ADIME_BEGIN_INLINE(type, name, args)  extern inline type name args
   #endif

   #define ADIME_PRINTF_FUNC(type, name, args, a, b) ADIME_FUNC(type, name, args) __attribute__ ((format (printf, a, b)))

   #ifndef ADIME_CONST
      #define ADIME_CONST   const
   #endif

   #define ADIME_CONST_FUNC(type, name, args)       ADIME_FUNC(type, name, args) __attribute__ ((const))

   /* todo: ADIME_BEGIN_LOCAL_INLINE */
#endif



/* fill in some default definitions of language features and helper
   functions, which are conditionalised so they will only be included if none
   of the above headers defined custom versions.
*/
#ifndef ADIME_VAR
   #define ADIME_VAR(type, name)                    extern type name
#endif

#ifndef ADIME_ARRAY
   #define ADIME_ARRAY(type, name)                  extern type name[]
#endif

#ifndef ADIME_FUNC
   #define ADIME_FUNC(type, name, args)             type name args
#endif

#ifndef ADIME_PRINTF_FUNC
   #define ADIME_PRINTF_FUNC(type, name, args, a, b) ADIME_FUNC(type, name, args)
#endif

#ifndef ADIME_CONST_FUNC
   #define ADIME_CONST_FUNC(type, name, args)       ADIME_FUNC(type, name, args)
#endif

#ifndef ADIME_METHOD
   #define ADIME_METHOD(type, name, args)           type (*name) args
#endif

#ifndef ADIME_FUNCPTR
   #define ADIME_FUNCPTR(type, name, args)          extern type (*name) args
#endif

#ifndef ADIME_CONST
   #define ADIME_CONST
#endif



/* Inline declaration stuff. */

/* Does platform support inline? */
#ifndef ADIME_BEGIN_INLINE
   #define ADIME_BEGIN_INLINE(type, name, args)     type name args
   #undef ADIME_HAS_INLINE
#else
   #define ADIME_HAS_INLINE
#endif

/* Should inline code be included in headers? */
#ifdef ADIME_HAS_INLINE
   #define ADIME_INLINE_PROVIDE_CODE
#endif

/* Are we compiling inline.c? */
#ifdef ADIME_INSTANTIATE_INLINE
   #define ADIME_INLINE_PROVIDE_CODE
   #undef ADIME_BEGIN_INLINE
   #undef ADIME_END_OF_INLINE
   #define ADIME_BEGIN_INLINE(type, name, args)  ADIME_FUNC(type, name, args)
#endif

/* It's always safe to put an extra semicolon after inline functions. */
#ifndef ADIME_END_OF_INLINE
   #define ADIME_END_OF_INLINE()  ;
#endif

#ifndef ADIME_END_OF_LOCAL_INLINE
   #define ADIME_END_OF_LOCAL_INLINE()  ;
#endif

/* Define shorter ways to declare inline functions. */
#ifdef ADIME_INLINE_PROVIDE_CODE
   #define ADIME_INLINE(type, name, args, code) \
      ADIME_BEGIN_INLINE(type, name, args) code ADIME_END_OF_INLINE()
#else
   #define ADIME_INLINE(type, name, args, code) \
      ADIME_BEGIN_INLINE(type, name, args) ADIME_END_OF_INLINE()
#endif

#define ADIME_LOCAL_INLINE(type, name, args, code) \
   ADIME_BEGIN_LOCAL_INLINE(type, name, args) code \
   ADIME_END_OF_LOCAL_INLINE()



#endif /* ifndef _ADIME_INCLUDE_ADIMECFG_H_ */
