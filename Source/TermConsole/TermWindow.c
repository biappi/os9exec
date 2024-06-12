/******************************************************************************
**                      P R O J E C T   I N F O
*******************************************************************************
**
**  Project Name:   SampleTermWindow
**     File Name:   TermWindow.c
**
**   Description:   This file along with the header and resource file contain
**                  the source code for a fully functional, but simple,
**                  CommToolBox terminal window.  The codes purpose is to
**                  enable developers easy access to a simple terminal window.
**
**                      Copyright © 1988-1991 Apple Computer, Inc.
**                      All rights reserved.
**
*******************************************************************************
**                      A U T H O R   I D E N T I T Y
*******************************************************************************
**
**  Initials    Name
**  --------    -----------------------------------------------
**  CSH         Craig Hotchkiss
**  JJS         Joseph J. Strout (jstrout@ucsd.edu)
**
*******************************************************************************
**                      R E V I S I O N   H I S T O R Y
*******************************************************************************

    05/22/96: replaced ProcPtr with ControlActionUPP in TrackControl call (JJS)
    05/25/96: added PStringToTermWindow() function (JJS)
    06/30/96: added TermWinScrolls variable and associated checks (JJS)
    06/30/96: reworked error-trapping methods (JJS)
    06/30/96: added forward declarations of functions (JJS)
    07/30/96: fixed ControlActionUPP usage (added ...UPP globals) (JJS)

******************************************************************************/

// Identifier for CW Pro 7 identification
// 0xXXYY meaning Compiler Version X.X, rev YY
#define CW7_MWERKS 0x2405 // V 2.4.5 = CW Pro 7

#ifdef DUMPFILENAME
#pragma load DUMPFILENAME
#else
#include <Menus.h>
#include <CommResources.h>
#include <CTBUtilities.h>
#include <Gestalt.h>
#include <Terminals.h>
#include <Controls.h>

#if __MWERKS__ >= CW7_MWERKS
#include <ControlDefinitions.h>
#endif

#include <Folders.h>
#endif

#include <stdio.h>
#include <string.h>
#include <Fsp_fopen.h>

#ifndef __TERMWINDOW__
#include "TermWindow.h"
#endif

#include "ResourceConstants.h"

short TermWinScrolls = 0;

ControlActionUPP TermWinHorUPP = NULL;
ControlActionUPP TermWinVerUPP = NULL;

/* forward declarations */
Boolean     IsNotAWindowProc(short windowProcID);
void        AdjustTermRect(Rect *theAdjustRect);
void        FixVertScrollTermWindow(TermWindowPtr termPtr);
void        FixHorizScrollTermWindow(TermWindowPtr termPtr);
void        CellRectToPixelRect(TermWindowPtr termPtr, Rect *theRect);
short       TermWindowSizeInLines(TermWindowPtr termPtr);
short       TermWindowSizeInCols(TermWindowPtr termPtr);
pascal void CommonActionTermWindow(ControlHandle theControl, short *amount);
void        AdjustHorizScrollTermWindow(TermWindowPtr termPtr);
pascal void HorizScrollTextTermWindow(ControlHandle theControl, short part);
void DoHorizScrollTermWindow(TermWindowPtr termPtr, short part, Point point);
void AdjustVertScrollTermWindow(TermWindowPtr termPtr);
pascal void VertScrollTextTermWindow(ControlHandle theControl, short part);
void    DoVertScrollTermWindow(TermWindowPtr termPtr, short part, Point point);
void    ContentClickTermWindow(TermWindowPtr      termPtr,
                               const EventRecord *theEventPtr);
void    DragTermWindow(TermWindowPtr termPtr, Point where);
Boolean IsNOTAWindowProc(short windowProcID);
void    ResizeTermWindow(TermWindowPtr termPtr, Point where);
void    ZoomTermWindow(TermWindowPtr termPtr, short zoomFactor, Point where);
void MouseDownTermWindow(TermWindowPtr termPtr, const EventRecord *theEventPtr);

void TerminalFullSize(TermWindowPtr termPtr, Point *SizePointPtr);

/********************************************************************************
                            Useful Macros
********************************************************************************/
// Replaces the Mac routines HiWord, LoWord
#define HiWrd(aLong) (((aLong) >> 16) & 0xFFFF)
#define LoWrd(aLong) ((aLong)&0xFFFF)

/********************************************************************************
    Procedure:  IsNOTAWindowProc
    Purpose:    This routine makes sure that the procID is contained within our
                list of known window proc types.
    Passed:     windowProcID -- This is the window type procID.
    Returns:    false if the procID matches a known window type, true if it
                does not.
********************************************************************************/
Boolean IsNOTAWindowProc(short windowProcID)
{
    switch (windowProcID) {
    case documentProc:
    case dBoxProc:
    case plainDBox:
    case altDBoxProc:
    case noGrowDocProc:
    case movableDBoxProc:
    case zoomDocProc:
    case zoomNoGrow:
    case rDocProc:
        return false;
        break;

    default:
        return true;
        break;
    }
} /*IsNOTAWindowProc*/

/********************************************************************************
    Procedure:  AdjustTermRect
    Purpose:    This routine adjusts the right and bottom portions of the rect
                    passed.  I suppose macros might be better.
    Passed:     theAdjustRect -- This is a pointer to the rectangle that
                    needs adjusting.
    Returns:    void
********************************************************************************/
void AdjustTermRect(Rect *theAdjustRect)
{
    (*theAdjustRect).right -= 15;
    (*theAdjustRect).bottom -= 15;
} /*AdjustTermRect*/

/********************************************************************************
    Procedure:  InitTermMgr
    Purpose:    Checks via Gestalt for the manager and initializes not only the
                    terminal manager, but CRM and CTB Utilities.
    Passed:     void
    Returns:    An OSErr indicating any problems
********************************************************************************/
pascal OSErr InitTermMgr(void)
{
    OSErr   result = noErr;
    Boolean hasCTB, hasCRM, hasTM;
    long    gestaltResult;

    // these ternary C operations do the assignments nicely here.
    //   gestaltCTBVersion   --  returns CTB version #
    //   gestaltCRMAttr      --  indicates that it exists
    //   gestaltTermMgrAttr  --  indicates that it exists
    hasCTB = (Gestalt(gestaltCTBVersion, &gestaltResult) ? false
                                                         : gestaltResult > 0);
    hasCRM =
        (Gestalt(gestaltCRMAttr, &gestaltResult) ? false : gestaltResult != 0);
    hasTM = (Gestalt(gestaltTermMgrAttr, &gestaltResult) ? false
                                                         : gestaltResult != 0);

    // Each operation here  is dependent upon successful completion of
    //   the others.
    if ((hasCTB) && (hasCRM) && (hasTM)) {
        if (noErr == (result = InitCRM())) {
            if (noErr == (result = InitCTBUtilities())) {
                // finally!
                if (noErr == (result = InitTM())) {
                }
                else {
                    result = 0; /* will be caught later on */
                    // OSErr at InitTM/InitTermMgr
                }
            }
            else {
                // OSErr at InitCTBUtilities/InitTermMgr
            }
        }
        else {
            // OSErr at InitCRM/InitTermMgr
        }
    }
    else {
        // Must not have everthing we need.
        result = -1;
    }

    return (result);
} /*InitTermMgr*/

/********************************************************************************
    Procedure:  FixVertScrollTermWindow
    Purpose:    Fixes the vertical scroll bar to the current window size.
    Passed:     termPtr:
                    A pointer to the common structure
    Returns:    void
********************************************************************************/
void FixVertScrollTermWindow(TermWindowPtr termPtr)
{
    MoveControl(termPtr->fVertScroll,
                ((WindowPtr)termPtr)->portRect.right - (16 - 1),
                -1);

    SizeControl(termPtr->fVertScroll,
                16,
                (((WindowPtr)termPtr)->portRect.bottom + 1) -
                    (((WindowPtr)termPtr)->portRect.top - 1) - (16 - 1));
} /*FixVertScrollTermWindow*/

/********************************************************************************
    Procedure:  FixHorizScrollTermWindow
    Purpose:    Fixes the horizontal scroll bar to the current window size.
    Passed:     termPtr:
                    A pointer to the common structure
    Returns:    void
********************************************************************************/
void FixHorizScrollTermWindow(TermWindowPtr termPtr)
{
    MoveControl(termPtr->fHorizScroll,
                ((WindowPtr)termPtr)->portRect.left - 1,
                ((WindowPtr)termPtr)->portRect.bottom - 15);

    SizeControl(termPtr->fHorizScroll,
                (((WindowPtr)termPtr)->portRect.right + 1) - 14,
                16);
} /*FixHorizScrollTermWindow*/

/********************************************************************************
    Procedure:  CellRectToPixelRect
    Purpose:    Modifies the cell-based rectangle passed so the measurements
                    become pixels.
    Passed:     termPtr:
                    A pointer to the common structure
                theRect:
                    The rectangle to be altered
    Returns:    void
********************************************************************************/
void CellRectToPixelRect(TermWindowPtr termPtr, Rect *theRect)
{
    // A utility that turns a 1-based cell/row arrangement into pixel
    //   measurements.  The subtraction of 1 from the top and left sides
    //   perform the conversion to a zero-base and the multiplication
    //   produces the real number.
    (*theRect).top = (((*theRect).top - 1) * termPtr->fTermEnvirons.cellSize.v);
    (*theRect).bottom =
        (((*theRect).bottom) * termPtr->fTermEnvirons.cellSize.v);

    (*theRect).left =
        (((*theRect).left - 1) * termPtr->fTermEnvirons.cellSize.h);
    (*theRect).right = (((*theRect).right) * termPtr->fTermEnvirons.cellSize.h);
} /*CellRectToPixelRect*/

/********************************************************************************
    Procedure:  TermWindowSizeInLines
    Purpose:    Measure the height of the window in lines.
    Passed:     termPtr:
                    A pointer to the common structure
    Returns:    An integer indicating the number of lines.
********************************************************************************/
short TermWindowSizeInLines(TermWindowPtr termPtr)
{
    short windowSize, lineSize, pixelSize, ourSize;
    Rect  ourRect, termRect;

    termRect = (**(termPtr->fTermHandle)).termRect;

    ourRect = ((WindowPtr)termPtr)->portRect;
    AdjustTermRect(&ourRect);
    pixelSize = ourRect.bottom - ourRect.top;
    lineSize  = termPtr->fTermEnvirons.cellSize.v;

    ourSize = pixelSize / lineSize;

    if (ourSize > termPtr->fTermEnvirons.textRows) {
        windowSize = termPtr->fTermEnvirons.textRows;
    }
    else {
        windowSize = ourSize;
    }

    return (windowSize);
} /*TermWindowSizeInLines*/

/********************************************************************************
    Procedure:  TermWindowSizeInLines
    Purpose:    Returns the pixel sizes for displaying the full terminal
    Passed:     termPtr:
                    A pointer to the common structure
    Returns:    Updated SizePointPtr.
********************************************************************************/
void TerminalFullSize(TermWindowPtr termPtr, Point *SizePointPtr)
{
    struct TermRecord *trP;
    /*  SizePointPtr->h =
            termPtr->fTermEnvirons.graphicSize.right-
            termPtr->fTermEnvirons.graphicSize.left;
        SizePointPtr->v =
            termPtr->fTermEnvirons.graphicSize.bottom-
            termPtr->fTermEnvirons.graphicSize.top; */
    trP = *(termPtr->fTermHandle);

    SizePointPtr->h = trP->viewRect.right;
    SizePointPtr->v = trP->viewRect.bottom;
}

/********************************************************************************
    Procedure:  TermWindowSizeInCols
    Purpose:    Measure the height of the window in columns.
    Passed:     termPtr:
                    A pointer to the common structure
    Returns:    An integer indicating the number of columns.
********************************************************************************/
short TermWindowSizeInCols(TermWindowPtr termPtr)
{
    short windowSize, columnSize, pixelSize, ourSize;
    Rect  ourRect, termRect;

    termRect = (**(termPtr->fTermHandle)).termRect;

    ourRect = ((WindowPtr)termPtr)->portRect;
    AdjustTermRect(&ourRect);
    pixelSize  = ourRect.right - ourRect.left;
    columnSize = termPtr->fTermEnvirons.cellSize.h;

    ourSize = pixelSize / columnSize;

    if (ourSize > termPtr->fTermEnvirons.textCols) {
        windowSize = termPtr->fTermEnvirons.textCols;
    }
    else {
        windowSize = ourSize;
    }

    return (windowSize);
} /*TermWindowSizeInCols*/

/********************************************************************************
    Procedure:  CommonActionTermWindow
    Purpose:    Modifies the control value by amount and nudges amount in the
                    proper direction.
    Passed:     theControl --   Handle to the control we are modifying
                amount --       Amount of modification
    Returns:    amount --       reflects new control change.
********************************************************************************/
pascal void CommonActionTermWindow(ControlHandle theControl, short *amount)
{
    short controlValue, controlMax;

    controlValue = GetControlValue(theControl);
    controlMax   = GetControlMaximum(theControl);

    *amount = controlValue - *amount;

    if (*amount < 0)
        *amount = 0;
    else if (*amount > controlMax)
        *amount = controlMax;

    SetControlValue(theControl, *amount);

    *amount = controlValue - *amount;
} /*CommonActionTermWindow*/

/********************************************************************************
    Procedure:  AdjustHorizScrollTermWindow
    Purpose:    Adjusts the horiz scroll bar
    Passed:     termPtr:
                    A pointer to the common structure
    Returns:    void
********************************************************************************/
void AdjustHorizScrollTermWindow(TermWindowPtr termPtr)
{
    short oldScroll, newScroll, maxTop;
    Rect  ourVisRect;

    maxTop = (termPtr->fTermEnvirons.textCols) - TermWindowSizeInCols(termPtr);

    if (maxTop <= 0) {
        maxTop = 0;
        HiliteControl(termPtr->fHorizScroll, 255); // inactive
    }
    else {
        if (maxTop > termPtr->fTermEnvirons.textCols) {
            maxTop = termPtr->fTermEnvirons.textCols;
        }

        HiliteControl(termPtr->fHorizScroll, 0); // active
    }

    SetControlMaximum(termPtr->fHorizScroll, maxTop);

    ourVisRect = (**(termPtr->fTermHandle)).visRect;
    CellRectToPixelRect(termPtr, &ourVisRect);

    oldScroll = ourVisRect.left - (**(termPtr->fTermHandle)).viewRect.left;
    newScroll = (GetControlValue(termPtr->fHorizScroll)) *
                (termPtr->fTermEnvirons.cellSize.h);

    // TMScroll(termPtr->fTermHandle, (oldScroll - newScroll), 0);
} /*AdjustHorizScrollTermWindow*/

/********************************************************************************
    Procedure:  HorizScrollTextTermWindow
    Purpose:    Makes sure that the control movement happens correctly and the
                    text is scrolled.
    Passed:     termPtr:
                    A pointer to the common structure
    Returns:    void
********************************************************************************/
pascal void HorizScrollTextTermWindow(ControlHandle theControl, short part)
{
    short         delta, oldValue;
    TermWindowPtr termPtr;

    termPtr = (TermWindowPtr)GetControlReference(theControl);

    oldValue = GetControlValue(termPtr->fHorizScroll);

    switch (part) {
    case kControlUpButtonPart:
    case kControlDownButtonPart:
        delta = 1;
        break;

    case kControlPageUpPart:
    case kControlPageDownPart:
        delta = TermWindowSizeInCols(termPtr);
        break;

    default:
        break;
    }

    if (part != 0) {
        if ((part == kControlDownButtonPart) ||
            (part == kControlPageDownPart)) {
            delta = -delta;
        }

        CommonActionTermWindow(termPtr->fHorizScroll, &delta);

        TMScroll(termPtr->fTermHandle,
                 (delta) * (termPtr->fTermEnvirons.cellSize.h),
                 0);
    }
} /*HorizScrollTextTermWindow*/

/********************************************************************************
    Procedure:  DoHorizScrollTermWindow
    Purpose:    Does the horiz scroll
    Passed:     termPtr --  A pointer to the common structure
                part --     What part of the scroll the user clicked in
                point --    Exact point where the click occurred
    Returns:    void
********************************************************************************/
void DoHorizScrollTermWindow(TermWindowPtr termPtr, short part, Point point)
{
    short controlValue;

    if (part == kControlIndicatorPart) {
        controlValue = GetControlValue(termPtr->fHorizScroll);

        part = TrackControl(termPtr->fHorizScroll, point, nil);
        if (part != 0) {
            controlValue -= GetControlValue(termPtr->fHorizScroll);
            if (controlValue < 0) {
                TMScroll(termPtr->fTermHandle,
                         (controlValue) * (termPtr->fTermEnvirons.cellSize.h),
                         0);
            }
            if (controlValue > 0) {
                TMScroll(termPtr->fTermHandle,
                         (controlValue) * (termPtr->fTermEnvirons.cellSize.h),
                         0);
            }
        }
    }
    else {
        // BLOWS UP (BUS ERROR) ON THIS LINE:
        if (!TermWinHorUPP)
            TermWinHorUPP = NewControlActionProc(HorizScrollTextTermWindow);
        part = TrackControl(termPtr->fHorizScroll, point, TermWinHorUPP);
        //      part = TrackControl(termPtr->fHorizScroll, point,
        //                      &HorizScrollTextTermWindow);
    }
} /*DoHorizScrollTermWindow*/

/********************************************************************************
    Procedure:  AdjustVertScrollTermWindow
    Purpose:    Adjusts the vert scroll bar
    Passed:     termPtr:
                    A pointer to the common structure
    Returns:    void
********************************************************************************/
void AdjustVertScrollTermWindow(TermWindowPtr termPtr)
{
    short oldScroll, newScroll, maxTop;
    Rect  ourVisRect;

    maxTop = (termPtr->fTermEnvirons.textRows) - TermWindowSizeInLines(termPtr);

    if (maxTop <= 0) {
        maxTop = 0;
        HiliteControl(termPtr->fVertScroll, 255); // inactive
    }
    else {
        if (maxTop > termPtr->fTermEnvirons.textRows) {
            maxTop = termPtr->fTermEnvirons.textRows;
        }

        HiliteControl(termPtr->fVertScroll, 0); // active
    }

    SetControlMaximum(termPtr->fVertScroll, maxTop);

    ourVisRect = (**(termPtr->fTermHandle)).visRect;
    CellRectToPixelRect(termPtr, &ourVisRect);

    oldScroll = ourVisRect.top - (**(termPtr->fTermHandle)).viewRect.top;
    newScroll = (GetControlValue(termPtr->fVertScroll)) *
                (termPtr->fTermEnvirons.cellSize.v);

    // TMScroll(termPtr->fTermHandle, 0, (oldScroll - newScroll));
} /*AdjustVertScrollTermWindow*/

/********************************************************************************
    Procedure:  VertScrollTextTermWindow
    Purpose:    Makes sure that the control movement happens correctly and the
                    text is scrolled.
    Passed:     termPtr:
                    A pointer to the common structure
    Returns:    void
********************************************************************************/
pascal void VertScrollTextTermWindow(ControlHandle theControl, short part)
{
    short         delta, oldValue;
    TermWindowPtr termPtr;

    termPtr = (TermWindowPtr)GetControlReference(theControl);

    oldValue = GetControlValue(termPtr->fVertScroll);

    switch (part) {
    case kControlUpButtonPart:
    case kControlDownButtonPart:
        delta = 1;
        break;

    case kControlPageUpPart:
    case kControlPageDownPart:
        delta = TermWindowSizeInLines(termPtr);
        break;

    default:
        break;
    }

    if (part != 0) {
        if ((part == kControlDownButtonPart) ||
            (part == kControlPageDownPart)) {
            delta = -delta;
        }

        CommonActionTermWindow(termPtr->fVertScroll, &delta);

        TMScroll(termPtr->fTermHandle,
                 0,
                 (delta) * (termPtr->fTermEnvirons.cellSize.v));
    }
} /*VertScrollTextTermWindow*/

/********************************************************************************
    Procedure:  DoVertScrollTermWindow
    Purpose:    Does the vert scroll
    Passed:     termPtr --  A pointer to the common structure
                part --     What part of the scroll the user clicked in
                point --    Exact point where the click occurred
    Returns:    void
********************************************************************************/
void DoVertScrollTermWindow(TermWindowPtr termPtr, short part, Point point)
{
    short controlValue;

    if (part == kControlIndicatorPart) {
        controlValue = GetControlValue(termPtr->fVertScroll);

        part = TrackControl(termPtr->fVertScroll, point, nil);
        if (part != 0) {
            controlValue -= GetControlValue(termPtr->fVertScroll);
            if (controlValue < 0) {
                TMScroll(termPtr->fTermHandle,
                         0,
                         (controlValue) * (termPtr->fTermEnvirons.cellSize.v));
            }
            if (controlValue > 0) {
                TMScroll(termPtr->fTermHandle,
                         0,
                         (controlValue) * (termPtr->fTermEnvirons.cellSize.v));
            }
        }

        AdjustVertScrollTermWindow(termPtr);
    }
    else {
        if (!TermWinVerUPP)
            TermWinVerUPP = NewControlActionProc(VertScrollTextTermWindow);
        part = TrackControl(termPtr->fVertScroll, point, TermWinVerUPP);
    }
} /*DoVertScrollTermWindow*/

/********************************************************************************
    Procedure:  ContentClickTermWindow
    Purpose:    Handles content clicks in the terminal region
    Passed:     termPtr --      A pointer to the common structure
                theEventPtr --  Event record containing event
    Returns:    void
********************************************************************************/
void ContentClickTermWindow(TermWindowPtr      termPtr,
                            const EventRecord *theEventPtr)
{
    short         thePart;
    Point         thePoint;
    ControlHandle theControl;
    Rect          tempRect;

    if ((WindowPtr)termPtr != FrontWindow()) {
        SelectWindow((WindowPtr)termPtr);
    }
    else {
        thePoint = theEventPtr->where;
        GlobalToLocal(&thePoint);

        thePart = FindControl(thePoint, (WindowPtr)termPtr, &theControl);

        // the user clicked in the vertical scroll bar.  We dispatch here
        //   to the set of vertical handlers.
        if (TermWinScrolls && theControl == termPtr->fVertScroll) {
            DoVertScrollTermWindow(termPtr, thePart, thePoint);
        }
        else {
            // maybe the user clicked in the horizontal scroller?
            if (TermWinScrolls && theControl == termPtr->fHorizScroll) {
                DoHorizScrollTermWindow(termPtr, thePart, thePoint);
            }
            else {
                // Must be a content click in the terminal region.  We'll
                //   let the terminal tool handle by calling TMClick.
                if (nil == theControl) {
                    tempRect = (**(termPtr->fTermHandle)).visRect;
                    CellRectToPixelRect(termPtr, &tempRect);
                    if (PtInRect(thePoint, &tempRect)) {
                        TMClick(termPtr->fTermHandle, theEventPtr);
                    }
                }
            }
        }
    }
} /*ContentClickTermWindow*/

/********************************************************************************
    Procedure:  DragTermWindow
    Purpose:    Selects the window if it's not frontmost or calls the toolbox
                    routine DragWindow for the window movement, then updates
                    the window and scroll bars
    Passed:     termPtr --      A pointer to the common structure
                where --        What point of the window was clicked
    Returns:    void
********************************************************************************/
void DragTermWindow(TermWindowPtr termPtr, Point where)
{
    Rect limitRect;

    if ((WindowPtr)termPtr != FrontWindow()) {
        SelectWindow((WindowPtr)termPtr);
    }
    else {
        SetRect(&limitRect,
                0,
                20,
                qd.screenBits.bounds.right,
                qd.screenBits.bounds.bottom);

        InsetRect(&limitRect, 4, 4);

        DragWindow((WindowPtr)termPtr, where, &qd.screenBits.bounds);
    }
} /*DragTermWindow*/

/********************************************************************************
    Procedure:  ResizeTermWindow
    Purpose:    Selects the window if it's not frontmost or handles size
                    changes for the window
    Passed:     termPtr --      A pointer to the common structure
                where --        The point where the user clicked
    Returns:    void
********************************************************************************/
void ResizeTermWindow(TermWindowPtr termPtr, Point where)
{
    long newSize;
    Rect limitRect;

    if ((WindowPtr)termPtr != FrontWindow()) {
        SelectWindow((WindowPtr)termPtr);
    }
    else {
        SetRect(&limitRect,
                termPtr->fMinWindowSize.h,
                termPtr->fMinWindowSize.v,
                (qd.screenBits.bounds.right - qd.screenBits.bounds.left),
                (qd.screenBits.bounds.bottom - qd.screenBits.bounds.top) - 20);

        newSize = GrowWindow((WindowPtr)termPtr, where, &limitRect);

        if (0 != newSize) {
            EraseRect(&((WindowPtr)termPtr)->portRect);

            SizeWindow((WindowPtr)termPtr,
                       LoWrd(newSize),
                       HiWrd(newSize),
                       true);

            limitRect = ((WindowPtr)termPtr)->portRect;
            AdjustTermRect(&limitRect);

            // is this (below) right?!?  Seems like we'd want
            // to pass the actual size, rather than the limit rect...
            TMResize(termPtr->fTermHandle, &limitRect);

            InvalRect(&((WindowPtr)termPtr)->portRect);
        }
    }
} /*ResizeTermWindow*/

/* Perform Zoom */
pascal void
DoZoomTermWindow(TermWindowPtr termPtr, short zoomFactor, int left, int top)
{
    Rect               limitRect;
    Point              StdSize;
    struct WStateData *wsP;

    SetPort(((WindowPtr)termPtr));
    EraseRect(&((WindowPtr)termPtr)->portRect);

    if (zoomFactor == inZoomOut) {
        // first set the standard size
        TerminalFullSize(termPtr, &StdSize);
        wsP = (WStateDataPtr)(*(termPtr->fWindowRec.dataHandle));
        if (left > 0)
            wsP->stdState.left = left; /* (bfo) */
        if (top > 0)
            wsP->stdState.top = top; /* (bfo) */
        wsP->stdState.right  = wsP->stdState.left + StdSize.h + 15;
        wsP->stdState.bottom = wsP->stdState.top + StdSize.v + 15;
    }
    ZoomWindow((WindowPtr)termPtr, zoomFactor, false);

    limitRect = ((WindowPtr)termPtr)->portRect;
    AdjustTermRect(&limitRect);
    TMResize(termPtr->fTermHandle, &limitRect);

    /* don't forget this: Without this you will get the Dirty-Strout */
    /* with scroll bars inside the window !! (bfo) */
    if (TermWinScrolls) {
        FixVertScrollTermWindow(termPtr);
        FixHorizScrollTermWindow(termPtr);
    }

    InvalRect(&((WindowPtr)termPtr)->portRect);
}

/********************************************************************************
    Procedure:  ZoomTermWindow
    Purpose:    Selects the window if it's not frontmost or handles zoom events
                    by calling the toolbox proc ZoomWindow
    Passed:     termPtr --      A pointer to the common structure
                where --        The point where the user clicked so we can
                                    call TrackBox
    Returns:    void
********************************************************************************/
void ZoomTermWindow(TermWindowPtr termPtr, short zoomFactor, Point where)
{
    if ((WindowPtr)termPtr != FrontWindow()) {
        SelectWindow((WindowPtr)termPtr);
    }
    else {
        if (TrackBox((WindowPtr)termPtr, where, zoomFactor)) {
            DoZoomTermWindow(termPtr, zoomFactor, 0, 0);
        }
    }
} /*ZoomTermWindow*/

/********************************************************************************
    Procedure:  MouseDownTermWindow
    Purpose:    Click dispatcher for this window type.
    Passed:     termPtr --      A pointer to the common structure
                theEventPtr --  Pointer to the event record containing the
                                    event information.
    Returns:    void
********************************************************************************/
void MouseDownTermWindow(TermWindowPtr termPtr, const EventRecord *theEventPtr)
{
    short     part;
    long      menuItem;
    WindowPtr whichWindow;

    part = FindWindow(theEventPtr->where, &whichWindow);

    if (whichWindow != nil) {
        switch (part) {
        case inMenuBar:
            menuItem = MenuSelect(theEventPtr->where);
            TMMenu(termPtr->fTermHandle, HiWrd(menuItem), LoWrd(menuItem));
            break;

        case inContent:
            ContentClickTermWindow(termPtr, theEventPtr);
            break;

        case inGoAway:
            if (termPtr->fWindowRec.goAwayFlag)
                break;
        case inDrag:
            DragTermWindow(termPtr, theEventPtr->where);
            if (TermWinScrolls) {
                FixVertScrollTermWindow(termPtr);
                FixHorizScrollTermWindow(termPtr);
            }
            break;

        case inGrow:
            ResizeTermWindow(termPtr, theEventPtr->where);
            FixVertScrollTermWindow(termPtr);
            FixHorizScrollTermWindow(termPtr);
            break;

        case inZoomIn:
        case inZoomOut:
            ZoomTermWindow(termPtr, part, theEventPtr->where);
            if (TermWinScrolls) {
                FixVertScrollTermWindow(termPtr);
                FixHorizScrollTermWindow(termPtr);
            }
            break;
        }

        if (TermWinScrolls) {
            AdjustVertScrollTermWindow(termPtr);
            AdjustHorizScrollTermWindow(termPtr);
        }
    }
} /*MouseDownTermWindow*/

/********************************************************************************
    Procedure:  WriteToTermWindow
    Purpose:    This routine will TMStream data to our emulation region.
    Passed:     termPtr:        Pointer to our storage area (also is a
WindowPtr) theData:        ptr to the data lengthOfData:   ptr to long
indicating how long data is. Returns:    OSErr to indicate an error.
********************************************************************************/
pascal OSErr WriteToTermWindow(TermWindowPtr termPtr,
                               Ptr           theData,
                               Size         *lengthOfData)
{
    OSErr   result = noErr;
    long    bytesWritten;
    GrafPtr savedPort;

    if (nil != termPtr) {
        GetPort(&savedPort);
        SetPort((GrafPtr)termPtr);

        if (nil != (termPtr->fTermHandle)) {
            if ((*lengthOfData) ==
                (bytesWritten = TMStream(termPtr->fTermHandle,
                                         theData,
                                         (*lengthOfData),
                                         cmFlagsEOM))) {
                TMIdle(termPtr->fTermHandle);

                if (TermWinScrolls)
                    AdjustVertScrollTermWindow(termPtr);
            }
            else {
                result = tmGenericError;
                DebugStr("\p unable to write to window!.");
            }

            SetPort(savedPort);
        }
        else {
            result = tmGenericError;
            DebugStr("\p theTerminal is nil!");
        }
    }
    else {
        result = tmGenericError;
        DebugStr("\p WindowPtr is nil!");
    }

    return (result);
} /*WriteToTermWindow*/

/********************************************************************************
    Procedure:  FillTermWindow
    Purpose:    Just a quick way to fill the terminal screen
    Passed:     termPtr --      A pointer to the common structure
    Returns:    void
********************************************************************************/
pascal void FillTermWindow(TermWindowPtr termPtr)
{
    //  OSErr           result              = noErr;
    long i, sLength;
    char cTempString[256];

    for (i = 0L; i < 10; i++) {
        sprintf(cTempString, "Ain't coding a blast?, loop# %ld\n\r", i);
        sLength = strlen(cTempString);
        WriteToTermWindow(termPtr, cTempString, &sLength);
    }
} /*FillTermWindow*/

/* config Terminal interactively */
pascal Boolean ConfigTerminal(TermWindowPtr *termPtr)
{
    Point where;
    short result;
    Ptr   configStream;

    // TMChoose Dialog has to hang off this point (global coordinates)
    SetPt(&where, 20, 40);

    // now do TMChoose et al:
    result = TMChoose(&((*termPtr)->fTermHandle), where, NULL);

    if ((result == chooseOKMajor) || (result == chooseOKMinor)) {
        configStream = TMGetConfig((*termPtr)->fTermHandle);
        if (configStream != NULL) {
            /* now apply it ! */
            TMSetConfig((*termPtr)->fTermHandle, configStream);
            DisposePtr(configStream);
            return true;
        }
    }
    return false;
} /* ConfigTerminal */

/* save settings of terminal tool in a file called "<Prefix> <Toolname>
 * Settings" */
pascal OSErr SaveTerminalSettings(TermWindowPtr *termPtr, char *Prefix)
{
    //  short           result;
    Ptr    configStream;
    Str255 toolName;
    Str255 filename;
    FSSpec theSpec;
    FILE  *theFile;

    configStream = TMGetConfig((*termPtr)->fTermHandle);
    if (configStream != NULL) {
        TMGetToolName((**(*termPtr)->fTermHandle).procID, toolName);
        p2cstr(toolName);
        strcpy(filename, Prefix);
        strcat(filename, " ");
        strcat(filename, toolName);
        strcat(filename, " Settings");
        if (FindFolder(kOnSystemDisk,
                       kPreferencesFolderType,
                       false,
                       &theSpec.vRefNum,
                       &theSpec.parID) == noErr) {
            /* preferences folder found */
            c2pstr(filename);
            BlockMove(filename, theSpec.name, 31);
            /* now write file */
            if ((theFile = FSp_fopen(&theSpec, "w")) != NULL) {
                fputs(configStream, theFile);
                fputc('\n', theFile);
                fclose(theFile);
            }
        }
        /* saved */
        DisposePtr(configStream);
    }
    return noErr; /* no error checking yet %%% */
} /* SaveTerminalSettings */

/* load settings of terminal tool in a file called "<Prefix> <Toolname>
 * Settings" */
pascal OSErr LoadTerminalSettings(TermWindowPtr *termPtr, char *Prefix)
{
    //  short           result;
#define MAXCONFIGLEN 1000 /* should be enough */
    char   configStream[MAXCONFIGLEN];
    Str255 toolName;
    Str255 filename;
    FSSpec theSpec;
    FILE  *theFile;

    TMGetToolName((**(*termPtr)->fTermHandle).procID, toolName);
    p2cstr(toolName);
    strcpy(filename, Prefix);
    strcat(filename, " ");
    strcat(filename, toolName);
    strcat(filename, " Settings");
    if (FindFolder(kOnSystemDisk,
                   kPreferencesFolderType,
                   false,
                   &theSpec.vRefNum,
                   &theSpec.parID) == noErr) {
        /* preferences folder found */
        c2pstr(filename);
        BlockMove(filename, theSpec.name, 31);
        /* now read file */
        if ((theFile = FSp_fopen(&theSpec, "r")) != NULL) {
            if (fgets(configStream, MAXCONFIGLEN, theFile) != NULL) {
                /* now config */
                TMSetConfig((*termPtr)->fTermHandle, configStream);
            }
            else {
                /* reset Terminal to defaults */
                TMReset((*termPtr)->fTermHandle);
            }
            fclose(theFile);
        }
    }
    return noErr; /* no error checking yet %%% */
} /* LoadTerminalSettings */

/********************************************************************************
    Procedure:  NewTermWindow
    Purpose:    This routine is used to determine initial window placement and
                    characteristics.
    Passed:     termPtr --          A pointer to the common structure, normally
                                        defined ahead of time for application
                                        convenience
                boundsRect --       Bounding rectangle for the screen location
                                        of the terminal window.
                title --            Window title
                visible --          Determines initial window visibility
                theProc --          This is the procID which determines window
                                        type.  Since we always attach scroll
                                        bars, it's best that you use a normal
                                        Macintosh document.
                behind --           Use this parameter to determine which
                                        window this window should appear
                                        behind.  Pass -1 for frontmost.
                goAwayFlag --       Display a close box?
                toolName --         Terminal tool name.
    Returns:    An OSErr(or) indicating any failure.
********************************************************************************/
pascal OSErr NewTermWindow(TermWindowPtr   *termPtr,
                           const Rect      *boundsRect,
                           ConstStr255Param title,
                           Boolean          visible,
                           short            theProc,
                           WindowPtr        behind,
                           Boolean          goAwayFlag,
                           Str31            toolName)
{
    OSErr     result = noErr;
    short     procID, realWindowProcID;
    GrafPtr   savedPort;
    WindowPtr frWindow = FrontWindow();
    Rect      termRect, realWindowRect = {0, 0, 0, 0};
    Str255    realWindowTitle;
    Str255    realToolName;

    realWindowProcID = theProc;
    if (IsNOTAWindowProc(theProc)) {
        realWindowProcID = zoomDocProc;
    }

    if (nil == (*termPtr)) {
        if (nil !=
            ((*termPtr) = (TermWindowPtr)NewPtrClear(sizeof(TermWindowRec)))) {
            (*termPtr)->fWindowType = 99;
        }
        else {
            DebugStr("\p Error @NewPtr;dw MemErr;dw ResErr");
            return result;
        }
        DebugStr("\p fWindowRecord was nil so we allocated a ptr here.");
    }

    // if we have a window to put this behind.
    if (!EmptyRect(boundsRect)) {
        realWindowRect = *boundsRect;
    }
    else {
        if (frWindow)
            realWindowRect = (frWindow->portRect);
        else
            realWindowRect = qd.screenBits.bounds;
    }

    // if we have a have a non-empty string use it.
    BlockMove(title, realWindowTitle, title[0] + 1);
    if (title[0] == 0) {
        BlockMove("\pStatus", (Ptr)title, 7);
        // strcpy(realWindowTitle, "\pStatus");
    }

    // arbitrary numbers for the window size.
    //(*termPtr)->fMinWindowSize.h = realWindowRect.right - realWindowRect.left;
    //(*termPtr)->fMinWindowSize.v = realWindowRect.bottom - realWindowRect.top;
    (*termPtr)->fMinWindowSize.v = 150;
    (*termPtr)->fMinWindowSize.h = 150;

    if (nil != (NewWindow((*termPtr),
                          &realWindowRect,
                          realWindowTitle,
                          false,
                          realWindowProcID,
                          behind,
                          goAwayFlag,
                          0L))) {
        GetPort(&savedPort);
        SetPort((GrafPtr)(*termPtr));
        // this doesn't get me a bigger document space...
        // I think I need to use TM routines...?
        // PortSize( realWindowRect.right-realWindowRect.left, 600 );

        if (TermWinScrolls) {
            (*termPtr)->fVertScroll =
                NewControl((WindowPtr)(*termPtr),
                           &((WindowPtr)*termPtr)->portRect,
                           "\p",
                           true,
                           0,
                           0,
                           0,
                           scrollBarProc,
                           (long)(*termPtr));

            (*termPtr)->fHorizScroll =
                NewControl((WindowPtr)(*termPtr),
                           &((WindowPtr)*termPtr)->portRect,
                           "\p",
                           true,
                           0,
                           0,
                           0,
                           scrollBarProc,
                           (long)(*termPtr));
            if (!(*termPtr)->fVertScroll || !(*termPtr)->fHorizScroll) {
                DebugStr("\p Err at NewControl");
                return tmGenericError;
            }

            FixVertScrollTermWindow((*termPtr));
            FixHorizScrollTermWindow((*termPtr));
        }

        BlockMove(toolName, realToolName, toolName[0] + 1);
        if (toolName[0] == 0) {
            BlockMove("\pVT102 Modul", realToolName, 20);
        }

        procID = TMGetProcID(realToolName);
        if (procID == -1) {
            BlockMove("\pVT102 Tool", realToolName, 20);
            procID = TMGetProcID(realToolName);
        }

        if (procID == -1) {
            CRMGetIndToolName(classTM, 1, realToolName);
            procID = TMGetProcID(realToolName); /* assign procid !! */
        }

        if (procID == -1) {
            StopAlert(rVT102Alert, NULL);
            ExitToShell();
        }

        termRect = ((WindowPtr)*termPtr)->portRect;
        AdjustTermRect(&termRect);

        (*termPtr)->fTermHandle = TMNew(&termRect,
                                        &termRect,
                                        0L,
                                        procID,
                                        (WindowPtr)(*termPtr),
                                        nil,
                                        nil,
                                        nil,
                                        nil,
                                        nil,
                                        0L,
                                        0L);

        (*termPtr)->fTermEnvirons.version = curTermEnvRecVers;
        result = TMGetTermEnvirons((*termPtr)->fTermHandle,
                                   &((*termPtr)->fTermEnvirons));

        if (result != noErr) {
            DebugStr("\p Err at TMGetTermEnvirons");
            return tmGenericError;
        }

        if (visible)
            ShowWindow((WindowPtr)*termPtr);

        SetPort(savedPort);
    }
    else {
        DebugStr("\p Err at NewWindow");
        result = tmGenericError;
    }

    return result;
} /*NewTermWindow*/

/********************************************************************************
    Procedure:  IsTermWindowEvent
    Purpose:    This routine peeks at the passed event record fields to
determine if the event should be destined for this window.  I use Boolean IFs
when peeking at the event record to save a few instructions instead of the
switch C token. Passed:     termPtr --          A pointer to the common
structure theEventPtr --      Event record containing the current event Returns:
True if the event is for our window.
********************************************************************************/
pascal Boolean IsTermWindowEvent(TermWindowPtr      termPtr,
                                 const EventRecord *theEventPtr)
{
    Boolean   result = false;
    short     part;
    WindowPtr whichWindow;

    if (nil != termPtr) {
        if (nil != (termPtr->fTermHandle)) {
            TMIdle(termPtr->fTermHandle);
        }

        if (mouseDown == (theEventPtr->what)) {
            part = FindWindow(theEventPtr->where, &whichWindow);
            if (whichWindow == (WindowPtr)termPtr) {
                result = true;
            }
        }
        else {
            if ((updateEvt == (theEventPtr->what)) ||
                (activateEvt == (theEventPtr->what))) {
                if (theEventPtr->message == (long)termPtr) {
                    result = true;
                }
            }
        }
    }

    return (result);
} /*IsTermWindowEvent*/

/********************************************************************************
    Procedure:  HandleTermWindowEvent
    Purpose:    This is a basic dispatcher of the event.  Since in a normal
                    implementation of this function it only gets called if the
                    routine is actually needed, many checks to verify the
                    window are avoided.
    Passed:     termPtr --          A pointer to the common structure
                theEventPtr --      Event record containing the current event
    Returns:    void
********************************************************************************/
pascal void HandleTermWindowEvent(TermWindowPtr      termPtr,
                                  const EventRecord *theEventPtr)
{
    Boolean activate;
    GrafPtr savedPort;

    if (nil != termPtr) {
        GetPort(&savedPort);
        SetPort((GrafPtr)termPtr);

        if (nil != (termPtr->fTermHandle)) {
            switch (theEventPtr->what) {
            case mouseDown:
                MouseDownTermWindow(termPtr, theEventPtr);
                break;

            case activateEvt:
                activate = (0 != (theEventPtr->modifiers & activeFlag));
                TMActivate(termPtr->fTermHandle, activate);
                if (activate) {
                    DrawGrowIcon((WindowPtr)termPtr);
                    DrawControls((WindowPtr)termPtr);
                    if (TermWinScrolls) {
                        AdjustVertScrollTermWindow(termPtr);
                        AdjustHorizScrollTermWindow(termPtr);
                    }
                }
                break;

            case updateEvt:
                BeginUpdate((WindowPtr)termPtr);
                if (nil != ((WindowPtr)termPtr)->visRgn) {
                    TMUpdate(termPtr->fTermHandle,
                             ((WindowPtr)termPtr)->visRgn);

                    DrawGrowIcon((WindowPtr)termPtr);
                    DrawControls((WindowPtr)termPtr);
                }
                EndUpdate((WindowPtr)termPtr);
                break;

            default:
                break;
            }
        }
        else {
            DebugStr("\p theTerminal is nil! at HandleTermWindowEvent");
        }

        SetPort(savedPort);
    }
    else {
        DebugStr("\p termPtr is nil! at HandleTermWindowEvent");
    }
} /*HandleTermWindowEvent*/

/********************************************************************************
    Procedure:  DisposeTermWindow
    Purpose:    This routine disposes of all private storage for our window. You
                    can avoid this step if you simply _ExitToShell
    Passed:     termPtr --          A pointer to the common structure
    Returns:    void
********************************************************************************/
pascal void DisposeTermWindow(TermWindowPtr termPtr)
{
    if (!termPtr) {
        DebugStr("\ptermPtr is nil");
        return;
    }
    if (TermWinScrolls) {
        if (termPtr->fVertScroll) {
            DisposeControl(termPtr->fVertScroll);
            termPtr->fVertScroll = nil;
        }
        if (termPtr->fHorizScroll) {
            DisposeControl(termPtr->fHorizScroll);
            termPtr->fHorizScroll = nil;
        }
    }
    if (termPtr->fTermHandle) {
        TMDispose(termPtr->fTermHandle);
        termPtr->fTermHandle = nil;
    }
    DisposeWindow((WindowPtr)termPtr);
} /*DisposeTermWindow*/

pascal OSErr PStringToTermWindow(TermWindowPtr termPtr, unsigned char *pstr)
{
    Size numChars = pstr[0];
    WriteToTermWindow(termPtr, (Ptr)(&pstr[1]), &numChars);
    return 0;
}
