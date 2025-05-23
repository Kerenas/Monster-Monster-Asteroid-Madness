/* Copyright (C) 2002, 2003 Vincent Penquerc'h.
   This file is part of the alogg library.
   Written by Vincent Penquerc'h <lyrian -at- kezako -dot- net>.

   The alogg library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The alogg library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the alogg Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.

   As a special exception, if you link this library statically with a
   program, you do not have to distribute the object files for this
   program.
   This exception does not however invalidate any other reasons why
   you would have to distribute the object files for this program.  */


#include <allegro.h>

/* Detect whether we're using a version of Allegro suitable
   for registering sample formats */

int main()
{
  register_sample_file_type("",0,0);
  return 0;
}
END_OF_MAIN()
