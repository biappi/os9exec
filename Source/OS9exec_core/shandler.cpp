// 
//    OS9exec,   OS-9 emulator for Mac OS, Windows and Linux 
//    Copyright (C) 2002 Lukas Zeller / Beat Forster
//	  Available under http://www.synthesis.ch/os9exec
// 
//    This program is free software; you can redistribute it and/or 
//    modify it under the terms of the GNU General Public License as 
//    published by the Free Software Foundation; either version 2 of 
//    the License, or (at your option) any later version. 
// 
//    This program is distributed in the hope that it will be useful, 
//    but WITHOUT ANY WARRANTY; without even the implied warranty of 
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//    See the GNU General Public License for more details. 
// 
//    You should have received a copy of the GNU General Public License 
//    along with this program; if not, write to the Free Software 
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
//

/**********************************************/
/*             O S 9 E x e c / NT             */
/*  Cooperative-Multiprocess OS-9 emulation   */
/*         for Apple Macintosh and PC         */
/*                                            */
/* (c) 1993-2006 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        bfo@synthesis.ch                    */
/**********************************************/

/*
 *  CVS:
 *    $Author$
 *    $Date$
 *    $Revision$
 *    $Source$
 *    $State$
 *    $Name$ (Tag)
 *    $Locker$ (who has reserved checkout)
 *  Log:
 *    $Log$
 *
 */


#include <signal.h>
#include <stdio.h>
//#include <stdexcept>
#include "shandler.h"


/*
//static void segv_handler(int sig, siginfo_t *sip, void *xxx)
static void int_handler( int sig )
{
  printf("Int handler Addr: %d\n", sig );
  fflush(0);
  		
  fnd=         (main_mco.spP->lastwritten_pid!=0);
  KeyToBuffer( &main_mco, CtrlC );
		if (fnd) return true; // do not abort program if signalled process found

//exit( 1 );
} // int_handler
*/


static void segv_handler( int /* sig */ )
{
  printf("*** Bus Error ***\n" );
  fflush(0);
  
  unsigned short err= 102;
  throw err;
} // segv_handler


void setup_exception( loop_proc lo )
{
  unsigned short err= 0;
  
  #ifdef __GNUC__
    struct sigaction sa;

    sa.sa_handler= &segv_handler;
    sigemptyset( &sa.sa_mask );
    sa.sa_flags  = 0;
    
    sigaction( SIGSEGV, &sa, NULL );
  #endif
  
  // loop as long as broken with exception
  #if defined __MACH__ || defined __GNUC__ 
    printf( "rein err=%d\n", err );
  #endif
  
  do {        
    unsigned short xErr= err; err= 0;
    try      { lo( xErr ); }
    catch( unsigned short exiterr ) {
                     err= exiterr; 
      #if defined __MACH__ || defined __GNUC__ 
        printf( "fehler\n" );
      #endif
    }
  //catch( std::exception& stdexc ) {
  //  printf( "stdexc\n" );
  //}
    catch( ... ) {
      #if defined __MACH__ || defined __GNUC__ 
        printf( "anything else\n" );
      #endif
    }
    
    #if defined __MACH__ || defined __GNUC__ 
      printf( "raus err=%d\n", err );
      fflush(0);
    #endif
  } while (err);
  
  #if defined __MACH__ || defined __GNUC__ 
    printf( "ganz err=%d\n", err );
    fflush(0);
  #endif
} // setup_exception


void throw_exception( unsigned short err )
{
  throw err;
} // throw_exception


/* eof */

