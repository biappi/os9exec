// 
// � �OS9exec,   OS-9 emulator for Mac OS, Windows and Linux 
// � �Copyright (C) 2002 �[ Lukas Zeller / Beat Forster ]
//	  Available under http://www.synthesis.ch/os9exec
// 
// � �This program is free software; you can redistribute it and/or 
// � �modify it under the terms of the GNU General Public License as 
// � �published by the Free Software Foundation; either version 2 of 
// � �the License, or (at your option) any later version. 
// 
// � �This program is distributed in the hope that it will be useful, 
// � �but WITHOUT ANY WARRANTY; without even the implied warranty of 
// � �MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// � �See the GNU General Public License for more details. 
// 
// � �You should have received a copy of the GNU General Public License 
// � �along with this program; if not, write to the Free Software 
// � �Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
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


#include "os9exec_incl.h"

/* process routines */
/* ================ */


void show_processes(void)
/* show processes */
{
    int          k;
    char         sta;
    mod_exec*    mod;
    char         idstr[10], mIDs[10];
    char*        mName;
    process_typ* cp;
    
    upo_printf(" Id S PId MId  Module   Prior  MemStart  MemEnd   Last Syscall Name\n");
    upo_printf("--- - --- --- --------- ----- --------- --------- ------------ ----------------\n");     
    for (k=0; k<MAXPROCESSES; k++) {
            cp= &procs[k];
        if (cp->state!=pUnused) {
            mod= get_module_ptr(cp->mid);
            switch (cp->state) {
                case pActive   : sta='A'; break;
                case pDead     : sta='D'; break;
                case pSleeping : sta='S'; break;
                case pWaiting  : sta='W'; break;
                case pIntUtil  : sta='I'; break;
                case pSysTask  : sta='T'; break;
                case pWaitRead : sta='S'; break;
                default        : sta='?'; break;
            }

            if (k==currentpid) mName= "iprocs";
            else {             mName= "<none>";
                if (mod!=NULL) mName= Mod_Name( mod );
            }

            sprintf( idstr,"%c%d", k==currentpid ? '*' : ' ', k );
            if (cp->mid==MAXMODULES) strcpy( mIDs,"-" );
            else                    sprintf( mIDs, "%d", cp->mid );
            
            upo_printf("%3s %c %3d %3s $%08lX %5d $%08lX $%08lX %-12s %s\n",
                        idstr,
                        sta,
                        cp->parentid,
                        mIDs,
                        (ulong) mod,
                        cp->_prior,
                        cp->memstart,
                        cp->memtop,
                        get_syscall_name(cp->lastsyscall),
                        mName);
        }
    }
} /* show_processes */


void init_processes()
/* initialize process descriptors */
{
    int  k;
    for (k=0; k<MAXPROCESSES; k++) procs[k].state= pUnused; /* invalidate this process */
    currentpid= 0; /* earlier: MAXPROCESSES; no process is running */
} /* init_processes */



os9err new_process(ushort parentid, ushort *newpid, ushort numpaths)
/* prepare a new process
 * Note: pid passed is the parent ID
 *       numpaths will not be updated (was once, but not required any more)
 */
{
    os9err      err;
    ushort      npid, k;
    process_typ *cp,*pap;
	int			dayOfWk;
    
    /* --- find empty process descriptor */
    debugprintf(dbgProcess,dbgNorm,("# new_process: parent=%d wants to create child\n",parentid));
    
    /* start searching with nr 2 (bfo) */
    /* OS-9: process 0 does not exist, process 1 is the kernel */
    for (npid=1;  npid<MAXPROCESSES; npid++) {
        if (procs[npid].state==pUnused) {
            /* this process descriptor is free, use it */
            cp= &procs[npid];
            /* initialize link to traphandler table, first entry=TRAP 1 */
            cp->os9regs.ttP=&(cp->TrapHandlers[0]);
            /* initialize flags */
            cp->os9regs.flags=
                (llm_fpu_present()      ? FLAGS_FPU : 0) |
                (llm_runs_in_usermode() ? FLAGS_UM  : 0);
            /* initialize registers */
            for (k=0; k<8; k++) {
                cp->os9regs.d[k]=0xDDDDDDD0+k;
                cp->os9regs.a[k]=0xAAAAAAA0+k;
                cp->os9regs.pc=0xCCCCCCCC;
            }
            #ifdef USE_UAEMU
            /* make sure that ISP ist ready for exception stack frames */
            cp->os9regs.isp= (ulong) &trapframebuf[TRAPFRAMEBUFLEN];
            #endif
            
            /* there was no last systemcall */
            cp->func       = STARTCALL;
            cp->lastsyscall= STARTCALL;
            
            /* reset statistics */
            cp->_uticks= 0;
            cp->_fticks= 0;
            cp->_iticks= 0;             /* julian time and date */
			Get_Time( (ulong*)&cp->_timbeg, (ulong*)&cp->_datbeg, &dayOfWk, false );
			
            cp->_fcalls= 0;  /* reset counters */
            cp->_icalls= 0;
            cp->_rbytes= 0;
            cp->_wbytes= 0;
            
            cp->exiterr    = E_PRCABT;     /* process aborted if no other code is set (through F$Exit e.g.) */          
            cp->parentid   = parentid;     /* remember parent */
            cp->siblingid  = 0;            /* has not yet siblings */
            cp->childid    = 0;            /* has not yet children */
            cp->mid        = MAXMODULES;   /* no main module yet */
            cp->memstart   = cp->memtop=0; /* no memory yet */
            cp->lastsignal = 0;            /* no signal, rteregs invalid */
            cp->masklevel  = 0;            /* sigmask is disabled by default */
            cp->icptroutine= 0;            /* no intercept routine installed */
            cp->last_mco   = NULL;         /* last console buffer */
            cp->wakeUpTick = 0;            /* to be woken up */
            cp->pW_age     = 0;            /* sleep aging */
            init_mem(npid);                /* init memory block list */
            init_traphandlers(npid);       /* init traphandlers */
            init_usrpaths    (npid);       /* init user paths */
            init_exceptions  (npid);       /* init exception handlers */
            cp->stdoutfilter= NULL;        /* no stdout filter function yet */
            cp->filtermem   = NULL;        /* no memory allocated for filter yet */
            
            /* --- inherit from parent (if any) */
            if (parentid>0 &&
                parentid<MAXPROCESSES)
                /* --- inherit from parent */
                 pap= &procs[parentid];
            else pap= &procs[1];
            
            if (npid>1) { /* nothing to inherit for the kernel process */
                /* inherit default dirs */
                cp->d.type = pap->d.type;
                cp->d.dev  = pap->d.dev; 
                cp->d.lsn  = pap->d.lsn;
                strncpy( cp->d.path, pap->d.path, OS9PATHLEN );
            
                #ifdef macintosh
                  cp->d.volID= pap->d.volID; 
                  cp->d.dirID= pap->d.dirID;
                #endif
            
                cp->x.type = pap->x.type;
                cp->x.dev  = pap->x.dev; 
                cp->x.lsn  = pap->x.lsn;
                strncpy( cp->x.path, pap->x.path, OS9PATHLEN );
            
                #ifdef macintosh
                  cp->x.volID= pap->x.volID; 
                  cp->x.dirID= pap->x.dirID;
                #endif
            
                if (parentid>0 &&
                    parentid<MAXPROCESSES) {
                
                //  strncpy( cp->d.path, pap->d.path, OS9PATHLEN );
                //  strncpy( cp->x.path, pap->x.path, OS9PATHLEN );
                
                    /* inherit standard paths */
                    if (numpaths>MAXUSRPATHS) return os9error(E_BPNUM); /* too many paths */
                    for (k=0; k<numpaths; k++) {
                        cp->usrpaths[k]= pap->usrpaths[k];
                        err=usrpath_link( npid, k, "fork " );
                
                        if (err && debugcheck(dbgProcess,dbgNorm)) {
                            uphe_printf("new_process: pid %d inherited inactive path from pid=%d: usrpath=%d (syspath=%d)\n",npid,parentid,k,cp->usrpaths[k]);
                            debug_halt( dbgProcess );
                        }
                        if (err) {
                            /* inheriting closed paths is allowed, simply pass them to child as unused */
                            cp->usrpaths[k]= 0; /* forget that one */
                        }
                    }
                }
                else {
                //  strncpy( cp->d.path, startPath, OS9PATHLEN );
                //  strncpy( cp->x.path, startPath, OS9PATHLEN );
                //
                //      p= egetenv("OS9DISK"); /* get path for default exe dir */
                //  if (p!=NULL) strncpy( cp->d.path,p, OS9PATHLEN );
                //
                //      p= egetenv("OS9CMDS"); /* get path for default exe dir */
                //  if (p!=NULL) strncpy( cp->x.path,p, OS9PATHLEN );
                //  else {
                //      strncpy( cp->x.path, cp->d.path,OS9PATHLEN-6);
                //      strcat ( cp->x.path, PATHDELIM_STR );
                //      strcat ( cp->x.path, "CMDS"        );
                //      strcat ( cp->x.path, PATHDELIM_STR );
                //  }

                    /* by default there is just one systempath */
                    cp->usrpaths[0]= sysStdin;
                    cp->usrpaths[1]= sysStdin;
                    cp->usrpaths[2]= sysStdin;
                
                    /* inherit stdin/stdout/stderr from MPW */
                    if (numpaths>1) cp->usrpaths[1]=sysStdout;
                    if (numpaths>2) cp->usrpaths[2]=sysStderr;
                }
            } /* if (npid>1) */
            
            /* --- process descriptor prepared ok */
            debugprintf(dbgProcess,dbgNorm,("# new_process: created pid=%d\n",npid));
            /* --- its only here where we activate the process */
            cp->state=pDead; /* correct state must be set afterwards, or process will be dead from start */
            *newpid=npid; /* return new PID */
            return 0;
        }
    }
    /* all processes used, can't prepare new process */
    return os9error(E_PRCFUL);
} /* new_process */



static void AssignNewChild( ushort parentid, ushort pid )
{
    ushort*  idp= &procs[parentid].childid;
    while  (*idp!=0) {
        if (*idp==pid) {            
            *idp= procs[*idp].siblingid;
            debugprintf(dbgProcess,dbgNorm,("# Assign new child: pid=%d\n",*idp));
            return;
        }
        
        idp= &procs[*idp].siblingid;
    } /* while */
} /* AssignNewChild */



os9err kill_process(ushort pid)
/* kill a process
 * Note: exiterr must be set before calling kill_process (by F_Exit or F_Kill)
 */
{
    const        funcdispatch_entry *fdeP;
    process_typ* cp= &procs[pid];
    process_typ* kp;
    regs_type*   rp;
    ushort       parentid,k;
    
    if (cp->state==pUnused) return os9error(E_IPRCID); /* process does not exist */
    debugprintf(dbgProcess,dbgNorm,("# kill_process: killing pid=%d, parentid=%d, exiterr=%d\n",
                                       pid,cp->parentid,cp->exiterr));

    /* remove some more resources */
    close_usrpaths     ( pid );
    unlink_traphandlers( pid );
    
    /* first orphan eventual children of the process */
    for (k=0; k<MAXPROCESSES; k++) {
    			kp= &procs[k];
        if     (kp->state   !=pUnused &&
                kp->parentid==pid) {
                kp->parentid=0; /* earlier: MAXPROCESSES */
            if (kp->state==pDead) kp->state= pUnused;
            
            debugprintf(dbgProcess,dbgNorm,("# kill_process: orphaned child pid=%d\n",k));
        }
    }
    if (cp->state==pDead) return os9error(E_IPRCID); /* avoid killing again, because double close is not good */

    /* now kill the process */
        parentid= cp->parentid; /* get parent ID */
    if (parentid>0 &&
        parentid<MAXPROCESSES) {
        /* there is a parent process */
        cp->state= pDead; /* keep it there until parent recognizes */
        if (pid==interactivepid) interactivepid=parentid; /* direct Cmd-'.' to parent now */
        AssignNewChild( parentid,pid );
    }

    else {
        /* there's no parent */
        cp->state= pUnused; /* invalidate descriptor */
    } /* if */

    /* module can't be unlinked as long as process is not pDead or pUnused */
    unlink_module( cp->mid );
    cp->mid= MAXMODULES; /* invalidate it */
    
    if (pid==currentpid) {
        debugprintf(dbgProcess,dbgNorm,("# kill_process: switch to parentid=%d (%s)\n",
                                           parentid, PStateStr(&procs[parentid])));
        /* current process has gone, so activate its parent */
            currentpid= parentid; /* in case of wait, adapt d0+d1 */
        if (currentpid!=0) {
                fdeP= getfuncentry(procs[currentpid].func);
            if (fdeP->func==OS9_F_Wait) {
                debugprintf(dbgProcess,dbgNorm,("# kill_process: new d0=%d d1=%d\n",
                                               pid, cp->exiterr));
                rp= &procs[currentpid].os9regs;
                retword(rp->d[0])= pid;                /* ID of dead child */
                retword(rp->d[1])= cp->exiterr; /* exit error of child */
            }
            
            arbitrate= false; /* don't arbitrate, simply wake/unwait current if needed */
        }
        else
            arbitrate= true; /* not a valid ID -> search a new process */
    }

    /* now dispose all the process' resources */
    if (cp->last_mco!=NULL) {
        cp->last_mco->spP->lastwritten_pid= 0; /* disconnect CtrlC/E signal */
        cp->last_mco= NULL;
    }
    
    A_Kill  ( pid ); /* remove all alarms of this process */
    free_mem( pid ); /* IMPORTANT: use this only after all other resources are freed (because they might have memory allocated, such as traphandlers) */
    if (cp->stdoutfilter!=NULL) releasefilter(&cp->filtermem); /* release filter */
    debugprintf(dbgProcess,dbgNorm,("# kill_process: successfully killed pid=%d\n",pid));
    return 0;
} /* kill_process */


/* send a signal to a process */
os9err send_signal(ushort pid, ushort signal)
{
    process_typ* cp  = &procs[currentpid]; /* ptr to my procs dsc */
    process_typ* sigp= &procs[       pid]; /* ptr to procs dsc to which the signal will be sent */
    sig_typ*     s   = &sig_queue;
	int          k;
	
	/* the broadcast (send to pid=0) is implemented here */
	if (currentpid!=0 && pid==0) {
		for (k=1; k<MAXPROCESSES; k++) {
			if (cp->_group==procs[k]._group &&
			    cp->_user ==procs[k]._user  &&
			    currentpid!=k) send_signal( k,signal );
		} /* for */
		
		return 0;
	} /* if */


    if (currentpid==0) cp= sigp;
    debugprintf(dbgProcess,dbgNorm,("# send signal=%d to pid=%d (%s) from current pid=%d\n",
                                       signal,pid, PStateStr(sigp), currentpid ));

    if (sigp->state==pUnused || /* bad process */
        sigp->state==pDead) return os9error(E_IPRCID);

    if (signal==S_Kill) {
        /* unconditionally kill process */
        sigp->exiterr= os9error(E_PRCABT); /* process aborted */
        kill_process( pid );
        return 0;
    }

    if (signal==S_Wake && pid==currentpid) return 0; /* ignore this */
    
    if    (cp->way_to_icpt || sigp->masklevel>0 || !async_area) {
        if (signal==S_Wake && sigp->masklevel>0) return 0; /* needn't to be stored in this case */
        
        s= &sig_queue; /* store it, but don't do anything */
        s->pid   [s->cnt]= pid;
        s->signal[s->cnt]= signal;
        
        if (s->cnt<MAXSIGNALS) s->cnt++;
        debugprintf(dbgProcess,dbgNorm,("# send signal=%d to pid=%d queued (level=%d)\n",
                                           signal,pid, s->cnt ));
        /* synchronise it (must be inside the os9exec area) */
        if (!async_area && sigp->masklevel==0) async_pending= true;
        return 0;
    }
    
    cp->icpt_pid   = pid;    /* keep it save */
    cp->icpt_signal= signal;
        
    /* first, wakeup process, if sleeping */
    if (sigp->state==pSleeping) {
        debugprintf(dbgProcess,dbgNorm,("# send_signal: waking pid=%d from sleep\n",pid));
        sigp->state= pActive;
        sigp->os9regs.d[0]= 0;      /* %%% return # of remaining ticks! */
        sigp->os9regs.sr &= ~CARRY; /* error-free return */
        sigp->way_to_icpt = true;   /* activate both */
          cp->way_to_icpt = true;
        arbitrate= false;           /* don't arbitrate, continue with woken-up process */
    
        if (currentpid==0 && signal==S_Wake) {
            currentpid= pid;
            arbitrate= true;
        }
    }

    if (signal!=S_Wake) {
        /* not wake, additional processing required */
        if (sigp->lastsignal!=0) return os9error(E_USIGP); /* behave like old OS9 <2.2 */
                  /* should never be called, because now a signal queue is implemented */

        if (sigp->state==pWaiting) {
            /* signal causes wakeup */
            debugprintf(dbgProcess,dbgDetail,("# send_signal: receiving pid=%d was waiting\n",pid));
            retword(sigp->os9regs.d[0])= 0;      /* return 0 means that no process has died (pid=0's death can't be awaited) */
            retword(sigp->os9regs.d[1])= signal; /* return signal code as error */
                    sigp->os9regs.sr  &= ~CARRY; /* error-free return */
        }
        else {
            sigp->state= pActive;        /* activate for all other cases */
        }
        
        sigp->rtestate= sigp->state;     /* save it, active after signal */ 
        arbitrate= false;
        sigp->lastsignal= signal;        /* signal currently being processed */

        /* now, check if target can catch signal */
        if (sigp->icptroutine!=0) { /* prepare execution of intercept routine */
            sigp->way_to_icpt = true;    /* activate both */
              cp->way_to_icpt = true;
            sigp->rteregs     = sigp->os9regs; /* save all regs */
            sigp->os9regs.pc  = sigp->icptroutine;
            sigp->os9regs.a[6]= sigp->icpta6;
            sigp->os9regs.d[1]= signal;
            
            if (currentpid==0) { /* for this special case we have to do it here */
                currentpid= pid;
                arbitrate= true;
            }
            
            debugprintf(dbgProcess,dbgNorm,("# send_signal=%d => continuing in icpt routine of pid=%d\n", signal,pid ));
            /* execution will continue in intercept routine */
        }
        else {
            /* abort process */
            debugprintf(dbgProcess,dbgNorm,("# send_signal=%d => pid=%d has no icpt routine, so it will be killed\n", signal,pid ));
            sigp->exiterr= signal; /* return signal as abort code */
            kill_process( pid );            
            sigp->way_to_icpt = false; /* don't handle it as intercept */
              cp->way_to_icpt = false;
        }
    }   

    return 0; /* signal sent successfully */
} /* send_signal */



os9err sigmask( ushort cpid, int level )
{
    os9err err;
    process_typ* cp= &procs[cpid];  /* ptr to procs descriptor */
    int*   plv= &cp->masklevel;
    sig_typ*      s= &sig_queue;
    int    ii;
    ushort pid, signal;
    
    switch (level) {
        case  0 :  *plv= 0; break;
        case  1 : (*plv)++; break;
        case -1 : (*plv)--; break;
    }
    
    if (*plv<=0 && s->cnt>0) {
        pid   = s->pid   [0]; /* use it as a fifo, save them first */
        signal= s->signal[0];
        
                      s->cnt--;
        for (ii=0; ii<s->cnt; ii++) { /* remove one element from stack */
            s->pid   [ii]= s->pid   [ii+1];
            s->signal[ii]= s->signal[ii+1];
        }
        
        if (s->cnt<=0) async_pending= false;
        err= send_signal( pid,signal );
    }
    
    return 0;
} /* sigmask */



void kill_processes()
/* kill all running processes and cleanup their resources */
{
    int  k;
    for (k=0; k<MAXPROCESSES; k++) {
        if (procs[k].state!=pUnused) {
            kill_process(k);
        }
    }
} /* kill_processes */



static void wait_for_signal( ushort pid )
/* this routine looks for incoming data from any of the connected */
/* network sockets */
{
    os9err       sig= 0; /* yes it is used (=assigned), even if there is no net support */
    syspath_typ* spP;
    int          k, sp;
    int          sv= currentpid;
    process_typ* cp= &procs[pid]; /* ptr to procs descriptor */
    
    HandleEvent();
    
    currentpid= 0;/* switch off */
    for (k=0; k<MAXUSRPATHS; k++) {
            sp= cp->usrpaths[k];
        if (sp>0 && sp<MAXSYSPATHS) {
                spP= get_syspath( pid,sp );
            if (spP!=NULL && 
                spP->type==fNET) {
                #ifdef WITH_NETWORK
                      sig= pNask( pid, spP );
                  if (sig) send_signal( pid,sig );
                #endif
            }
        } /* if */
    }
    currentpid= sv;
} /* wait_for_signal */


/* arbitrate and prepare next process
 * Note: checks global flag "arbitrate" to see if global "currentpid"
 *       should be changed to next waiting/active process
 */
void do_arbitrate(void)
{
    ushort       pid, cpid, spid;
    ushort       sleepingpid, deadpid;
    Boolean      done;
    process_typ  *cp, *sprocess;
    
    debugprintf(dbgTaskSwitch,dbgDetail,("# arbitrate: current pid=%d, arbitrate=%d\n",currentpid,arbitrate));
    sleepingpid= MAXPROCESSES; /* assume none sleeping */
    deadpid    = MAXPROCESSES; /* assume none dead */
    cpid       = currentpid; /* save */
    
    /* first, check if current process is valid at all (if not, force arbitration) */
    if (cpid>=MAXPROCESSES) { arbitrate=true; currentpid=cpid=0; } /* arbitrate anyway, start at first process */
    cp= &procs[cpid];

    if (!arbitrate) {
        if (cp->state==pSysTask ||                 /* we need aritration or we'll get stuck in systasks */
            cp->state==pUnused  ||                 /* unused, arbitrating needed */
            cp->state==pWaitRead) arbitrate= true; /* give a chance to other processes */
    }
    
    /* now arbitrate if needed */
    spid    =  currentpid;
    sprocess= &procs[spid];

    done= false;
    debugprintf(dbgTaskSwitch,dbgDetail,("# arbitrate: after correction: current pid=%d (state=%d), arbitrate=%d\n",
                                            cpid,PStateStr(cp),      arbitrate));
    do {
        debugprintf(dbgTaskSwitch,dbgDeep,("# arbitrate: checking pid=%d (state=%d), arbitrate=%d\n",
                                            spid,PStateStr(sprocess),arbitrate));
        if (arbitrate) {
            /* next process */
                spid++;
            if (spid>=MAXPROCESSES) spid= 0; /* wrap */
            if (spid==1) spid++;             /* avoid process 1 */
            sprocess= &procs[spid]; /* do it in correct order */
            
            /* --- test if all processes tested */
            if (cpid==spid) {
                /* -- no other process found to run */
                debugprintf(dbgTaskSwitch,dbgDetail,("# arbitrate: checked all, now pid=%d must be startable\n",spid));
                if  (sprocess->state==pSleeping) {
                    #if defined macintosh || defined(windows32)
                      ulong ticks   = GetSystemTick();
                      HandleEvent();
                      slp_idleticks+= GetSystemTick()-ticks;
                                          
                    #elif defined linux
                      usleep( 1 ); /* sleep in milliseconds */
                      slp_idleticks++;
                    #endif
                } 
                else done= true;
            }
            else if (sprocess->state==pUnused) continue; /* fast forward */
        } /* if arbitrate */
        
        /* --- test if process can be run */
        if (sprocess->state==pWaiting ||
            sprocess->state==pWaitRead) {
            /* --- search if there is a dead child of that process */
            for (pid=0; pid<MAXPROCESSES; pid++) {
                if (procs[pid].parentid==spid && procs[pid].state==pDead) {
                    deadpid= pid;
                    break;
                }
            }
            if (pid<MAXPROCESSES) break; /* yes, there is a dead child, we can unwait */
        } 
        
        /* --- check if process can be activated */
        if     (sprocess->state==pActive  ||       /* process can run, if one of these states */
                sprocess->state==pSysTask) break;

        if     (sprocess->state==pWaitRead) {      /* only every nth time for this mode */
            if (sprocess->pW_age--<=0) { 
                sprocess->pW_age= NewAge;  break;
            }
            done= false;
        }

        if (sprocess->state==pSleeping && /* slow down also here */
            sprocess->pW_age--<=0) {
            sprocess->pW_age= NewAge;
            
        	async_area= true; 
			if (async_pending) sigmask( spid,0 ); /* disable signal mask */
			/* asynchronous signals are allowed here */
			
            wait_for_signal( spid );
  			async_area= false; 
			/* asynchronous signals are no longer allowed */
                        
            if (sprocess->wakeUpTick < GetSystemTick()) {
                sprocess->os9regs.d[0]= 0;      /* no remaining ticks */
                sprocess->os9regs.sr &= ~CARRY; /* error-free return */
                sprocess->state= pActive; break;
            }

            sleepingpid= spid; /* remember sleeping process */  
        }
        
        if (sprocess->way_to_icpt) break;
    
//      /* --- b.t.w, remember sleeping processes */        
//      if (cp->state==pSleeping) sleepingpid=currentpid; /* remember sleeping process */   

        /* --- after one round, arbitration starts anyway */
        arbitrate= true; /* now advance anyway */
        if  (done) spid= MAXPROCESSES; /* should have exited via break by now */
    } while(!done);
    
    /* assign */
    currentpid= spid;
    
    
    /* now, if currentpid<MAXPROCESSES, there is a process to run or unwait-and-run
     * otherwise, only sleeping processes (if sleepingpid<MAXPROCESSES)
     * or no processes at all are left. 
     */
    if (currentpid<MAXPROCESSES) {
            cp= &procs[currentpid];
        if (cp->state==pWaiting && deadpid<MAXPROCESSES) {
            /* -- we need to terminate waiting first */
            retword(cp->os9regs.d[0])=       deadpid;          /* ID of dead child */
            retword(cp->os9regs.d[1])= procs[deadpid].exiterr; /* exit error of child */
            debugprintf(dbgTaskSwitch,dbgNorm,("# arbitrate: waiting pid=%d gets active because child pid=%d is dead (exiterr=%d)\n",currentpid,deadpid,procs[deadpid].exiterr));
            procs[deadpid].state  = pUnused;
            cp->state       = pActive; /* process is now active */
            cp->os9regs.sr &= ~CARRY; /* error-free return */
        }           
    }
    else currentpid= sleepingpid; /* is there a sleeping process ? */
} /* do_arbitrate */



static os9err os9exec_compatible( mod_exec* mod )
/* this is the list of modules, which can't run under OS9exec */
{
    char* p;
    short ed;

    p = Mod_Name( mod );
    ed= os9_word( mod->_mh._medit );
    
    if (ustrcmp(p,"deldir" )==0 && ed<=100) return E_BADREV; /* shrinking directory entries not supported */
    if (ustrcmp(p,"sysdbg" )==0 && ed<=100) return E_BADREV; /* crashes right at the beginning */
    
    return 0;
} /* os9exec_compatible */



void stop_os9exec(void)
/* print a messsage and stop the OS9 emulator */
{
	/* write message to main system path and stop emulator directly */
	quitFlag= true;
    usrpath_printf( 0,MAXUSRPATHS, "\n" );
    usrpath_printf( 0,MAXUSRPATHS, "# OS9 emulation ends here.\n" ); 
    fflush(stdout);

    exit(0); /* never come back */
} /* stop-os9exec */



void lw_pid( ttydev_typ* mco )
/* set the last written pid */
{
    if   (mco->spP->lastwritten_pid==gLastwritten_pid ) return;
    procs[mco->spP->lastwritten_pid].last_mco= NULL;   /* switch off old one */
          mco->spP->lastwritten_pid= gLastwritten_pid; /* assign for later use */
    procs[mco->spP->lastwritten_pid].last_mco= mco;    /* activate new */
} /* lw_pid */



os9err setprior(ushort pid, ushort newprior)
/* set priority (and update IRQblock flag */
{
    process_typ* cp= &procs[pid]; /* ptr to procs descriptor */

    if  (pid>=MAXPROCESSES)  return os9error(E_IPRCID);
    if  (cp->state==pUnused) return os9error(E_IPRCID);
         cp->_prior= newprior;
        
    if (newprior<IRQBLOCKPRIOR)
         cp->os9regs.flags |=   FLAGS_IRQ;  /* allow IRQs (and VBL) during OS9 code execution */
    else cp->os9regs.flags &= (~FLAGS_IRQ); /* disallow any IRQs during OS9 code execution */
    
    return 0;
} /* setprior */



/* prepare OS9 program module for fork
 * Note: procs[pid] must be already prepared (by new_process())
 */
os9err prepFork( ushort newpid,   char*  mpath,    ushort mid,
                 byte*  paramptr, ulong  paramsiz, ulong memplus,
                 ushort numpaths, ushort grp, ushort usr, ushort prior, Boolean *intCmd )
{
    byte         *mp,*p,*p2;
    ulong*       a;
    ulong        memsiz,cnt;
    ushort       err, mty;
    mod_exec*    theModule;
    process_typ* cp= &procs[newpid];
    process_typ* pp= &procs[cp->parentid];  
    regs_type*   rp= &cp->os9regs;

    #ifdef INT_CMD
      ushort     argc;
      char**     arguments;
    #endif  


    /* save main module ID */
    cp->mid=  mid;
    setprior( newpid,prior );
    
    /* inherit group and user */
    cp->_group= grp;
    cp->_user = usr;
    
    *intCmd= false; /* no internal command by default */
    #ifdef INT_CMD
          *intCmd= (isintcommand(mpath)>=0);
      if (*intCmd) {
          cp->state= pIntUtil; /* internal utility        */
          cp->mid  = 0;        /* assign "OS9exec" module */

          /* prepare args */
          prepArgs( paramptr, &argc,&arguments );
          arguments[0]= (char*)mpath;  /* set module name */
          
          if (pp->childid!=0) pp->siblingid= pp->childid;
          
          pp->childid= newpid; /* this is the child */
          currentpid = newpid; /* use the correct identification */
          
          /* execute command */
          err= callcommand( mpath,newpid, argc,arguments );
            
          /* simulate successful F$Exit of dummy process */
          release_mem( arguments,false );
          
          cp->exiterr= err;
          kill_process( newpid );
          return 0; /* internal-tool "fork" successful */
      } /* if isintcommand */
    #endif

    
           
    /* -- get pointer to main module */
    theModule= os9mod( mid );

    /* check if module is executeable and */
    /* check if this module is not in the "black list" of OS9exec */
        mty= os9_word( theModule->_mh._mtylan )>>BpB;
    if (mty!=MT_PROGRAM) err= E_NEMOD;
    else                 err= os9exec_compatible( theModule ); 

    if (err) { unlink_module( mid ); return err; }

    debugprintf(dbgProcess,dbgNorm,("# prepFork: Module mid=%d, address=$%08lX\n",mid,(ulong) theModule));

    /* -- prepare registers */
    rp->sr=0; /* everything cleared, USER state */
    rp->pc=os9_long(theModule->_mexec)+(ulong)theModule; /* entry point */
    rp->a[3]=(ulong)theModule; /* primary module pointer */
    rp->d[0]= newpid;    /* assign process ID */
    rp->d[1]= cp->_group<<(2*BpB)|
              cp->_user; /* inherited group/user */
    rp->d[2]= prior;     /* priority */
    rp->d[3]= numpaths;  /* number of paths inherited */
   
    /* -- prepare data area */
    debugprintf(dbgProcess,dbgDetail,("# prepFork: extra memory=%ld (= paramsiz:%ld + memplus:%ld)\n",memplus+paramsiz,paramsiz,memplus));
    err= prepData( newpid,theModule,memplus+paramsiz,&memsiz,&mp );
    if (err!=0) return err; /* no room for data */
    cp->memstart=(ulong)mp; /* save static storage start address */
    rp->a[6]=(ulong)mp+0x8000; /* biased A6 */
    rp->membase=mp; /* unbiased static storage pointer */

    /* -- copy parameter area */
    p=paramptr; p2=mp+memsiz-paramsiz;
    regcheck( newpid,"Param writing start",(ulong)p2,           RCHK_MEM );
    regcheck( newpid,"Param writing end",  (ulong)p2+paramsiz-1,RCHK_MEM );
    for (cnt=0;cnt<paramsiz;cnt++) *(p2++)=*(p++);

    /* set up parameter area regs */
    rp->a[5]= rp->a[7]=   (ulong)(mp+memsiz-paramsiz); /* parameter area start, top of stack */
    rp->a[1]= cp->memtop= (ulong)(mp+memsiz);          /* memory end */
    rp->d[5]= paramsiz; /* parameter size */
    rp->d[6]= memsiz; /* total initial memory allocation */

    /* prepare sigdat content */
     a= (ulong*)&cp->sigdat[0x12];
    *a= os9_long(cp->memtop-paramsiz);

    /* check for stdout filter and init */
    cp->stdoutfilter= initfilterfunc( Mod_Name( theModule ), paramptr, &cp->filtermem);
    debugprintf(dbgProcess,dbgNorm,("# prepFork: stdoutfilter=$%lX, filtermem=$%lX\n",cp->stdoutfilter,cp->filtermem));
    debugprintf(dbgProcess,dbgNorm,("# prepFork: Everything's ready for launch!\n"));

    return 0; /* ok */
} /* prepFork */


/* eof */