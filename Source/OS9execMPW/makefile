#   File:       makefile
#   Target:     os9/os968k/os9ppc
#   Created:    Wednesday, October 9, 1996 10:56:43 AM
#   Changed:    - Montag, 26. Oktober 1998 1:52:13 PM, LuZ: Tried with CW 4.0,
#                 added more warning suppress options to MWC-cmdline.
#               - Montag, 16. August 1999 1:20:27 PM, LuZ: Changed for
#                 new Directory structure
#               - 00/12/27 Bfo: filescsi.c included
#               - 01/03/25 Bfo: some other .h files included
#


# directories
ObjDir¥PPC   =  :::Output:OBJPPC:
ObjDir¥68K   =  :::Output:OBJ68k:
output       =  :::Output:

os9mpw       =
os9exec      =  ::os9exec_core:
platforms    =  ::platforms:


# everything

all Ä tools

# all OS-9 MPW tools
tools Ä os9 os968k os9ppc

# single OS-9 MPW tool
os9 Ä {output}os9

os968k Ä {output}os968k

os9ppc Ä {output}os9ppc


# tool names
toolname68k Ä
    set outname "os968k"

toolnamePPC Ä
    set outname "os9ppc"

toolnameFat Ä
    set outname "os9"


PROJHEADERS = ¶
        "{os9mpw}target_options.h" ¶
        "{platforms}net_platform.h" ¶
        "{platforms}macfiles.h" ¶
        "{os9exec}os9main_incl.h" ¶
        "{os9exec}os9_llm.h" ¶
        "{os9exec}os9funcs.h" ¶
        "{os9exec}os9exec_nt.h" ¶
        "{os9exec}os9main.h" ¶
        "{os9exec}funcdispatch.h" ¶
        "{os9exec}debug.h" ¶
        "{os9exec}utilstuff.h" ¶
        "{os9exec}procstuff.h" ¶
        "{os9exec}modstuff.h" ¶
        "{os9exec}memstuff.h" ¶
        "{os9exec}network.h" ¶
        "{os9exec}alarms.h" ¶
        "{os9exec}events.h" ¶
        "{os9exec}consio.h" ¶
        "{os9exec}pipefiles.h" ¶
        "{os9exec}fileaccess.h" ¶
        "{os9exec}filescsi.h" ¶
        "{os9exec}file_rbf.h" ¶
        "{os9exec}filestuff.h" ¶
        "{os9exec}fcalls.h" ¶
        "{os9exec}icalls.h" ¶
        "{os9exec}intcommand.h" ¶
        "{os9exec}vmod.h" ¶
        "{os9exec}telnetaccess.h" ¶
        "{os9exec}filters.h"




#MAKEFILE     = makefile
MAKEFILE     =
#¥MondoBuild¥ = {PROJHEADERS} {MAKEFILE}  # Make blank to avoid rebuilds when makefile is modified
#¥MondoBuild¥ = {PROJHEADERS}
¥MondoBuild¥ =

PROJINCLUDES = -i {os9exec} -i {platforms}
#Includes    =  {PROJINCLUDES} -i "{HeadersAndLibs}Interfaces:CIncludes:"
# Note: MWC uses implicit include paths which should do the trick!
Includes     = {PROJINCLUDES}

Sym¥PPC      =  
Sym¥68k      =  


C           =   MWC68K
PPCC        =   MWCPPC
Link        =   MWLink68k
PPCLink     =   MWLinkPPC


CDEFINES    = 
#CDEFINES   = -d NODEBUG

COMMONCOPTS = {CDEFINES} -w on,noemptydecl,nopragmas,nopossible,noextended,noimplicitconv,nounusedvar,nounusedarg -mpw_chars -mapcr

PPCCOptions = {Includes} {Sym¥PPC} {COMMONCOPTS}

COptions    = {Includes} {Sym¥68K} {COMMONCOPTS} -model far -mc68020

#        [no]hidevirtual     # warn on hidden virtual functions
#        [no]pragmas         # warn on illegal pragmas
#        [no]emptydecl       # warn on empty declarations
#        [no]possible        # warn on possible errors
#        [no]unusedvar       # warn on unused variables
#        [no]unusedarg       # warn on unused arguments
#        [no]extracomma      # warn on extra commas
#        [no]extended        # even more possible problems
#        [no]params          # warn on suspicious, obsolete, or substituted



AOptions     = -model near -sym on

CodeRes¥68k  = ¶
        "{ObjDir¥68k}os9_llm_ppc.a.o"

Objects¥PPC  = ¶
        "{ObjDir¥PPC}consio.c.x" ¶
        "{ObjDir¥PPC}debug.c.x" ¶
        "{ObjDir¥PPC}fcalls.c.x" ¶
        "{ObjDir¥PPC}fileaccess.c.x" ¶
        "{ObjDir¥PPC}filescsi.c.x" ¶
        "{ObjDir¥PPC}file_rbf.c.x" ¶
        "{ObjDir¥PPC}filestuff.c.x" ¶
        "{ObjDir¥PPC}icalls.c.x" ¶
        "{ObjDir¥PPC}intcommand.c.x" ¶
        "{ObjDir¥PPC}filters.c.x" ¶
        "{ObjDir¥PPC}int_dir.c.x" ¶
        "{ObjDir¥PPC}int_move.c.x" ¶
        "{ObjDir¥PPC}int_rename.c.x" ¶
        "{ObjDir¥PPC}macfiles.c.x" ¶
        "{ObjDir¥PPC}memstuff.c.x" ¶
        "{ObjDir¥PPC}network.c.x" ¶
        "{ObjDir¥PPC}net_mac.c.x" ¶
        "{ObjDir¥PPC}alarms.c.x" ¶
        "{ObjDir¥PPC}events.c.x" ¶
        "{ObjDir¥PPC}modstuff.c.x" ¶
        "{ObjDir¥PPC}os9exec_nt.c.x" ¶
        "{ObjDir¥PPC}os9main.c.x" ¶
        "{ObjDir¥PPC}funcdispatch.c.x" ¶
        "{ObjDir¥PPC}os9_llm.c.x" ¶
        "{ObjDir¥PPC}pipefiles.c.x" ¶
        "{ObjDir¥PPC}procstuff.c.x" ¶
        "{ObjDir¥PPC}utilstuff.c.x" ¶
        "{ObjDir¥PPC}vmod.c.x" ¶
        "{ObjDir¥PPC}telnetaccess.c.x" ¶
        "{ObjDir¥PPC}os9.c.x"

Objects¥68K  = ¶
        "{ObjDir¥68K}consio.c.o" ¶
        "{ObjDir¥68K}debug.c.o" ¶
        "{ObjDir¥68K}fcalls.c.o" ¶
        "{ObjDir¥68K}fileaccess.c.o" ¶
        "{ObjDir¥68K}filescsi.c.o" ¶
        "{ObjDir¥68K}file_rbf.c.o" ¶
        "{ObjDir¥68K}macfiles.c.o" ¶
        "{ObjDir¥68K}filestuff.c.o" ¶
        "{ObjDir¥68K}icalls.c.o" ¶
        "{ObjDir¥68K}intcommand.c.o" ¶
        "{ObjDir¥68K}filters.c.o" ¶
        "{ObjDir¥68K}int_dir.c.o" ¶
        "{ObjDir¥68k}int_move.c.o" ¶
        "{ObjDir¥68k}int_rename.c.o" ¶
        "{ObjDir¥68K}macfiles.c.o" ¶
        "{ObjDir¥68K}memstuff.c.o" ¶
#       "{ObjDir¥68K}network.c.o" ¶
#       "{ObjDir¥68K}net_mac.c.o" ¶
        "{ObjDir¥68K}alarms.c.o" ¶
        "{ObjDir¥68K}events.c.o" ¶
        "{ObjDir¥68K}modstuff.c.o" ¶
        "{ObjDir¥68K}os9exec_nt.c.o" ¶
        "{ObjDir¥68K}os9main.c.o" ¶
        "{ObjDir¥68K}funcdispatch.c.o" ¶
        "{ObjDir¥68K}os9_llm_68k.a.o" ¶
        "{ObjDir¥68K}os9_llm.c.o" ¶
        "{ObjDir¥68K}pipefiles.c.o" ¶
        "{ObjDir¥68K}procstuff.c.o" ¶
        "{ObjDir¥68K}utilstuff.c.o" ¶
        "{ObjDir¥68K}vmod.c.o" ¶
        "{ObjDir¥68K}telnetaccess.c.o" ¶
        "{ObjDir¥68K}os9.c.o"

doLinkPPC = ¶
    delete -i {output}{outname}; ¶
    {PPCLink} ¶
        -o "{output}{outname}" "{Sym¥PPC}" ¶
        -xm mpwtool ¶
        -merge off ¶
        "{SharedLibraries}InterfaceLib" ¶
        "{SharedLibraries}MathLib" ¶
        "{MWPPCLibraries}MSL MPWCRuntime.Lib"   ¶
        "{MWPPCLibraries}MSL C.PPC MPW(NL).Lib"   ¶
        "{PPCLibraries}PPCToolLibs.o" ¶
        "{SharedLibraries}OpenTransportLib" ¶
        "{PPCLibraries}OpenTransportExtnPPC.o" ¶
        "{SharedLibraries}OpenTptInternetLib" ¶
        "{SharedLibraries}ThreadsLib" ¶
        "{PPCLibraries}OpenTptInetPPC.o" ¶
        "{MWPPCLibraries}PLStringFuncsPPC.lib" ¶
        {Objects¥PPC}

doLink68k = ¶
    {Link} ¶
        -o "{output}{outname}" -d "{Sym¥68K}" ¶
        -xm mpwtool ¶
        -model far ¶
        "{MW68KLibraries}MSL C.68K MPW(NL_4i_8d).Lib" ¶
        "{MW68KLibraries}MSL MPWRuntime.68K.Lib" ¶
        "{MW68KLibraries}MSL Runtime68K.Lib" ¶
        "{MW68KLibraries}MacOS.lib" ¶
        "{MW68KLibraries}MathLib68K (4i_8d).lib" ¶
        #"{MW68KLibraries}PLStringFuncsCFM68k.lib" ¶
        "{Libraries}IntEnv.o" ¶
        "{Libraries}ToolLibs.o" ¶
        {Objects¥68k}

# add 68k low-level code resource (for use from PPC part)
doAddCR = ¶
    Link ¶
        -rt O9LM=200 ¶
        -m OS9_LLM_ENTRY ¶
        -o "{output}{outname}" -d "{Sym¥68k}" ¶
        "{CodeRes¥68k}"

# Add Resources

doAddRes = ¶
    Rez {os9mpw}os9.r -o "{output}{outname}" {Includes} -append ; ¶
    Rez {os9exec}os9exec_nt.r -o "{output}{outname}" {Includes} -append


# PPC Tool

"{output}os9ppc" ÄÄ toolnamePPC

"{output}os9ppc" ÄÄ {¥MondoBuild¥} {Objects¥PPC} {CodeRes¥68k}
    {doLinkPPC}

"{output}os9ppc" ÄÄ {¥MondoBuild¥} "{os9mpw}os9.r" "{os9exec}os9exec_nt.r"
    {doAddRes}

"{output}os9ppc" ÄÄ {¥MondoBuild¥} {CodeRes¥68k}
    {doAddCR}


# 68k Tool

"{output}os968k" ÄÄ toolname68k

"{output}os968k" ÄÄ {¥MondoBuild¥} {Objects¥68K}
    {doLink68k}

"{output}os968k" ÄÄ {¥MondoBuild¥} "{os9mpw}os9.r" "{os9exec}os9exec_nt.r"
    {doAddRes}
    
# Fat Tool

"{output}os9" ÄÄ toolnameFat

"{output}os9" ÄÄ {¥MondoBuild¥} {Objects¥68K} {Objects¥PPC} {CodeRes¥68k}
    {doLinkPPC}
    {doLink68k}

"{output}os9" ÄÄ {¥MondoBuild¥} "{os9mpw}os9.r" "{os9exec}os9exec_nt.r"
    {doAddRes}

"{output}os9" ÄÄ {¥MondoBuild¥} {CodeRes¥68k}
    {doAddCR}


# ASM files

"{ObjDir¥68k}os9_llm_ppc.a.o" Ä {¥MondoBuild¥} "{os9exec}os9_llm_ppc.a"
    {Asm} "{os9exec}os9_llm_ppc.a" -o {Targ} {AOptions}

"{ObjDir¥68k}os9_llm_68k.a.o" Ä {¥MondoBuild¥} "{os9exec}os9_llm_68k.a"
    {Asm} "{os9exec}os9_llm_68k.a" -o {Targ} {AOptions}


# C files

# PPC
"{ObjDir¥PPC}consio.c.x" Ä {¥MondoBuild¥} "{os9exec}consio.c"
    {PPCC} "{os9exec}consio.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}debug.c.x" Ä {¥MondoBuild¥} "{os9exec}debug.c"
    {PPCC} "{os9exec}debug.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}fcalls.c.x" Ä {¥MondoBuild¥} "{os9exec}fcalls.c"
    {PPCC} "{os9exec}fcalls.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}fileaccess.c.x" Ä {¥MondoBuild¥} "{os9exec}fileaccess.c"
    {PPCC} "{os9exec}fileaccess.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}filescsi.c.x" Ä {¥MondoBuild¥} "{os9exec}filescsi.c"
    {PPCC} "{os9exec}filescsi.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}file_rbf.c.x" Ä {¥MondoBuild¥} "{os9exec}file_rbf.c"
    {PPCC} "{os9exec}file_rbf.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}macfiles.c.x" Ä {¥MondoBuild¥} "{platforms}MAC:macfiles.c"
    {PPCC} "{platforms}MAC:macfiles.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}filestuff.c.x" Ä {¥MondoBuild¥} "{os9exec}filestuff.c"
    {PPCC} "{os9exec}filestuff.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}icalls.c.x" Ä {¥MondoBuild¥} "{os9exec}icalls.c"
    {PPCC} "{os9exec}icalls.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}intcommand.c.x" Ä {¥MondoBuild¥} "{os9exec}intcommand.c"
    {PPCC} "{os9exec}intcommand.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}filters.c.x" Ä {¥MondoBuild¥} "{os9exec}filters.c"
    {PPCC} "{os9exec}filters.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}int_dir.c.x" Ä {¥MondoBuild¥} "{os9exec}int_dir.c"
    {PPCC} "{os9exec}int_dir.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}int_move.c.x" Ä {¥MondoBuild¥} "{os9exec}int_move.c"
    {PPCC} "{os9exec}int_move.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}int_rename.c.x" Ä {¥MondoBuild¥} "{os9exec}int_rename.c"
    {PPCC} "{os9exec}int_rename.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}memstuff.c.x" Ä {¥MondoBuild¥} "{os9exec}memstuff.c"
    {PPCC} "{os9exec}memstuff.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}network.c.x" Ä {¥MondoBuild¥} "{os9exec}network.c"
    {PPCC} "{os9exec}network.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}net_mac.c.x" Ä {¥MondoBuild¥} "{platforms}MAC:net_mac.c"
    {PPCC} "{platforms}MAC:net_mac.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}alarms.c.x" Ä {¥MondoBuild¥} "{os9exec}alarms.c"
    {PPCC} "{os9exec}alarms.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}events.c.x" Ä {¥MondoBuild¥} "{os9exec}events.c"
    {PPCC} "{os9exec}events.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}modstuff.c.x" Ä {¥MondoBuild¥} "{os9exec}modstuff.c"
    {PPCC} "{os9exec}modstuff.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}os9exec_nt.c.x" Ä {¥MondoBuild¥} "{os9exec}os9exec_nt.c"
    {PPCC} "{os9exec}os9exec_nt.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}os9main.c.x" Ä {¥MondoBuild¥} "{os9exec}os9main.c"
    {PPCC} "{os9exec}os9main.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}os9_llm.c.x" Ä {¥MondoBuild¥} "{os9exec}os9_llm.c"
    {PPCC} "{os9exec}os9_llm.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}funcdispatch.c.x" Ä {¥MondoBuild¥} "{os9exec}funcdispatch.c"
    {PPCC} "{os9exec}funcdispatch.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}pipefiles.c.x" Ä {¥MondoBuild¥} "{os9exec}pipefiles.c"
    {PPCC} "{os9exec}pipefiles.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}procstuff.c.x" Ä {¥MondoBuild¥} "{os9exec}procstuff.c"
    {PPCC} "{os9exec}procstuff.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}utilstuff.c.x" Ä {¥MondoBuild¥} "{os9exec}utilstuff.c"
    {PPCC} "{os9exec}utilstuff.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}vmod.c.x" Ä {¥MondoBuild¥} "{os9exec}vmod.c"
    {PPCC} "{os9exec}vmod.c" -o {Targ} {PPCCOptions}

"{ObjDir¥PPC}telnetaccess.c.x" Ä {¥MondoBuild¥} "{os9exec}telnetaccess.c"
    {PPCC} "{os9exec}telnetaccess.c" -o {Targ} {PPCCOptions}


# OS9 MPW specifics PPC
"{ObjDir¥PPC}os9.c.x" Ä {¥MondoBuild¥} "{os9mpw}os9.c"
    {PPCC} "{os9mpw}os9.c" -o {Targ} {PPCCOptions}


# 68k
"{ObjDir¥68K}consio.c.o" Ä {¥MondoBuild¥} "{os9exec}consio.c"
    {C} "{os9exec}consio.c" -o {Targ} {COptions}

"{ObjDir¥68K}debug.c.o" Ä {¥MondoBuild¥} "{os9exec}debug.c"
    {C} "{os9exec}debug.c" -o {Targ} {COptions}

"{ObjDir¥68K}fcalls.c.o" Ä {¥MondoBuild¥} "{os9exec}fcalls.c"
    {C} "{os9exec}fcalls.c" -o {Targ} {COptions}

"{ObjDir¥68K}fileaccess.c.o" Ä {¥MondoBuild¥} "{os9exec}fileaccess.c"
    {C} "{os9exec}fileaccess.c" -o {Targ} {COptions}

"{ObjDir¥68K}filescsi.c.o" Ä {¥MondoBuild¥} "{os9exec}filescsi.c"
    {C} "{os9exec}filescsi.c" -o {Targ} {COptions}

"{ObjDir¥68K}file_rbf.c.o" Ä {¥MondoBuild¥} "{os9exec}file_rbf.c"
    {C} "{os9exec}file_rbf.c" -o {Targ} {COptions}

"{ObjDir¥68K}macfiles.c.o" Ä {¥MondoBuild¥} "{platforms}MAC:macfiles.c"
    {C} "{platforms}MAC:macfiles.c" -o {Targ} {COptions}

"{ObjDir¥68K}filestuff.c.o" Ä {¥MondoBuild¥} "{os9exec}filestuff.c"
    {C} "{os9exec}filestuff.c" -o {Targ} {COptions}

"{ObjDir¥68K}icalls.c.o" Ä {¥MondoBuild¥} "{os9exec}icalls.c"
    {C} "{os9exec}icalls.c" -o {Targ} {COptions}

"{ObjDir¥68K}intcommand.c.o" Ä {¥MondoBuild¥} "{os9exec}intcommand.c"
    {C} "{os9exec}intcommand.c" -o {Targ} {COptions}

"{ObjDir¥68K}filters.c.o" Ä {¥MondoBuild¥} "{os9exec}filters.c"
    {C} "{os9exec}filters.c" -o {Targ} {COptions}

"{ObjDir¥68K}int_dir.c.o" Ä {¥MondoBuild¥} "{os9exec}int_dir.c"
    {C} "{os9exec}int_dir.c" -o {Targ} {COptions}

"{ObjDir¥68K}int_move.c.o" Ä {¥MondoBuild¥} "{os9exec}int_move.c"
    {C} "{os9exec}int_move.c" -o {Targ} {COptions}

"{ObjDir¥68k}int_rename.c.o" Ä {¥MondoBuild¥} "{os9exec}int_rename.c"
    {C} "{os9exec}int_rename.c" -o {Targ} {COptions}

"{ObjDir¥68K}memstuff.c.o" Ä {¥MondoBuild¥} "{os9exec}memstuff.c"
    {C} "{os9exec}memstuff.c" -o {Targ} {COptions}

#Ê"{ObjDir¥68K}network.c.o" Ä {¥MondoBuild¥} "{os9exec}network.c"
#     {C} "{os9exec}network.c" -o {Targ} {COptions}

#Ê"{ObjDir¥68K}net_mac.c.o" Ä {¥MondoBuild¥} "{platforms}MAC:net_mac.c"
#     {C} "{platforms}MAC:net_mac.c" -o {Targ} {COptions}

"{ObjDir¥68K}alarms.c.o" Ä {¥MondoBuild¥} "{os9exec}alarms.c"
    {C} "{os9exec}alarms.c" -o {Targ} {COptions}

"{ObjDir¥68K}events.c.o" Ä {¥MondoBuild¥} "{os9exec}events.c"
    {C} "{os9exec}events.c" -o {Targ} {COptions}

"{ObjDir¥68K}modstuff.c.o" Ä {¥MondoBuild¥} "{os9exec}modstuff.c"
    {C} "{os9exec}modstuff.c" -o {Targ} {COptions}

"{ObjDir¥68K}os9exec_nt.c.o" Ä {¥MondoBuild¥} "{os9exec}os9exec_nt.c"
    {C} "{os9exec}os9exec_nt.c" -o {Targ} {COptions}

"{ObjDir¥68K}os9main.c.o" Ä {¥MondoBuild¥} "{os9exec}os9main.c"
    {C} "{os9exec}os9main.c" -o {Targ} {COptions}

"{ObjDir¥68K}os9_llm.c.o" Ä {¥MondoBuild¥} "{os9exec}os9_llm.c"
    {C} "{os9exec}os9_llm.c" -o {Targ} {COptions}

"{ObjDir¥68K}funcdispatch.c.o" Ä {¥MondoBuild¥} "{os9exec}funcdispatch.c"
    {C} "{os9exec}funcdispatch.c" -o {Targ} {COptions}

"{ObjDir¥68K}pipefiles.c.o" Ä {¥MondoBuild¥} "{os9exec}pipefiles.c"
    {C} "{os9exec}pipefiles.c" -o {Targ} {COptions}

"{ObjDir¥68K}procstuff.c.o" Ä {¥MondoBuild¥} "{os9exec}procstuff.c"
    {C} "{os9exec}procstuff.c" -o {Targ} {COptions}

"{ObjDir¥68K}utilstuff.c.o" Ä {¥MondoBuild¥} "{os9exec}utilstuff.c"
    {C} "{os9exec}utilstuff.c" -o {Targ} {COptions}

"{ObjDir¥68K}vmod.c.o" Ä {¥MondoBuild¥} "{os9exec}vmod.c"
    {C} "{os9exec}vmod.c" -o {Targ} {COptions}

"{ObjDir¥68K}telnetaccess.c.o" Ä {¥MondoBuild¥} "{os9exec}telnetaccess.c"
    {C} "{os9exec}telnetaccess.c" -o {Targ} {COptions}


# OS9 MPW specifics 68k
"{ObjDir¥68K}os9.c.o" Ä {¥MondoBuild¥} "{os9mpw}os9.c"
    {C} "{os9mpw}os9.c" -o {Targ} {COptions}

