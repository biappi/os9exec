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

/**********************************************/
/*             O S 9 E x e c / NT             */
/*  Cooperative-Multiprocess OS-9 emulation   */
/*         for Apple Macintosh and PC         */
/*                                            */
/* (c) 1993-2002 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        forsterb@dial.eunet.ch              */
/**********************************************/


/* includes */
/* ======== */
#include "os9exec_incl.h"



/* OS-9 event routines */
/* =================== */

void init_events(void)
{
    int  k;
    for (k=0; k<MAXEVENTS; k++) events[k].id= 0; /* invalidate event */
} /* init_events */



static event_typ* getEv( ulong evId )
{
    event_typ* ev;
    int  k;
    for (k=0; k<MAXEVENTS; k++) {
            ev= &events[k];
        if (ev->id==evId ) return ev;
    }

    return NULL;
} /* getEv */




os9err evLink( char* evName, ulong *evId )
{
    event_typ* ev;
    int        k;
    if (strlen(evName)>=OS9EVNAMELEN) return E_BNAM; /* name too long */

    for (k=0;  k<MAXEVENTS; k++) {
            ev= &events[k];
        if (ev->id!=0 && strcmp(ev->name,evName)==0 ) { 
            ev->e_linkcount++;
            *evId= ev->id;
            return 0; 
        }
    } /* for */
    
    return E_EVNF;
} /* evLink */



os9err evUnLnk( ulong evId )
{
    event_typ* ev;
    int        k;
    for (k=0;  k<MAXEVENTS; k++) {
                ev= &events[k];
        if     (ev->id!=0 && ev->id==evId ) { 
            if (ev->e_linkcount>0)
                ev->e_linkcount--;
            return 0; 
        }
    } /* for */
    
    return E_EVNTID;
} /* evUnLnk */



os9err evCreat( char* evName, int evValue, short wInc, short sInc, ulong *evId )
{
    event_typ* ev;
    int        k;

    if (strlen(evName)>=OS9EVNAMELEN) return E_BNAM; /* name too long */
        
    for (k=0; k<MAXEVENTS; k++) { /* already there ? */
            ev= &events[k];
        if (ev->id!=0 && strcmp(ev->name,evName)==0 ) return E_EVBUSY;
    } /* for */

        ev= getEv( 0 );   /* this is an event which is not yet in use */
    if (ev==NULL) return E_MEMFUL;
    
               newEventId+= EvOffs; /* get the next value */
    ev->id   = newEventId;
    strcpy(ev->name, evName );
    ev->value= evValue;
    ev->wInc = wInc;
    ev->sInc = sInc;
    ev->e_linkcount++;
    *evId    = ev->id;
    
    return 0;
} /* evCreat */



os9err evDelet( char* evName )
{
    event_typ* ev;
    int        k;

    if (strlen(evName)>=OS9EVNAMELEN) return E_BNAM; /* name too long */

    for (k=0;  k<MAXEVENTS; k++) {
            ev= &events[k];
        if (ev->id!=0 && strcmp(ev->name,evName)==0 ) {
            ev->id= 0; return 0;
        }
    } /* for */
    
    return E_EVNF;
} /* evDelet */


os9err evWait( ulong evId, int minV, int maxV, int *evValue )
{
    os9err       err;
    syspath_typ* spP;
    ulong        cnt;
    int          k, prev;
    
    event_typ*   ev= getEv( evId );
    if          (ev==NULL) return E_EVNTID;

    /* search for events from peripheral devices */ 
    for (k=0; k<MAXSYSPATHS; k++) {
            spP= get_syspath( currentpid,k );
        if (spP!=NULL && 
            spP->set_evId==evId) {
                 err= syspath_gs_ready( currentpid,k, &cnt );
            if (!err) { err= evSet( evId, 1, &prev ); break; }
        } /* if */
    }
    
    if (ev->value>=minV &&
        ev->value<=maxV) { 
        ev->value+= ev->wInc;
        *evValue  = ev->value; 
        return 0; 
    } /* if */
    
    return E_EVNTID;
} /* evWait */



os9err evSignl( ulong evId )
{
    event_typ*  ev= getEv( evId );
    if         (ev==NULL) return E_EVNTID;
    ev->value+= ev->sInc;
    return 0;
} /* evSignl */


os9err evSet( ulong evId, int evValue, int *prvValue )
{
    event_typ* ev= getEv( evId );
    if        (ev==NULL) return E_EVNTID;

    *prvValue= ev->value;
    ev->value= evValue;
    return 0;
} /* evSet */



/* eof */
