// 
// Ê ÊOS9exec,   OS-9 emulator for Mac OS, Windows and Linux 
// Ê ÊCopyright (C) 2002 Ê[ Lukas Zeller / Beat Forster ]
//	  Available under http://www.synthesis.ch/os9exec
// 
// Ê ÊThis program is free software; you can redistribute it and/or 
// Ê Êmodify it under the terms of the GNU General Public License as 
// Ê Êpublished by the Free Software Foundation; either version 2 of 
// Ê Êthe License, or (at your option) any later version. 
// 
// Ê ÊThis program is distributed in the hope that it will be useful, 
// Ê Êbut WITHOUT ANY WARRANTY; without even the implied warranty of 
// Ê ÊMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// Ê ÊSee the GNU General Public License for more details. 
// 
// Ê ÊYou should have received a copy of the GNU General Public License 
// Ê Êalong with this program; if not, write to the Free Software 
// Ê ÊFoundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
//

/* This Rez source generates OS9C resources for the
 * common OS9 libraries used by OS9exec/NT based tools
 */

#include "cmdo.r"
#include "mactypes.r"
 
/* os9 program for test */
/* read 'OS9C' (0,"date") ":os9modules:date"; */

/* these are the trap handlers */
/* read 'OS9C' (1,"cio") ":os9modules:cio";   */
/* read 'OS9C' (2,"math") ":os9modules:math"; */
/* read 'OS9C' (3,"csl") ":os9modules:csl";   */

resource 'vers' (1, purgeable) {
        0x3,
        0x20,
        beta,
        0x1,
        verGrSwiss,
        "3.20",
        "OS9exec V3.20 © 1993-2002 by luz/bfo, luz@synthesis.ch / beat.forster@ggaweb.ch"
};

resource 'cmdo' (257, "os9") {
    {   /* array dialogs: 1 elements */
        /* [1] */
        317,
        "os9 is used to start an OS9ª emulation w"
        "ithin MPW. OS9 Programs should reside in"
        " the {OS9CMDS} directory",
        {   /* array itemArray: 8 elements */
            /* [1] */
            NotDependent {

            },
            CheckOption {
                NotSet,
                {171, 35, 191, 150},
                "Dummy F$Fork",
                "-f",
                "Use OS9exec 1.14-style dummy F$Fork (com"
                "mand print to stdout)"
            },
            /* [2] */
            NotDependent {

            },
            RegularEntry {
                "Debug mask (hex)",
                {125, 36, 140, 158},
                {125, 179, 140, 214},
                "1",
                ignoreCase,
                "-d",
                "Set Debug mask in Hex (0=none, 1=anomali"
                "es, others see '-dh' option)"
            },
            /* [3] */
            NotDependent {

            },
            RegularEntry {
                "Stop mask (hex) ",
                {148, 36, 163, 157},
                {148, 179, 163, 214},
                "0",
                ignoreCase,
                "-s",
                "Set Debug-stop mask in Hex (when to stop"
                " execution and present debug menu)"
            },
            /* [4] */
            NotDependent {

            },
            RegularEntry {
                "Extra memory",
                {94, 36, 109, 158},
                {95, 179, 110, 262},
                "0",
                ignoreCase,
                "-m",
                "Set extra memory for first OS9 process ("
                "use K and M for kilo- and megabytes)"
            },
            /* [5] */
            NotDependent {

            },
            CheckOption {
                NotSet,
                {93, 282, 113, 441},
                "Show Usage/Version",
                "-h",
                "Show usage help text and version info"
            },
            /* [6] */
            NotDependent {

            },
            CheckOption {
                NotSet,
                {122, 226, 142, 385},
                "Show Debug help",
                "-dh",
                "Show debug/halt mask code descriptions"
            },
            /* [7] */
            NotDependent {

            },
            RegularEntry {
                "OS9 command line",
                {21, 18, 36, 146},
                {41, 22, 80, 459},
                "",
                ignoreCase,
                "",
                "Enter OS9 command line here"
            },
            /* [8] */
            Or {
                {   /* array OrArray: 2 elements */
                    /* [1] */
                    7,
                    /* [2] */
                    6
                }
            },
            DoItButton {

            }
        }
    }
};

