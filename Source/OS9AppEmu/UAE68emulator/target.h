 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Target specific stuff, Win32 version
  *
  * Copyright 1997 Mathias Ortmann
  */

#define TARGET_NAME "win32"

#define NO_MAIN_IN_MAIN_C

#define OPTIONSFILENAME "default.uae"

#define DEFPRTNAME "LPT1"
#define DEFSERNAME "COM1"

#define PICASSO96_SUPPORTED
#ifndef _MSC_VER
#define BSDSOCKET_SUPPORTED
#endif
