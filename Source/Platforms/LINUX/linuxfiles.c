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
 *    Revision 1.2  2006/02/19 23:50:16  bfo
 *    Header changed for 2006
 *
 *    Revision 1.1  2005/06/30 11:00:36  bfo
 *    Moved to the platform dependent things
 *
 *    Revision 1.5  2004/11/20 17:53:42  bfo
 *    Changed to version V3.25 as well
 *
 *    Revision 1.4  2002/10/15 17:59:09  bfo
 *    'CutUp' adapted with Prev parameter
 *
 *
 */

/* Linux specific file handling */
#include "os9exec_incl.h"
#include <utime.h>
#include <ctype.h>
#include <stddef.h>

void Conv_to_2e(char *pathname, char **qP, char **qsP)
/* ":2e" must be treated as "." and vice versa */
{
    int     L_Plen = strlen(L_P);
    ptrdiff_t  blk    = *qsP - *qP;
    Boolean fnd;

    debugprintf(dbgFiles, dbgNorm, ("# Conv_to_2e  ~>  '%s'\n", pathname));

    /* copy including NUL character at the end */
    MoveBlk(&(*qP)[L_Plen], &(*qP)[1], blk + 1); /* do not overrite itself */
    memcpy(&(*qP)[0], L_P, L_Plen);              /* add ":2e" string */

    *qP += L_Plen;      /* jump over the ":2e" sequence */
    *qsP += L_Plen - 1; /* the string is now longer */

    fnd = PathFound(pathname) || FileFound(pathname);

    while (**qP != '\0' && **qP != PATHDELIM) { /* goto next sub path */
        if (!fnd)
            **qP = toupper(**qP); /* convert to upper string, if not found */
        (*qP)++;
    }
}

Boolean CaseSens(char *pathname, char *filename, Boolean *reduS)
/* Case sensitivity handling: Make it insensitive for OS-9 */
{
    Boolean     ok = false; /* not yet found */
    DIR        *d;
    dirent_typ *dEnt;
    int         P_Len = strlen(".");
    char        tmp[OS9PATHLEN];
    char        name[DIRNAMSZ];

    *reduS = false;
    *(filename - 1) =
        '\0'; /* cut the string into two pieces, 2nd accessible by <filename> */

    debugprintf(dbgFiles,
                dbgNorm,
                ("# CaseSens:  (in) '%s' - '%s'\n", pathname, filename));

    d = opendir(pathname);
    if (d == NULL) {
        *(filename - 1) = PATHDELIM;
        return false;
    }

    strcpy(tmp, filename);
    if (strncmp(tmp, ":2e", 3) == 0) {
        tmp[0] = '.';
        tmp[1] = NUL;
        strcat(tmp, &filename[3]);
    }

    while (true) {
        dEnt = readdir(d);
        if (dEnt == NULL)
            break; /* EOF */
        GetEntry(dEnt, name, false);
        if (ustrcmp(filename, name) == 0) {
            ok = true;
            break;
        }
        if (ustrcmp(tmp, name) == 0) {
            ok     = true;
            *reduS = true;
            break;
        }
    }


    if (ok)
        strcpy(filename, dEnt->d_name); /* overwrite it */
    *(filename - 1) = PATHDELIM;        /* cut them together again */
    debugprintf(dbgFiles,
                dbgNorm,
                ("# CaseSens: (out) '%s' %d\n", pathname, ok));

    closedir(d);

    return ok;
}

os9err AdjustPath(const char *pathname, char *adname, Boolean creFile)
/* Adjusts a <pathname> to <adname> by supporting linux features
   especially the case sensitivity and specific "netatalk" things
   ( ":2e" / ".AppleDouble" )
*/
{
#define Prev "/."

    os9err  err = 0;
    size_t     len;
    Boolean fnd, reduS;
    char   *v, *q, *qs, *qc;

    /* make an copy which can be adapted */
    strncpy(adname, pathname, OS9PATHLEN);
    debugprintf(dbgFiles, dbgNorm, ("# AdjustPath (in) '%s'\n", adname));

    v = adname;
    while (true) { /* replace all " " by "_" */
        q = strstr(v, " ");
        if (q == NULL)
            break;
        *q = '_';
    }

    /* cut out /xxxx/../ sequences */
    CutUp(adname, Prev);

    /* make some preparations first */
    len = strlen(adname);
    q   = &adname[len - 1]; /* last position of the string */
    qs  = q;                /* make a copy for later use */
    debugprintf(dbgFiles, dbgNorm, ("# AdjustPath:     '%s'\n", adname));

    /* search backwards until a valid path has been found */
    while (q >= adname) {
        debugprintf(dbgFiles, dbgNorm, ("# AdjustPath  <=  '%s'\n", adname));
        if (PathFound(adname))
            break; /* if the path is found, leave the loop */

        while (q >= adname) { /* cut the next subpath */
            if (*q == PATHDELIM) {
                *q = '\0';
                break;
            }
            q--;
        }
    }

    /* and forward again with uppercase/converted strings */
    while (q < qs) {
        *q++ = PATHDELIM;

        qc = adname + strlen(adname) - 1;
        debugprintf(dbgFiles,
                    dbgNorm,
                    ("# AdjustPath  =>  '%s'%s %08x %08x\n",
                     adname,
                     creFile ? " cre" : "",
                     qc,
                     qs));

        fnd = CaseSens(adname, q, &reduS);

        if (qc == qs) { /* end of the string reached ? */
            debugprintf(dbgFiles,
                        dbgNorm,
                        ("# AdjustPath (s)  '%s' fnd:%d\n", adname, fnd));

            if (fnd || creFile) {
                if (fnd && creFile)
                    err = E_CEF; /* both options true -> problem */
                break;
            }
        }

        if (!fnd) {
            err = E_PNNF;
            break;
        }

        q  = adname + strlen(adname);
        *q = PATHDELIM;
        if (reduS) {
            *++q = NUL;
            strcat(adname, q + 2);
            qs = qs - 2;
        }

        while (*q != NUL && *q != PATHDELIM)
            q++;
    }

    debugprintf(dbgFiles,
                dbgNorm,
                ("# AdjustPath(out) '%s' err=%d\n", adname, err));
    return err;
}

void CheckDir(DIR *d)
{
    dirent_typ *dEnt;
    long int    pos;

    while (true) {
        dEnt = readdir(d);
        if (dEnt == NULL)
            break;
        pos = telldir(d);
        // printf( "%08x '%s'\n", pos, dEnt->d_name );
    }

    seekdir(d, 0);
    pos = telldir(d);
    // printf( "%08x EOF\n", pos );
}

/* eof */
