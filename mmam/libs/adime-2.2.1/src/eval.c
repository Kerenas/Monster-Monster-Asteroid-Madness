/*    _      _ _
 *   /_\  __| (_)_ __  ___
 *  / _ \/ _` | | '  \/ -_)
 * /_/ \_\__,_|_|_|_|_\___|
 *
 * This file is distributed with the Adime library, but it was written by
 * Shawn Hargreaves for the EGG system (version 1.0, last updated July
 * 1999). For more information about EGG, see http://www.talula.demon.co.uk/
 *
 * See readme.txt for more information about Adime.
 */

/*
 *  EVAL.C - part of the EGG system.
 *
 *  Helper routine for evaluating arithmetic expressions.
 *
 *  By Shawn Hargreaves.
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

/* The following includes were added when moving the file to Adime. Also,
   the inclusion of "egg.h" was removed. I also added some lines of
   `evaluate_error = TRUE;' in `add_operator()'.
*/
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"

#ifndef M_PI
#define M_PI      3.14159265358979323846
#endif

#ifndef M_E
#define M_E       2.7182818284590452354
#endif



/* convert radians <-> degrees */
#define DEG2RAD(x)   ((x)*M_PI/180.0)
#define RAD2DEG(x)   ((x)*180.0/M_PI)



/* internal state information */
static int evaluate_error;

static char operator_stack[256];
static double value_stack[256];

static int stack_depth;

static double current_val;

static int current_valid;



/* operator tokens */
#define OP_OPEN_PAREN      '('
#define OP_CLOSE_PAREN     ')'
#define OP_PLUS            '+'
#define OP_MINUS           '-'
#define OP_MUL             '*'
#define OP_DIV             '/'
#define OP_POWER           '^'
#define OP_NEGATE          '~'
#define OP_SQRT            'q'
#define OP_SIN             's'
#define OP_COS             'c'
#define OP_TAN             't'
#define OP_ASIN            'S'
#define OP_ACOS            'C'
#define OP_ATAN            'T'
#define OP_LOG             'l'
#define OP_LN              'L'
#define OP_CEIL            'e'
#define OP_FLOOR           'f'
#define OP_ROUND           'r'
#define OP_ABS             'a'
#define OP_MOD             '%'
#define OP_EQUALS          '='
#define OP_NOT_EQUALS      '#'
#define OP_LESS            '<'
#define OP_GREATER         '>'
#define OP_LESS_EQUALS     '{'
#define OP_GREATER_EQUALS  '}'
#define OP_OR              '|'
#define OP_AND             '&'
#define OP_NOT             '!'
#define OP_TERMINATOR      ' '



/* precedence:
 *  Returns the precedence of the specified operator.
 */
static int precedence(char op)
{
   switch (op) {

      case OP_OPEN_PAREN:
      case OP_CLOSE_PAREN:
         return 1;

      case OP_TERMINATOR:
         return 2;

      case OP_OR:
      case OP_AND:
         return 3;

      case OP_EQUALS:
      case OP_NOT_EQUALS:
      case OP_LESS:
      case OP_GREATER:
      case OP_LESS_EQUALS:
      case OP_GREATER_EQUALS:
         return 4;

      case OP_PLUS:
      case OP_MINUS:
         return 5;

      case OP_MUL:
      case OP_DIV:
      case OP_MOD:
         return 6;

      case OP_POWER:
         return 7;

      case OP_NEGATE:
      case OP_NOT:
      case OP_SQRT:
      case OP_SIN:
      case OP_COS:
      case OP_TAN:
      case OP_ASIN:
      case OP_ACOS:
      case OP_ATAN:
      case OP_LOG:
      case OP_LN:
      case OP_CEIL:
      case OP_FLOOR:
      case OP_ROUND:
      case OP_ABS:
         return 8;

      default:
         return -1;
   }
}



/* is_unary:
 *  Checks whether an operator is unary.
 */
static int is_unary(char op)
{
   if ((op == OP_NEGATE) || (op == OP_SQRT) || (op == OP_SIN)   ||
       (op == OP_COS)    || (op == OP_TAN)  || (op == OP_ASIN)  ||
       (op == OP_ACOS)   || (op == OP_ATAN) || (op == OP_LOG)   ||
       (op == OP_LN)     || (op == OP_CEIL) || (op == OP_FLOOR) ||
       (op == OP_ROUND)  || (op == OP_ABS)  || (op == OP_NOT))
      return TRUE;

   return FALSE;
}



/* add_value:
 *  Processes a new numeric value from the input string.
 */
static void add_value(double val)
{
   if (current_valid) {
      evaluate_error = TRUE;
      return;
   }

   current_val = val;
   current_valid = TRUE;
}



/* add_operator:
 *  Processes a new operator from the input string.
 */
static void add_operator(char op)
{
   /* bodge for unary negation */
   if ((op == OP_MINUS) && (!current_valid))
      op = OP_NEGATE;

   /* check validity */
   if ((op == OP_PLUS)        || (op == OP_MINUS)          ||
       (op == OP_MUL)         || (op == OP_DIV)            ||
       (op == OP_POWER)       || (op == OP_MOD)            ||
       (op == OP_EQUALS)      || (op == OP_NOT_EQUALS)     ||
       (op == OP_LESS)        || (op == OP_GREATER)        ||
       (op == OP_LESS_EQUALS) || (op == OP_GREATER_EQUALS) ||
       (op == OP_OR)          || (op == OP_AND)) {

      if (!current_valid) {
         evaluate_error = TRUE;
         return;
      }
   }

   /* evaluate */
   if (op != OP_OPEN_PAREN) {
      while ((stack_depth > 0) &&
             ((precedence(op) <= precedence(operator_stack[stack_depth-1]) && (!is_unary(operator_stack[stack_depth-1]))) ||
              (precedence(op) < precedence(operator_stack[stack_depth-1]) && (is_unary(operator_stack[stack_depth-1]))))) {

         stack_depth--;

         switch (operator_stack[stack_depth]) {

            case OP_PLUS:
               current_val = value_stack[stack_depth] + current_val;
               break;

            case OP_MINUS:
               current_val = value_stack[stack_depth] - current_val;
               break;

            case OP_MUL:
               current_val = value_stack[stack_depth] * current_val;
               break;

            case OP_DIV:
               if (current_val != 0)
                  current_val = value_stack[stack_depth] / current_val;
               else {
                  evaluate_error = TRUE;
                  current_val = 0;
               }
               break;

            case OP_POWER:
               current_val = pow(value_stack[stack_depth], current_val);
               break;

            case OP_NEGATE:
               current_val = -current_val;
               break;

            case OP_SQRT:
               if (current_val >= 0)
                  current_val = sqrt(current_val);
               else {
                  evaluate_error = TRUE;
                  current_val = 0;
               }
               break;

            case OP_SIN:
               current_val = sin(DEG2RAD(current_val));
               break;

            case OP_COS:
               current_val = cos(DEG2RAD(current_val));
               break;

            case OP_TAN:
               current_val = tan(DEG2RAD(current_val));
               break;

            case OP_ASIN:
               if ((current_val >= -1) && (current_val <= 1))
                  current_val = RAD2DEG(asin(current_val));
               else {
                  evaluate_error = TRUE;
                  current_val = 0;
               }
               break;

            case OP_ACOS:
               if ((current_val >= -1) && (current_val <= 1))
                  current_val = RAD2DEG(acos(current_val));
               else {
                  current_val = 0;
                  evaluate_error = TRUE;
               }
               break;

            case OP_ATAN:
               current_val = RAD2DEG(atan(current_val));
               break;

            case OP_LOG:
               if (current_val > 0)
                  current_val = log10(current_val);
               else {
                  current_val = 0;
                  evaluate_error = TRUE;
               }
               break;

            case OP_LN:
               if (current_val > 0)
                  current_val = log(current_val);
               else {
                  current_val = 0;
                  evaluate_error = TRUE;
               }
               break;

            case OP_CEIL:
               current_val = ceil(current_val);
               break;

            case OP_FLOOR:
               current_val = floor(current_val);
               break;

            case OP_ROUND:
               if (current_val < 0)
                  current_val = (int)(current_val - 0.5);
               else
                  current_val = (int)(current_val + 0.5);
               break;

            case OP_ABS:
               current_val = fabs(current_val);
               break;

            case OP_MOD:
               if (current_val >= 1)
                  current_val = fmod(value_stack[stack_depth], current_val);
               else {
                  current_val = 0;
                  evaluate_error = TRUE;
               }
               break;

            case OP_EQUALS:
               current_val = (value_stack[stack_depth] == current_val);
               break;

            case OP_NOT_EQUALS:
               current_val = (value_stack[stack_depth] != current_val);
               break;

            case OP_LESS:
               current_val = (value_stack[stack_depth] < current_val);
               break;

            case OP_GREATER:
               current_val = (value_stack[stack_depth] > current_val);
               break;

            case OP_LESS_EQUALS:
               current_val = (value_stack[stack_depth] <= current_val);
               break;

            case OP_GREATER_EQUALS:
               current_val = (value_stack[stack_depth] >= current_val);
               break;

            case OP_OR:
               current_val = ((int)value_stack[stack_depth] || (int)current_val);
               break;

            case OP_AND:
               current_val = ((int)value_stack[stack_depth] && (int)current_val);
               break;

            case OP_NOT:
               current_val = !(int)current_val;
               break;

            case OP_OPEN_PAREN:
               if (op == OP_CLOSE_PAREN)
                  return;
               break;
         }
      }
   }

   /* push onto the stack */
   if (op != OP_CLOSE_PAREN) {
      operator_stack[stack_depth] = op;
      value_stack[stack_depth] = current_val;
      stack_depth++;
      current_val = 0;
      current_valid = FALSE;
   }
   else {
      if (stack_depth <= 0)
         evaluate_error = TRUE;
   }
}



/* adime_evaluate:
 *  Top level evaluation function.
 */
double adime_evaluate(ADIME_CONST char *equation, int *error,
                     double (*variable)(ADIME_CONST char *name))
{
   #define MAX_IDENTIFIER_SIZE 256
   char buf[MAX_IDENTIFIER_SIZE];
   double val;
   int i;
   char *end_p;

   stack_depth = 0;
   current_val = 0;
   current_valid = FALSE;
   evaluate_error = FALSE;

   while ((*equation) && (!evaluate_error)) {
      /* skip whitespace */
      while (isspace(*equation))
         equation++;

      switch (*equation) {

         case '+':
            /* addition */
            add_operator(OP_PLUS);
            equation++;
            break;

         case '-':
            /* subtraction */
            add_operator(OP_MINUS);
            equation++;
            break;

         case '*':
            /* multiplication */
            add_operator(OP_MUL);
            equation++;
            break;

         case '/':
            /* division */
            add_operator(OP_DIV);
            equation++;
            break;

         case '^':
            /* rasing to a power (_not_ XOR!) */
            add_operator(OP_POWER);
            equation++;
            break;

         case '%':
            /* modulus */
            add_operator(OP_MOD);
            equation++;
            break;

         case '|':
            /* logical or */
            add_operator(OP_OR);
            equation++;
            break;

         case '&':
            /* logical and */
            add_operator(OP_AND);
            equation++;
            break;

         case '=':
            /* equality test (requires dual ==) */
            if (equation[1] == '=') {
               add_operator(OP_EQUALS);
               equation += 2;
            }
            else
               evaluate_error = TRUE;
            break;

         case '!':
            /* could be inequality test or logical not */
            if (equation[1] == '=') {
               add_operator(OP_NOT_EQUALS);
               equation += 2;
            }
            else {
               add_operator(OP_NOT);
               equation++;
            }
            break;

         case '<':
            /* could be less than or less/equal test */
            if (equation[1] == '=') {
               add_operator(OP_LESS_EQUALS);
               equation += 2;
            }
            else {
               add_operator(OP_LESS);
               equation++;
            }
            break;

         case '>':
            /* could be greater than or greater/equal test */
            if (equation[1] == '=') {
               add_operator(OP_GREATER_EQUALS);
               equation += 2;
            }
            else {
               add_operator(OP_GREATER);
               equation++;
            }
            break;

         case '(':
            /* open bracket */
            add_operator(OP_OPEN_PAREN);
            equation++;
            break;

         case ')':
            /* close bracket */
            add_operator(OP_CLOSE_PAREN);
            equation++;
            break;

         case '0':
            /* special case for hex constants (0x prefix) */
            if (equation[1] == 'x') {
               equation += 2;

               for (i=0; (i<MAX_IDENTIFIER_SIZE-1) &&
                         (isxdigit(equation[i])); i++)
                  buf[i] = equation[i];

               buf[i] = 0;

               while(isxdigit(equation[i]))
                  i++;
               equation += i;

               val = strtol(buf, NULL, 16);
               add_value(val);
               break;
            }
            /* else fall through */

         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
      #if 1
            val = strtod(equation, &end_p);
            equation = end_p;
            add_value(val);
            break;
      #else
         /* todo: why does original version use this code? Is the handling
            of exponentials portable?
         */
            /* floating point constant */
            for (i=0; (i<MAX_IDENTIFIER_SIZE-1) &&
                      (isdigit(equation[i]) || (equation[i] == '.')); i++)
               buf[i] = equation[i];

            buf[i] = 0;
            while(isdigit(equation[i]) || (equation[i] == '.'))
               i++;
            equation += i;

            val = atof(buf);
            al_trace("%d: %f\n", __LINE__, val);
            add_value(val);
            break;
      #endif

         default:
            /* this is a string, could be a variable or function */
            for (i=0; (i<MAX_IDENTIFIER_SIZE-1) &&
                      ((isalpha(equation[i])) || (equation[i] == '_')); i++)
               buf[i] = tolower(equation[i]);

            buf[i] = 0;
            while(isalpha(equation[i]) || (equation[i] == '_'))
               i++;
            equation += i;

            if (strcmp(buf, "pi") == 0) {
               /* pi (built in constant) */
               add_value(M_PI);
            }
            else if (strcmp(buf, "e") == 0) {
               /* e (built in constant) */
               add_value(M_E);
            }
            else if (strcmp(buf, "sqrt") == 0) {
               /* square root function */
               add_operator(OP_SQRT);
            }
            else if (strcmp(buf, "sin") == 0) {
               /* sin function */
               add_operator(OP_SIN);
            }
            else if (strcmp(buf, "cos") == 0) {
               /* cos function */
               add_operator(OP_COS);
            }
            else if (strcmp(buf, "tan") == 0) {
               /* tan function */
               add_operator(OP_TAN);
            }
            else if (strcmp(buf, "asin") == 0) {
               /* inverse sin function */
               add_operator(OP_ASIN);
            }
            else if (strcmp(buf, "acos") == 0) {
               /* inverse cos function */
               add_operator(OP_ACOS);
            }
            else if (strcmp(buf, "atan") == 0) {
               /* inverse tan function */
               add_operator(OP_ATAN);
            }
            else if (strcmp(buf, "log") == 0) {
               /* base 10 logarithm function */
               add_operator(OP_LOG);
            }
            else if (strcmp(buf, "ln") == 0) {
               /* natural logarithm function */
               add_operator(OP_LN);
            }
            else if (strcmp(buf, "ceil") == 0) {
               /* round upwards function */
               add_operator(OP_CEIL);
            }
            else if (strcmp(buf, "floor") == 0) {
               /* round downwards function */
               add_operator(OP_FLOOR);
            }
            else if (strcmp(buf, "round") == 0) {
               /* round to nearest integer function */
               add_operator(OP_ROUND);
            }
            else if (strcmp(buf, "abs") == 0) {
               /* absolute value function */
               add_operator(OP_ABS);
            }
            else if (strcmp(buf, "rand") == 0) {
               /* random number between 0 and 1 */
               add_value((rand()&32767)/32767.0);
            }
            else {
               /* user-supplied callback for looking up variables */
               if ((buf[0]) && (variable)) {
                  add_value(variable(buf));
               }
               else {
                  if (error)
                     *error = TRUE;

                  return 0;
               }
            }
            break;
      }
   }

   if ((evaluate_error) || (!current_valid)) {
      if (error)
         *error = TRUE;

      return 0;
   }

   /* force a stack flush */
   add_operator(OP_TERMINATOR);

   if (stack_depth != 1) {
      if (error)
         *error = TRUE;

      return 0;
   }

   if (error)
      *error = FALSE;

   return value_stack[0];
}
