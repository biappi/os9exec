/*      termconsole.c
**
**      A console based on a CTB terminal window.
**      by Joseph J. Strout (jstrout@ucsd.edu)
**      based on console.stubs.c by Metrowerks
**
**      Last revision: 7/23/96
**      Adapted by LuZ/Bfo for the OS9exec project
*/

#ifndef __CONSOLE__
#include <console.h>
#endif

#include "TermWindow.h"
#include <Events.h>
#include <Scrap.h>
#include <ToolUtils.h>
#include "os9exec_incl.h"
#include "serialaccess.h"

#define STARTVAL -200




/* definitions for Multi-Strout support (bfo)   */
/* there are currently 50 terminal consoles and */
/* 50 (!) drawing windows available */
ttydev_typ mac_console[ MAX_CONSOLE ];      


static void InitConsoles()
/* there are many different consoles supported now */
/* initialize them all */
{
    ttydev_typ* mco;
    int         k, h;

    #ifndef USE_CARBON
      InitSerials();
    #endif
    
    for (k=0; k<MAX_CONSOLE; k++) {
        mco= &mac_console[k];
        mco->installed      = false;
        mco->consoleTermPtr =   nil;
        mco->inBufUsed      =     0;
        mco->holdScreen     = false;
        mco->spP            =  NULL;
        mco->pict_tot       =     0;
        
        for (h=0; h<MAX_PICTS; h++) 
            mco->pict_hdl[h]=  NULL;
    }
} /* InitConsoles */



// module global variables...
int                 toolBoxDone       = false;
int                 gInBackground     = false;
int                 gConsoleQuickInput= false;
short               gConsoleEcho      = true;   /* as it was standard in the original */
int                 gConsoleSleep     = 1;      /* default was 5 */

short               gConsoleNLExpand  = true;
char                gTitle[OS9NAMELEN];         /* title for   /vmod output */
Rect*               gRect             = NULL;   /* window size /vmod output */

int                 gLastwritten_pid  = 0;      /* last written character's process */
int                 gConsoleID        = 0;      /* global console ID */
syspath_typ*        g_spP             = NULL;
ulong               gNetActive        = 0;      /* speed-up for bg tasks if net activity */
ulong               gNetLast          = 0;      /* speed-up for bg tasks if net activity */
Boolean             gDocDone          = false;   
FSSpec              gFS;                        /* FSSpec, if called via file */

#include "Resource Constants.h"

#define kDITop                  0x0050
#define kDILeft                 0x0070
#define kOSEvent                app4Evt /* event used by MultiFinder */
#define kSuspendResumeMessage   1       /* high byte of suspend/resume event message */
#define kResumeMask             1       /* bit of message field for resume vs. suspend */
#define kMouseMovedMessage      0xFA    /* high byte of mouse-moved event message */
#define kNoEvents               0       /* no events mask */
#define keyRETURN '\r'                  /* ASCII code given when Return is pressed */



// forward declarations of functions within this module

static void DoSetupToolbox(void);
void AdjustMenus();
void DoUpdate(WindowPtr window);
int  CanCopy();
int  DoCopy();
int  CanPaste();
int  DoPaste();
int  DoEditConfig();    
void DoMenuCommand(long menuResult);
void HandleKey( char key );
void DoEvent       ( EventRecord*  event );
void HandleOneEvent( EventRecord* pEvent );
void EventLoop();

short      InstallConsole( short fd );
void        RemoveConsole( void );
long  WriteCharsToConsole( char *buffer, long n );
long ReadCharsFromConsole( char *buffer, long n );
Boolean          DevReady( long *count );
ulong       SysPathWindow( syspath_typ* spP );
ulong       CurrentWindow( void );
void           UpdatePrms( ulong  wPtr,   ulong  wStore, ulong wSize, 
                           ushort windex, ushort wTot );
void    CheckInputBuffers( void );
Boolean       KeyToBuffer( ttydev_typ* mco, char key );



//  Set up the tool box, if not done already...
static void DoSetupToolbox(void)
{
    #ifndef USE_CARBON
    Handle menuBar;
    
    if (toolBoxDone) return;
    
    //  Initialize the ToolBox ...
    InitGraf(&qd.thePort);
    InitFonts();
    FlushEvents(everyEvent, 0L);
    InitWindows();
    InitMenus();
    TEInit();
    InitDialogs(0L);
    InitCursor();
    
    MaxApplZone();
    MoreMasters();

    menuBar = GetNewMBar(rMenuBar);         /* read menus into menu bar */
    if ( menuBar == nil ) ExitToShell();    // should do something better here!
    SetMenuBar(menuBar);                    /* install menus */
    DisposeHandle(menuBar);
    AppendResMenu(GetMenuHandle(mApple), 'DRVR');   /* add DA names to Apple menu */
    DrawMenuBar();
    #endif

    toolBoxDone = true;
}

/*  Enable and disable menus based on the current state.
    The user can only select enabled menu items. We set up all the menu items
    before calling MenuSelect or MenuKey, since these are the only times that
    a menu item can be selected. Note that MenuSelect is also the only time
    the user will see menu items. This approach to deciding what enable/
    disable state a menu item has the advantage of concentrating all
    the decision-making in one routine, as opposed to being spread throughout
    the application. Other application designs may take a different approach
    that is just as valid. */

void AdjustMenus()
{
    WindowPtr   window;
    MenuHandle  menu;

    window = FrontWindow();

    menu = GetMenuHandle(mFile);
    DisableItem(menu, iClose);  /* can't close the console window? */

    menu = GetMenuHandle(mEdit);    // use only certain Edit items... 
    DisableItem(menu, iUndo);
    DisableItem(menu, iCut);
    if (CanCopy()) EnableItem (menu, iCopy);
    else DisableItem(menu, iCopy);
    DisableItem(menu, iClear);
    if (CanPaste()) EnableItem (menu, iPaste);
    else DisableItem(menu, iPaste);
    EnableItem(menu,iEditConfig);
}

/*  This is called when an update event is received for a window.
    It calls DrawWindow to draw the contents of an application window.
    As an effeciency measure that does not have to be followed, it
    calls the drawing routine only if the visRgn is non-empty. This
    will handle situations where calculations for drawing or drawing
    itself is very time-consuming. */


#pragma segment Main
static int FrontConsole( ttydev_typ **mcp )
/* returns true, if current console is in front */
{
    TermWindowPtr fw;
    int k;
    ttydev_typ *mco;

    /* search for the front console */  
    fw= FrontWindow();
    for (k=0; k<MAX_CONSOLE; k++) {
            mco= &mac_console[k];
        if (mco->installed && (TermWindowPtr)mco->consoleTermPtr==fw) {
           *mcp= mco;
            return 1; /* it is the front window */
        }
    }
    
    return 0;
} /* FrontConsole */


void DoUpdate(WindowPtr window)
{
    if ( true ) {       //   <-- put a check here for if it's right kinda window
        BeginUpdate    (window);            /* this sets up the visRgn */
        if ( ! EmptyRgn(window->visRgn) ) { /* draw if updating needs to be done */
//           DrawWindow(window);
        }    
        EndUpdate      (window);
    }
} /*DoUpdate*/



/*  Can we copy -- i.e., is there an active selection?  */
int CanCopy()
{
    long rc;
    ResType textType='TEXT';
    ttydev_typ*   mco;
    TermWindowPtr tw;
    TermHandle    term;
    
    if (gConsoleID<0 || 
        gConsoleID>=TTY_Base) return 0;
    
                         mco= &mac_console[gConsoleID];
    tw  = (TermWindowPtr)mco->consoleTermPtr;
    term= tw->fTermHandle;

    // Is this valid?!?  Passing a NULL handle to check on selection size?!?
    rc = TMGetSelect( term, 0, &textType );     // get the data
    return rc>0 ? rc:0;
}



/*  Edit configuration of the Terminal tool */
int DoEditConfig()
{
    #ifndef USE_CARBON
      ttydev_typ*   mco;
      TermWindowPtr tw;
    #endif
    
    if (gConsoleID<0 || 
        gConsoleID>=TTY_Base) return 0; /* only valid for mac consoles */

    #ifndef USE_CARBON
      if      (FrontConsole(&mco)) {
          tw= (TermWindowPtr)mco->consoleTermPtr;

          if (ConfigTerminal      (&tw)) {
              SaveTerminalSettings(&tw,"OS9App" );
              DoZoomTermWindow    ( tw,inZoomOut, 0,0);
          }
      }
    #endif
    
    return 0;
}



/*  Copy selection to the scrap (i.e. clipboard) */
int DoCopy()
{
    Handle  myHandle;
    long rc;
    ResType textType='TEXT';
    ttydev_typ*   mco;
    TermWindowPtr tw;
    TermHandle    term;

    if (gConsoleID<0 || 
        gConsoleID>=TTY_Base) return 0;
    
                         mco= &mac_console[gConsoleID];
    tw  = (TermWindowPtr)mco->consoleTermPtr;
    term= tw->fTermHandle;

    myHandle = NewHandle( 0 );      /* allocate 0-length data area */

    rc = TMGetSelect( term, myHandle, &textType ); // get the data
    
    if (rc>0) {
        SetHandleSize( myHandle, rc+1 );
        HLock( myHandle );
        ZeroScrap();
        PutScrap( rc, textType, *myHandle );
        HUnlock( myHandle );
    }
    
    DisposeHandle(myHandle);
    return rc>0 ? rc:0;
}



/*  Return how many text characters are on the clipboard. */
int CanPaste() {
    long    scrapOffset, rc;

    rc = GetScrap( 0, 'TEXT', &scrapOffset );
    return rc>0 ? rc:0;
}

/*  Paste from the clipboard into the terminal window.  */
int DoPaste() {
    Handle  myHandle;
    char*   myData;
    long    scrapOffset, rc;
    int     i;

    myHandle = NewHandle( 0 );      /* allocate 0-length data area */

    rc = GetScrap( myHandle, 'TEXT', &scrapOffset );
    if ( rc < 1 ) return 0;
    else {
        SetHandleSize( myHandle, rc+1 );
        HLock( myHandle );
        myData = *myHandle;
        for (i=0; i<rc; i++) HandleKey( myData[i] );
        HUnlock( myHandle );
    }
    DisposeHandle(myHandle);
    return rc;
}


/*  This is called when an item is chosen from the menu bar (after calling
    MenuSelect or MenuKey). It performs the right operation for each command.
    It is good to have both the result of MenuSelect and MenuKey go to
    one routine like this to keep everything organized. */

#pragma segment Main
void DoMenuCommand(long menuResult)
{
    short       menuID;             /* the resource ID of the selected menu */
    short       menuItem;           /* the item number of the selected menu */
    short       itemHit;
    Str255      daName;
    short       daRefNum;
    Boolean     handledByDA;

    menuID = HiWord(menuResult);    /* use macros for efficiency to... */
    menuItem = LoWord(menuResult);  /* get menu item number and menu number */
    switch ( menuID ) {
        case mApple:
            switch ( menuItem ) {
                case iAbout:        /* bring up alert for About */
                    itemHit = Alert(rAboutAlert, nil);
                    break;
                default:            /* all non-About items in this menu are DAs */
                    /* type Str255 is an array in MPW 3 */
                    GetMenuItemText(GetMenuHandle(mApple), menuItem, daName);
                    daRefNum = OpenDeskAcc(daName);
                    break;
            }
            break;
        case mFile:
            switch ( menuItem ) {
                case iClose:
                    //DoCloseWindow(FrontWindow());
                    break;
                case iQuit:
                    ExitToShell();
                    break;
            }
            break;
        case mEdit:                 /* call SystemEdit for DA editing & MultiFinder */
            handledByDA = SystemEdit(menuItem-1);
            if (!handledByDA) {
                switch ( menuItem ) {
                    case iCopy:     DoCopy();           break;
                    case iPaste:    DoPaste();          break;
                    case iEditConfig: DoEditConfig();   break;
                }
            }
            break;
    
    }
    HiliteMenu(0);                  /* unhighlight what MenuSelect (or MenuKey) hilited */
} /*DoMenuCommand*/



/* respond to a key by sending it to th terminal window,
   and placing it in the input buffer */
void HandleKey( char key )
{
    Size          n = 1;
    int           ii= 0;
    ttydev_typ*   mco;
    TermWindowPtr tw;
    
    /* write the character to the front window's input buffer */
    if ( FrontConsole(&mco) && KeyToBuffer(mco,key) ) {
            // echo the key to the screen
        tw= (TermWindowPtr)mco->consoleTermPtr;

        if (gConsoleEcho) { 
            switch (key) {
                case keyRETURN: n=2; WriteToTermWindow( tw, "\n\r", &n ); break;
                default       :      WriteToTermWindow( tw,  &key,  &n );
            }
        }
    } /* if */
} /* HandleKey */



/* ----- this part is nor yet really working ----- */

static OSErr DoOpenDocument( AppleEvent *ae, AppleEvent *reply, long refcon )
{
    #pragma unused(reply,refcon)
    
    OSErr       anErr;
    AEDescList  docList; /* list of docs passed in */
    long        index, itemsInList;
//	char        fName[32];
    
    AEKeyword   keywd;
    DescType    returnedType;
    Size        actualSize;

    anErr = AEGetParamDesc( ae, keyDirectObject, typeAEList, &docList);
    anErr = AECountItems( &docList, &itemsInList); /* how many files passed in ? */
    
    /* get first file's info */
    if (itemsInList>0) {
        index= 1;
        anErr= AEGetNthPtr( &docList, index, typeFSS, &keywd, &returnedType,
                            (Ptr)(&gFS), sizeof(gFS), &actualSize);
    }
    
//  memcpy( fName, gFS.name, 32 );
//	p2cstr( fName );
//  upe_printf( "%d %d '%s'\n", gFS.vRefNum, gFS.parID, fName );
    gDocDone= true;
    return 0;
} /* DoOpenDocument */



static void HandleHighLevelEvent( EventRecord* event )
{
    OSErr err= AEProcessAppleEvent( event );
    if   (err==errAEEventNotHandled) gDocDone= true;
} /* HandleHighLevelEvent */



/* this is the really tricky thing for the callback sequence */
/* taken from "SimpleText" sample */
#if GENERATINGCFM
static RoutineDescriptor gODocRD = BUILD_ROUTINE_DESCRIPTOR(uppAEEventHandlerProcInfo, DoOpenDocument);
static AEEventHandlerUPP gODoc   = &gODocRD;
#else
static AEEventHandlerUPP gODoc   = (AEEventHandlerUPP) DoOpenDocument;
#endif


void Install_AppleEvents()
{   AEInstallEventHandler( kCoreEventClass,kAEOpenDocuments, gODoc, 0, false );
}


/* Do the right thing for an event. Determine what kind of event it is, and call
 the appropriate routines. */
#pragma segment Main
void DoEvent(EventRecord* event)
{
    short       part, err;
    WindowPtr   window;
    Boolean     hit;
    char        key;
    Point       aPoint;

    switch ( event->what ) {
        case mouseDown:
            part = FindWindow(event->where, &window);
            switch ( part ) {
                case inMenuBar:          /* process a mouse menu command (if any) */
                    AdjustMenus();
                    DoMenuCommand(MenuSelect(event->where));
                    break;
                case inSysWindow:          /* let the system handle the mouseDown */
                    SystemClick(event, window);
                    break;
                case inContent:
                    if ( window != FrontWindow() ) {
                        SelectWindow(window);
                        /*DoEvent(event);*/ /* use this line for "do first click" */
                    } else
                        //DoContentClick(window);
                    break;
                case inDrag:        /* pass screenBits.bounds to get all gDevices */
                    DragWindow(window, event->where, &qd.screenBits.bounds);
                    break;
                case inGrow:
                    break;
                case inZoomIn:
                case inZoomOut:
                    hit = TrackBox(window, event->where, part);
                    if ( hit ) {
                        SetPort   (window);             /* the window must be the current port... */
                        EraseRect(&window->portRect);   /* because of a bug in ZoomWindow */
                        ZoomWindow(window, part, true); /* note that we invalidate and erase... */
                        InvalRect(&window->portRect);   /* to make things look better on-screen */
                    }
                    break;
            }
            break;
        case keyDown:
        case autoKey:                       /* check for menukey equivalents */
            key = event->message & charCodeMask;
            if ( event->modifiers & cmdKey ) {          /* Command key down */
                if ( event->what == keyDown ) {
                    AdjustMenus();                      /* enable/disable/check menu items properly */
                    DoMenuCommand(MenuKey(key));
                    if (key=='.') HandleKey( CtrlE );   /* handle cmd-. */
                }
            } else HandleKey( key );    /* regular (non-Command) keypress */

            break;
        case activateEvt:
            //DoActivate((WindowPtr) event->message, (event->modifiers & activeFlag) != 0);
            break;
        case updateEvt:
            DoUpdate((WindowPtr) event->message);
            break;
        /*  1.01 - It is not a bad idea to at least call DIBadMount in response
            to a diskEvt, so that the user can format a floppy. */
        case diskEvt:
            if ( HiWord(event->message) != noErr ) {
                SetPt(&aPoint, kDILeft, kDITop);
                err = DIBadMount(aPoint, event->message);
            }
            break;
        case kOSEvent:
        /*  1.02 - must BitAND with 0x0FF to get only low byte */
            switch ((event->message >> 24) & 0x0FF) {       /* high byte of message */
                case kSuspendResumeMessage:     /* suspend/resume is also an activate/deactivate */
                    gInBackground = (event->message & kResumeMask) == 0;
                    //DoActivate(FrontWindow(), !gInBackground);
                    break;
            }
            break;
        
        case kHighLevelEvent:
            HandleHighLevelEvent( event );
            break;
        
    /*  default: upe_printf( "unknown %d\n", event->what ); */
    }
} /*DoEvent*/



static void HandleVModUpdate( TermWindowPtr tw, ttydev_typ* mco )
{
    GrafPtr   my_port;
    Rect      r;
    int       h;
    PicHandle hdl;
    int       top, blk;
    
    GetPort(&my_port ); /* save current port */
    SetPort( tw      );

    if (nil != ((WindowPtr)tw)->visRgn) {
        TMUpdate( tw->fTermHandle, ((WindowPtr)tw)->visRgn );
        
        for (h=0;  h<mco->pict_tot; h++) {
                hdl= mco->pict_hdl[ h ];
            if (hdl!=NULL)
                memcpy( &r,&tw->fWindowRec.port.portRect, sizeof(Rect) );
                r.right -= 20; /* window has scroll bars */
                r.bottom-= 20;

                top     =  r.top;
                blk     = (r.bottom-top)/mco->pict_tot;
                r.top   = top + blk* h;
                r.bottom= top + blk*(h+1);
                
                DrawPicture( hdl, &r ); /* draw PCT */
        }
    }
                    
    SetPort( my_port ); /* and restore previous window */
} /* HandleVModUpdate */



/* Define an event loop, so the user doesn't have to
*/
void HandleOneEvent(EventRecord* pEvent)
{
//  RgnHandle      cursorRgn;
    Boolean        gotEvent;
    EventRecord    event, *eventPtr;
    ttydev_typ*    mco;
    TermWindowPtr  tw;
    WindowPtr      window= NULL;
    int            k;
    short          part = 0;
    Boolean        front= false;
    ulong          ctick= GetSystemTick();
    static int     hov  = STARTVAL;
    static Boolean first= true;

    if     (gNetActive>ctick) {
        if (gNetLast>0 &&
            gNetLast+TICKS_PER_SEC>ctick &&
            hov<0) {
            hov++; return; /* every nth only */
        }
        
        hov     = STARTVAL; 
        gNetLast= ctick;
    }

    // if we're not given an event, then wait for one
    if (!pEvent) {
        // LATER: check for WaitNextEvent trap; if unavailable, do something else!
        if (first)
            first= false; /* can be used as brkpnt place for ugly starter */
        gotEvent = WaitNextEvent(everyEvent, &event, gConsoleSleep, 0);
        eventPtr = &event;
    } else {
        gotEvent = 1;
        eventPtr = pEvent;
    }

    //
    //  These are the dispatchers for the TermWindow stuff.
    //
    if (gDocDone) { /* if already initialized */
        for (k=0; k<MAX_CONSOLE; k++) {
                               mco= &mac_console[k];
            tw= (TermWindowPtr)mco->consoleTermPtr;
            
            #ifndef USE_CARBON
            if (mco->installed &&
                IsTermWindowEvent    (tw, eventPtr)) {
                part = FindWindow( eventPtr->where, &window );
                front= (&window!=FrontWindow());
                HandleTermWindowEvent(tw, eventPtr);
                
                if (k>=VModBase && &window!=nil) {
                    switch (eventPtr->what) {
                        case mouseDown:
                            if (!front) break; /* already done */
                            
                            if (part==inGoAway &&
                                TrackGoAway( window, eventPtr->where )) {
                                    strcpy( gTitle, mco->pict_title );
                                    gConsoleID= k;
                                    RemoveConsole();
                                    break;
                            }
                                
                        case updateEvt:
                                HandleVModUpdate( tw, mco );
                                break;
                    }
                } /* if */

                gotEvent = false; break;
            }
            #endif
        } /* for */
    } /* if */
        
    #ifndef USE_CARBON
      if (gotEvent) DoEvent( &event );
    #endif
} /* HandleOneEvent */


void HandleEvent( void )
{    HandleOneEvent(nil);
} /* HandleEvent */


static void HandleNthEvent()
{
    static int hvv= STARTVAL;
    
    if (hvv<0) hvv++;
    else { 
        hvv= STARTVAL; 
        HandleEvent(); 
        arbitrate= true;
    }
} /* HandleNthEvent */


void EventLoop()
{   while ( true ) HandleEvent();
} /*EventLoop*/



static void GetVModDyn_ID()
{
    ttydev_typ* mco;
    
    int  k;
    for (k=VModBase; k<MAX_CONSOLE; k++) {
            mco= &mac_console[ k ];
        if (mco->installed) {
            if (ustrcmp( gTitle,mco->pict_title )==0) {
                gConsoleID= k; return; /* adapt it to new requirements */
            }
        }
    }

    /* not found, take the first unused one */
    for (k=VModBase; k<MAX_CONSOLE; k++) {
             mco= &mac_console[ k ];
        if (!mco->installed) {
                strcpy( mco->pict_title,gTitle );
                gConsoleID= k; return;
        }
    }
} /* GetVModDin_ID */



/*
 *  The following four functions provide the UI for the console package.
 *  Users wishing to replace SIOUX with their own console package need
 *  only provide the four functions below in a library.
 */

/*
 *  extern short InstallConsole(short fd);
 *
 *  Installs the Console package, this function will be called right
 *  before any read or write to one of the standard streams.
 *
 *  short fd:       The stream which we are reading/writing to/from.
 *  returns short:  0 no error occurred, anything else error.
 */
#ifndef USE_CARBON
short InstallConsole(short fd)
{
    #pragma unused(fd)
    
    TermWindowPtr tw;
    static int    main_installed= false;
    Rect          windowRect;
    Rect*         w;
    ttydev_typ*   mco;
    Str255        title;
    OSErr         result;
    Boolean       goAway, vmod;
    int           woffs;
    
    if (!main_installed) {                   
         main_installed= true; /* don't do this again  */
         InitConsoles();       /* prepare the consoles */
         InitTTYs    ();       /* prepare the TTYs     */
    }

    if (gConsoleID<0) {
        #ifndef USE_CARBON
          InstallSerial( -gConsoleID ); /* Mac serial */
        #endif
         
        return 0;
    }
    
    if (gConsoleID>=VModBase) GetVModDyn_ID();
    
        mco= &mac_console[gConsoleID];
    if (mco->installed) return 0;
        mco->spP= g_spP;
    
    DoSetupToolbox();
    
    #ifdef USE_CARBON
      result= noErr;
    #else    
      result= InitTermMgr();
    #endif
    
    if (result==noErr) {
        vmod= (gConsoleID>=VModBase);
        WindowTitle( title,vmod );
        goAway= vmod;
        
        if (vmod) {
            woffs= 20*(gConsoleID-VModBase);
            w= gRect; /* display terminals on different locations */
            w->left  += woffs;  
            w->top   += woffs;  
            w->right += woffs;
            w->bottom+= woffs;
        }
        else {
            w= &windowRect;
            w->left  =  9 + 20*gConsoleID;  /* display terminals on */
            w->top   = 45 + 20*gConsoleID;  /* different locations */
            w->right = w->left + 80*6 + 20;
            w->bottom= w->top  + 24*12;
        }
        
        c2pstr(title); /* make pascal notation */
        
            mco->consoleTermPtr= (ulong)NewPtrClear( sizeof(TermWindowRec) );
        if (mco->consoleTermPtr!=nil) {
            result= noErr;
            
            #ifndef USE_CARBON
              result= NewTermWindow(&mco->consoleTermPtr,
                                    w, 
                                    title,
                                    true,
                                    zoomDocProc, 
                                    nil,
                                    goAway, 
                                    "" );
            
              if (result==noErr) {
                  //we initialized!
                  /* now try to read Preferences for Terminal Window */
                  tw= (TermWindowPtr)mco->consoleTermPtr;

                  if (gConsoleID<VModBase) {
                      LoadTerminalSettings( &tw,"OS9App" ); /* parametrize! */
                      DoZoomTermWindow(tw,inZoomOut, w->left,
                                                     w->top);
                  }
              } 
              else {
                  DebugStr("\p Error @NewTermWindow");
                  //AlertUser();                      // error at NewTermWindow
                  return result;
              }
            #endif
        } else {
            DebugStr("\p Error @NewPtr;dw ResErr;dw MemErr");
            //AlertUser();                      // error at NewPtr
            return result;
        }
    } else {
        //AlertUser();                      // error at InitTermMgr
        return result;
    }
    
        tw= (TermWindowPtr)mco->consoleTermPtr;
    if (tw!=FrontWindow()) SelectWindow(tw);
    
    mco->installed = true;      // don't do this again!
    return 0;
} /* InstallConsole */
#endif




/*
 *  extern void RemoveConsole(void);
 *
 *  Removes the console package.  It is called after all other streams
 *  are closed and exit functions (installed by either atexit or _atexit)
 *  have been called.  Since there is no way to recover from an error,
 *  this function doesn't need to return any.
 */
#ifndef USE_CARBON
void RemoveConsole(void)
{
    // so that the user gets one last chance to read the screen,
    // we should go into an event loop here before destroying the window
    ttydev_typ*   mco;
    TermWindowPtr tw;
    int           h;
    PicHandle*    hdl;
    
    HandleEvent();
                                 /* if no longer -> loop forever */
    if (gConsoleID==NO_CONSOLE) { 
        gConsoleID= 0; g_spP= NULL; 
        if (quitFlag) ExitToShell();
        EventLoop();
    }
    
    if (gConsoleID==0) return;  /* do not close the main console */

    if (gConsoleID<0) {                      /* close Mac Serial */
        #ifndef USE_CARBON
          RemoveSerial( -gConsoleID );
        #endif
        
        return;  
    }
          
    if (gConsoleID>=VModBase) GetVModDyn_ID();
    
                       mco= &mac_console[gConsoleID];
    tw= (TermWindowPtr)mco->consoleTermPtr;
        
    if (mco->installed && tw) {
        mco->installed= false;
        
        for (h=0;  h<MAX_PICTS; h++) {
                 hdl= &mco->pict_hdl[ h ];
            if (*hdl!=NULL) {
                DisposeHandle( *hdl );
                *hdl= NULL;
            }
        }
        
        #ifndef USE_CARBON
          DisposeTermWindow( tw );
        #endif
    }
} /* RemoveConsole */
#endif




static OSErr WriteToTermW(ttydev_typ* mco, Ptr theData, Size *lengthOfData)
{
    #ifdef USE_CARBON
      return 0;
    #else
      while (mco->holdScreen) CheckInputBuffers(); /* XOn/XOff flow control */
      return WriteToTermWindow( (TermWindowPtr)mco->consoleTermPtr, theData,lengthOfData );
    #endif
} /* WriteToTermW */



/*
 *  extern long WriteCharsToConsole(char *buffer, long n);
 *
 *  Writes a stream of output to the Console window.  This function is
 *  called by write.
 *
 *  char *buffer:   Pointer to the buffer to be written.
 *  long n:         The length of the buffer to be written.
 *  returns short:  Actual number of characters written to the stream,
 *                  -1 if an error occurred.
 */

#ifndef USE_CARBON
long WriteCharsToConsole(char *buffer, long n)
{
    // to make it act like a normal window, we need to convert
    // certain characters... in particular '\r' -> '\r\n'
    Size numchars=1;
    char *c=buffer, *start=buffer;
    char linend[]   = "\x0D\x0A";
    Size linendsize = 2;
    ttydev_typ* mco;
        
    /* modified by Bfo for Serial Interface */
    if (gConsoleID<0) {
        #ifndef USE_CARBON
          return WriteCharsToSerial( buffer, n, -gConsoleID );
        #endif
        return 0;
    }
    
    if (gConsoleID>=TTY_Base) /* write to tty connection */
        return WriteCharsToPTY   ( buffer, n,  gConsoleID, false ); 

            mco= &mac_console[gConsoleID];
    if    (!mco->consoleTermPtr) return -1; /* not a valid terminal */
    lw_pid( mco ); /* assign for later use ! */

    /* modified by LuZ */
    if (gConsoleNLExpand) {
        /* expand CRs and LFs to CR/LF */
        do {
            // loop till CR or LF or end of string
            while (c-buffer < n && *c!=CR && *c!=LF) c++;
            numchars = c-start;
            if (WriteToTermW( mco, (Ptr)start,&numchars ) != noErr) return -1;
            // if next char is CR, output a CRLF instead
            if (c-buffer < n && (*c==CR || *c==LF)) {
                WriteToTermW( mco, (Ptr)linend, &linendsize );
                start = c++;
            }
        } while (c-buffer < n);
    }
    else {  
        /* without any extra CR/LF expansion stuff */
        numchars=n;
        if (WriteToTermW( mco, (Ptr)start,&numchars ) != noErr) return -1;
    }
    return n;
} /* WriteCharsToConsole */
#endif



Boolean DevReady( long *count )
/* true, if next character(s) can be read */
/* used by "ReadCharsFromConsole" and "GetStat.SS_Ready" */
{
    Boolean     ok;
    ttydev_typ* mco;
    
    HandleNthEvent();
    if                                   (gConsoleID<0) 
             ok= DevReadySerial  (count, -gConsoleID);
        
    else if (gConsoleID>=TTY_Base)
             ok= DevReadyTTY     (count,  gConsoleID);

    else {  mco= &mac_console[ gConsoleID ];
             ok= DevReadyTerminal(count, mco );
    }

    arbitrate= true;
    return ok;
} /* DevReady */



ulong SysPathWindow( syspath_typ* spP )
/* Get window pointer of a given syspath */
/* used for the "/vmod" system */
{
    ttydev_typ* mco;

    if (spP->type!=fCons) return NULL;
    
           mco= &mac_console[spP->term_id];
    return mco->consoleTermPtr;
} /* SysPathWindow */


ulong CurrentWindow( void )
/* Get window pointer of a given syspath */
/* used for the "/vmod" system */
{
    ttydev_typ* mco= &mac_console[gConsoleID];
    return      mco->consoleTermPtr;
} /* CurrentWindow */


void UpdatePrms( ulong  wPtr,   ulong  wStore, ulong wSize, 
                 ushort wIndex, ushort wTot )
{
    ttydev_typ*   mco;
    TermWindowPtr tw;
    PicHandle     ph= (byte*)wStore;
    PicHandle*    hq;
    
    int  k;
    for (k=VModBase; k<MAX_CONSOLE; k++) {
            mco= &mac_console[k];
        if (mco->installed) {
                tw= (TermWindowPtr)mco->consoleTermPtr;
            if (tw==(TermWindowPtr)wPtr) { 
                hq= &mco->pict_hdl[ wIndex ];
                     mco->pict_tot= wTot;
                
                /* this makes sure that scroll bars will not be active */ 
                tw->fTermEnvirons.textRows= 1;
                tw->fTermEnvirons.textCols= 1;
                       
//              if      (*hq!=NULL) release_mem( *hq,  true );
//                       *hq=           get_mem( wSize,true );
                if      (*hq!=NULL) DisposeHandle( *hq   );
                         *hq=           NewHandle( wSize );
                memcpy( **hq, *ph, wSize );
                return;
            }
        }
    }
} /* UpdatePrms */



/*
 *  extern long ReadCharsFromConsole(char *buffer, long n);
 *
 *  Reads from the Console into a buffer.  This function is called by
 *  read.
 *
 *  char *buffer:   Pointer to the buffer which will recieve the input.
 *  long n:         The maximum amount of characters to be read (size of
 *                  buffer).
 *  returns short:  Actual number of characters read from the stream,
 *                  -1 if an error occurred.
 */
#ifndef USE_CARBON
long ReadCharsFromConsole(char *buffer, long n)
{
    //  Get chars from the console.  If we have any in the buffer, just 
    //  return them all and the stdio routines will sort it out (calling
    //  us again if necessary).  If no chars in the buffer, then call
    //  an event loop. Return devIsReady=true if some are obtained 
    //  Do not loop here
    
    int         cnt;
    ttydev_typ* mco;
    
    /* currently read one char to have Read and DevReady consistent */ 
    if (gConsoleQuickInput) n= 1;

    if (gConsoleID<0) {               /* access to Serial Interface */
        cnt= 0;
        
        #ifndef USE_CARBON
          cnt= ReadCharsFromSerial( buffer,n, -gConsoleID );
          if (!devIsReady) HandleEvent();    /* go to the eventloop */
        #endif
        
        return cnt;
    }

    if (gConsoleID>=TTY_Base) {
        cnt= ReadCharsFromPTY   ( buffer,n,  gConsoleID );
        if (!devIsReady) HandleEvent();      /* go to the eventloop */
        return cnt;
    }

    mco= &mac_console[gConsoleID];
    cnt= ReadCharsFromTerminal  ( buffer,n,  mco );
    if (!devIsReady) HandleEvent();          /* go to the eventloop */
    return cnt;
} /* ReadCharsFromConsole */
#endif



/*
 *  extern char *__ttyname(long fildes);
 *
 *  Return the name of the current terminal (only valid terminals are
 *  the standard stream (ie stdin, stdout, stderr).
 *
 *  long fildes:    The stream to query.
 *
 *  returns char*:  A pointer to static global data which contains a C string
 *                  or NULL if the stream is not valid.
 */
#ifndef USE_CARBON
extern char *__ttyname(long fildes)
{
    #pragma unused(fildes)
    
    /* all streams have the same name */
    static char *__devicename = "null device";

    if (fildes >= 0 && fildes <= 2)
        return   __devicename;

    return 0L;
}
#endif


