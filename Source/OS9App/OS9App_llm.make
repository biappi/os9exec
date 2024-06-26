# Make OS9-lowlevel files for use in CW OS9App project 

ObjDir�PPC   =  :::Output:OBJPPC:
ObjDir�68K   =  :::Output:OBJ68k:
Output       =  :::Output:Precompiled:
os9exec      =  ::OS9exec_core:


# We need the os9_llm assembler 
os9app_llm � {Output}os9llm_CodeRes.r {Output}os9llm_Library.lib
    

# Code Resource for use in PPC App
{Output}os9llm_CodeRes.r � {Output}os9llm_CodeRes.rsrc
    Derez {Output}os9llm_CodeRes.rsrc -only O9LM > {Targ}

{Output}os9llm_CodeRes.rsrc � "{ObjDir�68k}os9_llm_ppc.a.o"
    Link �
        -rt O9LM=200 �
        -m OS9_LLM_ENTRY �
        -o {Targ} �
        "{ObjDir�68k}os9_llm_ppc.a.o"


# Library for use in 68k App
{Output}os9llm_Library.lib � "{ObjDir�68K}os9_llm_68k.a.o"
    MWLink68k �
        -o {Targ} �
        -xm library �
        -model far �
        "{ObjDir�68K}os9_llm_68k.a.o"


# ASM files
"{ObjDir�68k}os9_llm_ppc.a.o" � {os9exec}os9_llm_ppc.a
    Asm {os9exec}os9_llm_ppc.a -o {Targ} {AOptions}

"{ObjDir�68k}os9_llm_68k.a.o" � {os9exec}os9_llm_68k.a
    Asm {os9exec}os9_llm_68k.a -o {Targ} {AOptions}
