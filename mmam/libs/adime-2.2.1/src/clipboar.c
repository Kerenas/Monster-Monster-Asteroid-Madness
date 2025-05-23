/*    _      _ _
     /_\  __| (_)_ __  ___
    / _ \/ _` | | '  \/ -_)
   /_/ \_\__,_|_|_|_|_\___|

   clipboar.c:
   This file contains functions for handling a clipboard with generic
   datatypes.

   By Sven Sandberg

   See readme.txt for more information about Adime.
*/
#include <string.h>
#include <allegro.h>

#include "adime.h"
#include "adime/adimeint.h"



/* clipboard_has_contents:
   Returns nonzero if there is data of some type in the clipboard `c'.
*/
int clipboard_has_contents(CLIPBOARD *c)
{
   return ((c != NULL) && c->num_items &&
           (c->item[0].type != CLIPBOARD_TYPE_EMPTY)) ? TRUE : FALSE;
}



/* clipboard_get_data:
   Returns a pointer to data of type `type' of current clipboard item, if it
   has that kind of data.
*/
void *clipboard_get_data(CLIPBOARD *c, int type)
{
   CLIPBOARD_ITEM *ci;

   if (clipboard_has_contents(c))
      for (ci = c->item; ci != NULL; ci = ci->next)
         if ((ci->type == type) && (ci->data != NULL))
            return ci->data;

   return NULL;
}



/* clipboard_has_type:
   Returns TRUE if clipboard contains the type `type'.
*/
int clipboard_has_type(CLIPBOARD *c, int type)
{
   return (clipboard_get_data(c, type) == NULL) ? FALSE : TRUE;
}



/* clipboard_put_item:
   Saves the pointer in clipboard.
*/
int clipboard_put_item(CLIPBOARD *c, void *data, int type,
                       AL_METHOD(void, destroy, (void *data)))
{
   ADIME_ASSERT(c != NULL, ("Clipboard was NULL."));
   ADIME_ASSERT(c->item != NULL, ("Clipboard->item was NULL."));

   /* Moving old clipboard contents. */
   if (c->num_items < c->max_history)
      c->num_items++;
   else
      destroy_clipboard_item(c->item + c->max_history - 1);
   memmove(c->item + 1, c->item,
           (c->max_history - 1) * sizeof(CLIPBOARD_ITEM));

   /* Initializing the new item. */
   c->item[0].type = type;
   c->item[0].data = data;
   c->item[0].destroy = destroy;
   c->item[0].next = NULL;

   return 0;
}



/* clipboard_put_subitem:
   Saves the pointer in clipboard without destroying what already was there.
   This means that there will be two things at the same time in clipboard,
   which might be useful if you want to put something there in more than one
   format.
*/
int clipboard_put_subitem(CLIPBOARD *c, void *data, int type,
                          AL_METHOD(void, destroy, (void *data)))
{
   CLIPBOARD_ITEM *ci;

   ADIME_ASSERT(c != NULL, ("Clipboard was NULL"));
   ADIME_ASSERT(c->item != NULL, ("Clipboard->item was NULL."));

   for (ci = c->item; ci->next; ci = ci->next)
      ; /* Do nothing. */

   _ADIME_MALLOC(ci->next, 1, CLIPBOARD_ITEM);
   ci = ci->next;
   ci->data = data;
   ci->type = type;
   ci->destroy = destroy;
   ci->next = NULL;

   return 0;
}



/* create_history_clipboard:
   Returns a newly created clipboard struct with most fields set to default
   values.
*/
CLIPBOARD *create_history_clipboard(int max_history)
{
   CLIPBOARD *ret;
   int i;

   ADIME_MALLOC(ret, 1, CLIPBOARD);

   ret->max_history = max_history;
   ret->num_items = 0;
   ret->item = ADIME_MALLOC(1, CLIPBOARD_ITEM);

   for (i = 0; i < max_history; i++) {
      ret->item[i].type = CLIPBOARD_TYPE_EMPTY;
      ret->item[i].destroy = NULL;
      ret->item[i].data = NULL;
      ret->item[i].next = NULL;
   }

   return ret;
}



/* create_clipboard:
   Shortcut for creating a clipboard without history.
*/
CLIPBOARD *create_clipboard(void)
{
   return create_history_clipboard(1);
}



/* destroy_clipboard_item:
*/
void destroy_clipboard_item(CLIPBOARD_ITEM *ci)
{
   if (ci) {
      /* We don't want to destroy the very first item, since that is an item
         in the array. */
      destroy_clipboard_item(ci->next);
      free(ci->next);
      if (ci->data)
         if (ci->destroy)
            ci->destroy(ci->data);
   }
}



/* destroy_clipboard:
   Frees all data in a `CLIPBOARD'.
*/
void destroy_clipboard(CLIPBOARD *c)
{
   int i;

   if (c) {
      if (c->item) {
         for (i = 0; i < c->num_items; c++)
            destroy_clipboard_item(c->item + i);
         free(c->item);
      }
      free(c);
   }
}



/* clipboard_put_text:
   Copies text into clipboard.
*/
int clipboard_put_text(CLIPBOARD *c, char *text)
{
   return clipboard_put_item(c, text, CLIPBOARD_TYPE_TEXT, free);
}



/* clipboard_put_bitmap:
   Copies a bitmap into clipboard.
*/
int clipboard_put_bitmap(CLIPBOARD *c, BITMAP *bmp)
{
   ADIME_ASSERT(is_memory_bitmap(bmp),
               ("Only memory bitmaps can be placed in clipboard."));
   return clipboard_put_item(c, bmp, CLIPBOARD_TYPE_BITMAP,
                             (CLIPBOARD_ITEM_DESTROYER)destroy_bitmap);
   return 0;
}



/* clipboard_put_sample:
   Copies a sample into clipboard.
*/
int clipboard_put_sample(CLIPBOARD *c, SAMPLE *s)
{
   return clipboard_put_item(c, s, CLIPBOARD_TYPE_SAMPLE,
                             (CLIPBOARD_ITEM_DESTROYER)destroy_sample);
}
