/******************************************************************************
**
**  Project Name:   SampleTermWindow
**     File Name:   TermWindow.h
**
**   Description:   Header file for TermWindow.c
**
**                      Copyright © 1988-1991 Apple Computer, Inc.
**                      All rights reserved.
**
*******************************************************************************
**                       A U T H O R   I D E N T I T Y
*******************************************************************************
**
**  Initials    Name
**  --------    -----------------------------------------------
**  CSH         Craig Hotchkiss
**  JJS         Joseph J. Strout    (jstrout@ucsd.edu)
**
*******************************************************************************
**                      R E V I S I O N   H I S T O R Y
*******************************************************************************
**
**  5/24/96     Added necessary #includes (JJS)
**
******************************************************************************/

#ifndef __TERMWINDOW__
#define __TERMWINDOW__

#include <Terminals.h>

#include <Windows.h>
#include <Controls.h>

extern short TermWinScrolls;
extern short TermExpandNewline;

/********************************************************************************
 *                   Application stuctures - Typedefs
 ********************************************************************************/
struct TermWindowRec {
    WindowRecord   fWindowRec;     // so it can be a WindowPtr
    short          fWindowType;    // for application usage
    TermHandle     fTermHandle;    // CommToolBox terminal handle
    TermEnvironRec fTermEnvirons;  // needed for environs info
    ControlHandle  fVertScroll;    // vertical scroll bar
    ControlHandle  fHorizScroll;   // horizontal scroll bar
    Point          fMinWindowSize; // width/height of window
};
typedef struct TermWindowRec TermWindowRec;
typedef TermWindowRec       *TermWindowPtr;

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************
 *                               PROTOTYPEs
 ********************************************************************************/
pascal OSErr InitTermMgr(void);

pascal OSErr NewTermWindow(TermWindowPtr   *termPtr,
                           const Rect      *boundsRect,
                           ConstStr255Param title,
                           Boolean          visible,
                           short            theProc,
                           WindowPtr        behind,
                           Boolean          goAwayFlag,
                           Str31            toolName);
pascal void  DisposeTermWindow(TermWindowPtr termPtr);

pascal Boolean ConfigTerminal(TermWindowPtr *termPtr);
pascal OSErr   SaveTerminalSettings(TermWindowPtr *termPtr, char *Prefix);
pascal OSErr   LoadTerminalSettings(TermWindowPtr *termPtr, char *Prefix);
pascal void
DoZoomTermWindow(TermWindowPtr termPtr, short zoomFactor, int left, int top);

pascal Boolean IsTermWindowEvent(TermWindowPtr      termPtr,
                                 const EventRecord *theEventPtr);
pascal void    HandleTermWindowEvent(TermWindowPtr      termPtr,
                                     const EventRecord *theEventPtr);

pascal OSErr WriteToTermWindow(TermWindowPtr termPtr,
                               Ptr           theData,
                               Size         *lengthOfData);

pascal OSErr PStringToTermWindow(TermWindowPtr termPtr, unsigned char *pstr);

pascal void FillTermWindow(TermWindowPtr termPtr);

#ifdef __cplusplus
}
#endif

#endif __TERMWINDOW__
