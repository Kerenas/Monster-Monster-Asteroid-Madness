/* fortify2.c:
   Our own functions to make most possible use of Fortify.
*/
#include <allegro.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "fortify2.h"

/* Must test that __WATCOMC__ is not defined, since DJGPP will be defined
   when we `make depend' for Watcom. */
#if (defined DJGPP) && !(defined __WATCOMC__)
   #include <setjmp.h>
   #include <sys/nearptr.h>
#endif

#if (defined WRAP_SALAD)
   #include <salad.h>
#endif



int always_close_fortify_file = 0, fortify_write_traceback = 0;
static FILE *fortify_file = NULL;
static int fortify2_installed = 0;



#define FORTIFY_LOG  "fortify.log"



#if defined DJGPP

#define MAX_CALLERS           (16384)
#define MAX_TRACEBACK_DEPTH   (4)



/* get_caller:
   If depth==1, returns the address of the function who called the function
   who called `get_caller()'. If depth is greater, returns the address of the
   function earlier on the stack.

   The code is stolen from Allegro (and cut down quite a bit), where in turn
   it was stolen from dpmiexcp.c from the djgpp libc sources, by Charles
   Sandmann.
*/
static unsigned int get_caller(int depth)
{
   extern unsigned int end __asm__ ("end");
   unsigned int *vbp, *vbp_new, *tos;
   jmp_buf j;
   int d = depth, vbp_1;

   setjmp(j);

   tos = (unsigned int *)__djgpp_selector_limit;
   vbp = (unsigned int *)j->__ebp;

   while(((unsigned int)vbp >= j->__esp) && (vbp >= &end) && (vbp < tos)) {
      vbp_new = (unsigned int *)vbp[0];
      if(!vbp_new)
         break;
      vbp_1 = vbp[1];

      if(d-- < 0)
         return vbp_1;
      vbp = vbp_new;
   }
   return 0;
}



#if (defined WRAP_MALLOC) || (defined WRAP_SALAD)
static char callers[MAX_CALLERS][MAX_TRACEBACK_DEPTH * 10 - 1];
static int num_callers = 0;


/* get_caller_name:
*/
static char *get_caller_name(void)
{
   int i;

   if(num_callers < MAX_CALLERS) {
      for(i = 0; i < MAX_TRACEBACK_DEPTH; i++) {
         if(i > 0) {
            callers[num_callers][i * 10 - 2] = '<';
            callers[num_callers][i * 10 - 1] = '-';
         }
         sprintf(callers[num_callers] + i * 10, "%08x", get_caller(1 + i));
      }
      return callers[num_callers++];
   }
   else
      return "<out of filename memory>";
}
#endif



#else /* ifdef DJGPP */



#define get_caller_name()    ("<unknown>")
#define get_caller(depth)    (0)



#endif /* ifdef DJGPP .. else */



#ifdef WRAP_MALLOC



#ifdef __cplusplus
   extern "C" {
#endif



extern void *__real_malloc(size_t size);
extern void *__real_realloc(void *p, size_t size);
extern void *__real_calloc(size_t num_elements, size_t size);
#ifdef FORTIFY_STRDUP
   extern char *__real_strdup(const char *str);
#endif
extern void __real_free(void *p);



/* __wrap_malloc:
   Wrapper function for libc's calls to malloc.
*/
void *__wrap_malloc(size_t size)
{
   if(fortify2_installed)
      return Fortify_malloc(size, get_caller_name(), 0);
   else
      return __real_malloc(size);
}



/* __wrap_calloc:
   Wrapper function for libc's calls to calloc.
*/
void *__wrap_calloc(size_t num_elements, size_t size)
{
   if(fortify2_installed)
      return Fortify_malloc(size, get_caller_name(), 0);
   else
      return __real_calloc(num_elements, size);
}



/* __wrap_strdup:
   Wrapper function for libc's calls to strdup.
*/
char *__wrap_strdup(const char *str)
{
   if(fortify2_installed)
      return Fortify_strdup(str, get_caller_name(), 0);
   else
      return __real_strdup(str);
}



/* __wrap_realloc:
   Wrapper function for libc's calls to realloc.
*/
void *__wrap_realloc(void *p, size_t size)
{
   if(fortify2_installed)
      return Fortify_realloc(p, size, get_caller_name(), 0);
   else
      return __real_realloc(p, size);
}



/* __wrap_free:
   Wrapper function for libc's calls to free.
*/
void __wrap_free(void *p)
{
   if(fortify2_installed)
      Fortify_free(p, get_caller_name(), 0);
   else
      __real_free(p);
}



#ifdef __cplusplus
   }; /* end of extern "C" */
#endif



#endif /* ifdef WRAP_MALLOC */



#ifdef WRAP_SALAD

static void *fortify2_malloc_wrapper(size_t size)
{
   return Fortify_malloc(size, get_caller_name(), 0);
}

static void *fortify2_realloc_wrapper(void *p, size_t size)
{
   return Fortify_realloc(p, size, get_caller_name(), 0);
}

static void fortify2_free_wrapper(void *p)
{
   Fortify_free(p, get_caller_name(), 0);
}

#endif



/* close_fortify_file:
*/
static void close_fortify_file(void)
{
   if(fortify_file != NULL) {
      if(fclose(fortify_file)) {
         fprintf(stderr, "Error in %s.%d\n", __FILE__, __LINE__);
         exit(255);
      }
      fortify_file = NULL;
   }
}



/* fortify_write:
   Our own function for outputting info from fortify.
*/
static void fortify_write(const char *text)
{
   static int virgin = 1;
   int len;
   int did_remove = 0;
   time_t now;
   int i;

   if(virgin)
      did_remove = !remove(FORTIFY_LOG);

   if(fortify_file == NULL) {
      fortify_file = fopen(FORTIFY_LOG, "wa");
      if(fortify_file == NULL) {
         fprintf(stderr, "Error in %s.%d\n", __FILE__, __LINE__);
         exit(255);
      }
   }

   if(virgin) {
      time(&now);
      fprintf(fortify_file,
              "Fortify log started at %s"
              "%s",
              asctime(localtime(&now)),
              did_remove ? "after successfully removing " FORTIFY_LOG ".\n\n":
                           FORTIFY_LOG " didn't exist before this.\n\n");
      virgin = 0;
   }

   if(fortify_write_traceback) {
      for(i = 0; i < 10; i++)
         fprintf(fortify_file, "%08x<-", get_caller(i + 1));
      fprintf(fortify_file, "\n");
   }

   len = strlen(text);
   if(fwrite(text, 1, len, fortify_file) != (size_t)len) {
      fprintf(stderr, "Error in %s.%d\n", __FILE__, __LINE__);
      exit(255);
   }

   if(always_close_fortify_file)
      close_fortify_file();
   else
      fflush(fortify_file);

}



/* fortify_init:
*/
void fortify_init(void)
{
   atexit(fortify_exit);

   fortify2_installed = 1;

   #ifdef WRAP_MALLOC
      Fortify_real_malloc = __real_malloc;
      Fortify_real_realloc = __real_realloc;
      Fortify_real_calloc = __real_calloc;
      #ifdef FORTIFY_STRDUP
         Fortify_real_strdup = __real_strdup;
      #endif
      Fortify_real_free = __real_free;
   #endif

   #ifdef WRAP_SALAD
      _salad_malloc = fortify2_malloc_wrapper;
      _salad_realloc = fortify2_realloc_wrapper;
      _salad_free = fortify2_free_wrapper;
   #endif

   Fortify_SetOutputFunc(fortify_write);
   Fortify_EnterScope();
}



/* fortify_exit:
*/
void fortify_exit(void)
{
   if(fortify2_installed) {
      Fortify_LeaveScope();
      close_fortify_file();

      fortify2_installed = 0;
   }
}
