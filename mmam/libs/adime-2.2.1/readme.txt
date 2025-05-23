
                              _      _ _
                             /_\  __| (_)_ __  ___
                            / _ \/ _` | | '  \/ -_)
                           /_/ \_\__,_|_|_|_|_\___|

                           Allegro Dialogs Made Easy
                                 Version 2.2.1

                                    Readme

                          http://adime.sourceforge.net/

                               by Sven Sandberg



======================================
============ Introduction ============
======================================

Adime is a library for constructing Allegro gui dialogs in a way
similar to scanf(), using a format string.  You only need one function
call to create, e.g., a dialog where the user can enter an integer, a
string, select a file, and select a string from a list.

Other features include (customizable) 3d look and feel, 3d versions of
some of Allegro's gui procs, complete help in many formats (html, txt,
chm, texinfo, man, devhelp, rtf), and four example programs.

Adime can currently be compiled on Linux, Windows/MinGW32,
Windows/Borland C++, Windows/MSVC, DOS/DJGPP, DOS/Watcom, Apple MacOS
X, and QNX.



=================================
============ Licence ============
=================================

The zlib/libpng license: see license.txt for more information.

You are also strongly encouraged to give away the source code of your
program, but you do not have to do so.



======================================
============ Requirements ============
======================================

You need to have installed and compiled Allegro
(http://alleg.sf.net/). You need at least version 4.*.

If you don't have a local copy of the Allegro library (e.g. because
you deleted it to save some space after installation), then download
the dimalleg.zip package from Adime's homepage. Unzip it to the same
place where you unzipped Adime, then continue the installation as
usually.



======================================
============ Installation ============
======================================

See the file corresponding to your platform:

   linux.txt
   mingw32.txt
   bcc32.txt
   msvc.txt
   djgpp.txt
   watcom.txt
   qnx.txt
   macosx.txt

These files are located in the docs/build/ directory.

=========================================
============ Common Problems ============
=========================================

It is easy to give invalid arguments to `adime_dialogf()' by accident,
and the error might not point out the source of the problem very
well. Here is a checklist with the most common problems:

"Everything crashes when I call `adime_dialogf()'."
   This is most likely caused by you passing one of the `...' arguments
   from the wrong type.
    - Check that all "%something[]" format specifiers match the correct type
      of parameter. Remember that you must pass _pointers_ to integers and
      floating point numbers, not just integers or floating point numbers.
      Strings should be `char *', not `char **' though.
    - If your format string spans several rows of your source, make sure
      there isn't an extra comma in the end of one of those lines.
   Also, the crash may not be a crash but rather an assertion that failed in
   one of Adime's routines. If this is true, then an allegro.log file has
   been created which describes the error. It usually means that there is
   something wrong with your format string.

"All input fields in the dialog contain garbage by default."
    - You may have forgotten to initialize some of the variables before
      passing their addresses to `adime_dialogf()'. The values of the `...'
      parameters will be taken as default values in the input fields.
    - If your format string spans several rows of your source, make sure
      there isn't an extra comma in the end of one of those lines.

"The program shuts down and says that an assertion has been raised."
   Then your format string is wrong. An allegro.log file should have been
   created, in which you find an error message that describes what your
   mistake is.

"The program crashes horribly when I write a mathematically undefined
expression (such as sqrt(-1)) or a very large expression (such as exp(1000))
in the expression evaluator."
   As far as I know, this problem only occurs with Watcom (so let me know if
   you are using another platform!) I don't know any easy way to fix it:
   that's simply Watcom's horrible way of handling floating point exceptions.
   Checking for errors before they occur is so hard that it's almost out of
   the question (almost any mathematical operation can go wrong, e.g.
   addition of big enough valid numbers results in overflow). The best
   solution is that you switch to djgpp: it's a much better compiler anyway.



======================================
============ Contact Info ============
======================================

See thanks.txt for email addresses to the developers.

The homepage is at http://adime.sourceforge.net/

