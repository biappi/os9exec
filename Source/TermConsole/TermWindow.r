/*
    Here's a bunch of crap required by the TermWindow code I adapted from the
    Apple demo.  I say "crap" because most of it should be set by the application
    itself, not the console package.  Eventually I'll rip out as much of this
    stuff as I can.     -- Joe

*/

#include "SysTypes.r"
#include "Types.r"

#include "ResourceConstants.h"

#define kMinSize    128             /* application's minimum size (in K) */

/*  We made the preferred size bigger than the minimum size by 12K, so that
    there would be even more room for the scrap, FKEYs, etc. */

#define kPrefSize   256             /* application's preferred size (in K) */


/* these #defines are used to set enable/disable flags of a menu */

#define AllItems    0b1111111111111111111111111111111   /* 31 flags */
#define NoItems     0b0000000000000000000000000000000
#define MenuItem1   0b0000000000000000000000000000001
#define MenuItem2   0b0000000000000000000000000000010
#define MenuItem3   0b0000000000000000000000000000100
#define MenuItem4   0b0000000000000000000000000001000
#define MenuItem5   0b0000000000000000000000000010000
#define MenuItem6   0b0000000000000000000000000100000
#define MenuItem7   0b0000000000000000000000001000000
#define MenuItem8   0b0000000000000000000000010000000
#define MenuItem9   0b0000000000000000000000100000000
#define MenuItem10  0b0000000000000000000001000000000
#define MenuItem11  0b0000000000000000000010000000000
#define MenuItem12  0b0000000000000000000100000000000


// resource 'vers' (1) {
//  0x02, 0x00, release, 0x00,
//  verUS,
//  "0.5",
//  "0.5 by J. Strout (jstrout@ucsd.edu)"
// };


/* we use an MBAR resource to conveniently load all the menus */

resource 'MBAR' (rMenuBar, preload) {
    { mApple, mFile, mEdit };
};


resource 'MENU' (mApple, preload) {
    mApple, textMenuProc,
    AllItems & ~MenuItem2,  /* Disable dashed line, enable About and DAs */
    enabled, apple,
    {
        "About TermConsole…",
            noicon, nokey, nomark, plain;
        "-",
            noicon, nokey, nomark, plain
    }
};

resource 'MENU' (mFile, preload) {
    mFile, textMenuProc,
    MenuItem12,             /* enable Quit only, program enables others */
    enabled, "File",
    {
        "New",
            noicon, "N", nomark, plain;
        "Open",
            noicon, "O", nomark, plain;
        "-",
            noicon, nokey, nomark, plain;
        "Close",
            noicon, "W", nomark, plain;
        "Save",
            noicon, "S", nomark, plain;
        "Save As…",
            noicon, nokey, nomark, plain;
        "Revert",
            noicon, nokey, nomark, plain;
        "-",
            noicon, nokey, nomark, plain;
        "Page Setup…",
            noicon, nokey, nomark, plain;
        "Print…",
            noicon, nokey, nomark, plain;
        "-",
            noicon, nokey, nomark, plain;
        "Quit",
            noicon, "Q", nomark, plain
    }
};

resource 'MENU' (mEdit, preload) {
    mEdit, textMenuProc,
    NoItems,                /* disable everything, program does the enabling */
    enabled, "Edit",
     {
        "Undo",
            noicon, "Z", nomark, plain;
        "-",
            noicon, nokey, nomark, plain;
        "Cut",
            noicon, "X", nomark, plain;
        "Copy",
            noicon, "C", nomark, plain;
        "Paste",
            noicon, "V", nomark, plain;
        "Clear",
            noicon, nokey, nomark, plain;
        "-",
            noicon, nokey, nomark, plain;
        "Terminal Settings…",
            noicon, "E", nomark, plain
    }
};


/* this ALRT and DITL are used as an About screen */

resource 'ALRT' (rAboutAlert, purgeable) {
    {40, 20, 160, 297},
    rAboutAlert,
    { /* array: 4 elements */
        /* [1] */
        OK, visible, silent,
        /* [2] */
        OK, visible, silent,
        /* [3] */
        OK, visible, silent,
        /* [4] */
        OK, visible, silent
    },
    alertPositionParentWindowScreen
};

resource 'DITL' (rAboutAlert, purgeable) {
    { /* array DITLarray: 5 elements */
        /* [1] */
        {88, 185, 108, 265},
        Button {
            enabled,
            "OK"
        },
        /* [2] */
        {8, 8, 24, 214},
        StaticText {
            disabled,
            "TermConsole"
        },
        /* [3] */
        {32, 8, 48, 296},
        StaticText {
            disabled,
            "Copyright © 1996 Joseph J. Strout"
        },
        /* [4] */
        {56, 8, 72, 136},
        StaticText {
            disabled,
            "adapted from code by"
        },
        /* [5] */
        {80, 24, 112, 167},
        StaticText {
            disabled,
            "Craig Hotchkiss & Apple Computer"
        }
    }
};


/* this ALRT and DITL are used as an error screen */

resource 'ALRT' (rUserAlert, purgeable) {
    {40, 20, 120, 260},
    rUserAlert,
    { /* array: 4 elements */
        /* [1] */
        OK, visible, silent,
        /* [2] */
        OK, visible, silent,
        /* [3] */
        OK, visible, silent,
        /* [4] */
        OK, visible, silent
    },
    alertPositionParentWindowScreen
};


resource 'DITL' (rUserAlert, purgeable) {
    { /* array DITLarray: 3 elements */
        /* [1] */
        {50, 150, 70, 230},
        Button {
            enabled,
            "OK"
        },
        /* [2] */
        {10, 60, 30, 230},
        StaticText {
            disabled,
            "Sample - Error occurred!"
        },
        /* [3] */
        {8, 8, 40, 40},
        Icon {
            disabled,
            2
        }
    }
};



resource 'ALRT' (rVT102Alert, purgeable) {
    {40, 20, 170, 330},
    rVT102Alert,
    { /* array: 4 elements */
        /* [1] */
        OK, visible, silent,
        /* [2] */
        OK, visible, silent,
        /* [3] */
        OK, visible, silent,
        /* [4] */
        OK, visible, silent
    },
    alertPositionParentWindowScreen
};


resource 'DITL' (rVT102Alert, purgeable) {
    { /* array DITLarray: 3 elements */
        /* [1] */
        {80, 120, 100, 200},
        Button {
            enabled,
            "Sorry"
        },
        /* [2] */
        {10, 60, 70, 300},
        StaticText {
            disabled,
            "'VT102 Modul' can't be found at\nthe System Extensions Folder !"
        },
        /* [3] */
        {8, 8, 40, 40},
        Icon {
            disabled,
            2
        }
    }
};


resource 'WIND' (rWindow, preload, purgeable) {
    {60, 40, 290, 160},
    noGrowDocProc, visible, noGoAway, 0x0, "Traffic",noAutoCenter
};


/* here is the quintessential MultiFinder friendliness device, the SIZE resource */

#ifdef WANTTHATSTUFF
resource 'SIZE' (-1) {
    dontSaveScreen,
    acceptSuspendResumeEvents,
    enableOptionSwitch,
    canBackground,              /* we can background; we don't currently, but our sleep value */
                                /* guarantees we don't hog the Mac while we are in the background */
    multiFinderAware,           /* this says we do our own activate/deactivate; don't fake us out */
    backgroundAndForeground,    /* this is definitely not a background-only application! */
    dontGetFrontClicks,         /* change this is if you want "do first click" behavior like the Finder */
    ignoreChildDiedEvents,      /* essentially, I'm not a debugger (sub-launching) */
    not32BitCompatible,         /* this app should not be run in 32-bit address space */
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    kPrefSize * 1024,
    kMinSize * 1024 
};
#endif