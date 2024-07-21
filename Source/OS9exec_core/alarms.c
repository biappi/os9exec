//
//    OS9exec,   OS-9 emulator for Mac OS, Windows and Linux
//    Copyright (C) 2002 Lukas Zeller / Beat Forster
//    Available under http://www.synthesis.ch/os9exec
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
 *    Revision 1.8  2004/11/27 11:51:06  bfo
 *    _XXX_ introduced
 *
 *    Revision 1.7  2004/11/20 11:44:08  bfo
 *    Changed to version V3.25 (titles adapted)
 *
 *    Revision 1.6  2004/10/22 22:51:11  bfo
 *    Most of the "pragma unused" eliminated
 *
 *    Revision 1.5  2003/05/17 11:03:20  bfo
 *    (CVS header included)
 *
 *
 */

/* includes */
/* ======== */
#include "os9exec_incl.h"

#define alarms_capacity 0xff

struct {
    alarm_typ *aa;
} alarms_all[alarms_capacity];

int alarms_curr = 0;

alarm_typ *alarm_from_id(uint32_t id)
{
    if (id > alarms_curr)
        return NULL;
    return alarms_all[id].aa;
}

void alarm_clear(uint32_t id)
{
    if (id > alarms_curr)
        return;
    alarms_all[id].aa = NULL;
}

void alarm_clear_from_ptr(alarm_typ *ptr)
{
    for (int i = 0; i < alarms_curr; i++)
        if (alarms_all[i].aa == ptr)
            alarms_all[i].aa = NULL;
}

/* OS-9 alarm routines */
/* =================== */

void init_alarms(void)
{
    int k;
    for (k = 0; k < MAXALARMS; k++) {
        alarms[k].pid  = 0; /* invalidate alarms */
        alarm_queue[k] = NULL;
    }
}

void A_Insert(alarm_typ *aa)
{
    alarm_typ *q;
    int        k;

    debugprintf(dbgProcess, dbgNorm, ("# A_Insert: aa=%08X\n", (ulong)aa));
    for (k = MAXALARMS - 1; k > 0; k--) {
        q = alarm_queue[k - 1];
        if (q != NULL) {
            if (aa->due >= q->due) {
                alarm_queue[k] = aa;
                return;
            }

            alarm_queue[k] = q;
        }
    }

    alarm_queue[0] = aa; /* if list is still empty */
}

void A_Remove(alarm_typ *aa)
{
    alarm_typ *q;
    Boolean    fnd = false;

    int k; /* be careful: index k+1 !! */

    debugprintf(dbgProcess, dbgNorm, ("# A_Remove: aa=%08X\n", (ulong)aa));
    for (k = 0; k < MAXALARMS - 1; k++) {
        q = alarm_queue[k];
        if (q != NULL && q == aa) {
            q->pid = 0;
            fnd    = true;
            alarm_clear_from_ptr(q);
        }

        if (fnd)
            alarm_queue[k] = alarm_queue[k + 1];
    }

    /* invalidate at least this */
    alarm_queue[MAXALARMS - 1] = NULL;
}

static alarm_typ *A_GetNew(ushort pid)
{
    alarm_typ *aa;
    int        k;

    for (k = 0; k < MAXALARMS; k++) {
        aa = &alarms[k];
        if (aa->pid == 0) {
            aa->pid = pid; /* activate it */
            return aa;
        }
    }

    /* no empty entry available */
    return NULL;
}

/* General routine to make alarms */
os9err
A_Make(ushort pid, uint32_t *aId, uint16_t aCode, uint32_t aTicks, Boolean cyclic)
{
    alarm_typ *aa;
    if (*aId != 0)
        return E_BPADDR;
    if (alarms_curr == alarms_capacity)
        return E_NORAM;
    aa = A_GetNew(pid);
    if (aa == NULL)
        return E_NORAM;

    aa->signal = aCode;
    aa->ticks  = aTicks;
    aa->due    = aTicks + GetSystemTick();
    aa->cyclic = cyclic;

    A_Insert(aa);
    alarms_all[alarms_curr].aa = aa;
    *aId                       = alarms_curr;
    alarms_curr++;
    return 0;
}

void A_Kill(ushort pid)
/* Remove all alarms of this process */
{
    alarm_typ *q;
    Boolean    fnd = false;
    int        k;

    debugprintf(dbgProcess, dbgNorm, ("# A_Kill: pid=%d\n", pid));
    for (k = 0; k < MAXALARMS; k++) {
        q = alarm_queue[k];
        if (q != NULL && q->pid == pid)
            A_Remove(q);
    }
}

/* ------------------------------------------------------------------ */

/* A$Delete call: 0 */
static os9err Alarm_Delete(_pid_, uint32_t aId)
{
    alarm_typ *ptr = alarm_from_id(aId);
    alarm_typ *aa;
    int        k;

    for (k = 0; k < MAXALARMS; k++) {
        aa = &alarms[k];
        if (aa != NULL && aa == ptr) {
            A_Remove(aa);
            alarm_clear(aId);
            return 0;
        }
    }

    /* can't find this alarm */
    return E_BPADDR;
}

/* A$Set call: 1 */
static os9err Alarm_Set(ushort pid, uint32_t *aId, uint16_t aCode, uint32_t aTicks)
{ return A_Make(pid, aId, aCode, aTicks, false); }

/* A$Cycle call: 2 */
static os9err Alarm_Cycle(ushort pid, uint32_t *aId, uint16_t aCode, uint32_t aTicks)
{ return A_Make(pid, aId, aCode, aTicks, true); }

/* A$AtDate call: 3 */
static os9err
Alarm_AtDate(ushort pid, uint32_t *aId, uint16_t aCode, uint32_t aTime, uint32_t aDate)
{
    uint32_t  iTime, iDate, aTicks;
    int    dayOfWk, currentTick;
    uint32_t  mx = (0xffffffff - GetSystemTick()) / SecsPerDay / TICKS_PER_SEC;
    byte   tc[4];
    ulong *tcp = (ulong *)&tc[0];

    Get_Time(&iTime, &iDate, &dayOfWk, &currentTick, false, false);

    *tcp  = os9_long(aTime);                   /* get time */
    aTime = tc[1] * 3600 + tc[2] * 60 + tc[3]; /* seconds since midnight */

    *tcp  = os9_long(aDate); /* get date */
    aDate = j_date(tc[3], tc[2], hiword(aDate));

    /* alarms in the past are not allowed */
    if (aDate < iDate)
        return E_PARAM;
    if (aDate == iDate && aTime < iTime)
        return E_PARAM;
    if (aDate - iDate >= mx)
        return E_PARAM;

    aTicks = ((aDate - iDate) * SecsPerDay + aTime - iTime) * TICKS_PER_SEC;
    return A_Make(pid, aId, aCode, aTicks, false);
}

/* A$AtJul call: 4 */
static os9err
Alarm_AtJul(ushort pid, uint32_t *aId, ushort aCode, uint32_t aTime, uint32_t aDate)
{
    uint32_t iTime, iDate, aTicks;
    int   dayOfWk, currentTick;
    uint32_t mx = (0xffffffff - GetSystemTick()) / SecsPerDay / TICKS_PER_SEC;

    Get_Time(&iTime, &iDate, &dayOfWk, &currentTick, false, false);

    /* alarms in the past are not allowed */
    if (aDate < iDate)
        return E_PARAM;
    if (aDate == iDate && aTime < iTime)
        return E_PARAM;
    if (aDate - iDate >= mx)
        return E_PARAM;

    aTicks = ((aDate - iDate) * SecsPerDay + aTime - iTime) * TICKS_PER_SEC;
    return A_Make(pid, aId, aCode, aTicks, false);
}

os9err
Alarm(ushort pid, uint32_t *aId, short aFunc, ushort sig, uint32_t aTime, uint32_t aDate)
{
#define A_Delete 0x00
#define A_Set 0x01
#define A_Cycle 0x02
#define A_AtDate 0x03
#define A_AtJul 0x04

    os9err err;

    switch (aFunc) {
    case A_Delete:
        err = Alarm_Delete(pid, *aId);
        break;
    case A_Set:
        err = Alarm_Set(pid, aId, sig, aTime);
        break;
    case A_Cycle:
        err = Alarm_Cycle(pid, aId, sig, aTime);
        break;
    case A_AtDate:
        err = Alarm_AtDate(pid, aId, sig, aTime, aDate);
        break;
    case A_AtJul:
        err = Alarm_AtJul(pid, aId, sig, aTime, aDate);
        break;
    default:
        err = E_UNKSVC;
    }

    return err;
}

/* eof */
