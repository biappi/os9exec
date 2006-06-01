# Makefile generated by Metrowerks CodeWarrior IDE
#  - manually adapted for 32 bit compilation (bfo 06/03/07)
#

all: OS9Linux

clean: clean_OS9Linux

##############################
#######  TARGET: OS9Linux 
##############################
TOP_OS9Linux=..
WD_OS9Linux=$(shell cd ${TOP_OS9Linux};echo `pwd`)
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/consio.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/debug.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/fcalls.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/fileaccess.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/filestuff.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/file_rbf.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/filters.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/funcdispatch.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/icalls.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/intcommand.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/int_dir.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/int_move.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/int_rename.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/memstuff.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/modstuff.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/os9exec_nt.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/os9main.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/pipefiles.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/procstuff.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/utilstuff.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9AppEmu/UAE68emulator/cpudefs.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9AppEmu/UAE68emulator/cpustbl.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9AppEmu/UAE68emulator/newcpu.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9AppEmu/os9_uae.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9AppEmu/UAE68emulator/readcpu.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9AppEmu/UAE68emulator/support.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9AppEmu/luzstuff.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9AppEmu/UAE68emulator/cpuemu.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9AppEmu/UAE68emulator/fpp.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/events.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/network.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/telnetaccess.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/vmod.c
c_SRC_OS9Linux+=${WD_OS9Linux}/Platforms/LINUX/linuxfiles.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/filescsi.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/alarms.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9exec_core/printer.c
c_SRC_OS9Linux+=${WD_OS9Linux}/OS9App/os9app.c
c_SRC_OS9Linux+=${WD_OS9Linux}/Platforms/LINUX/net_linux.c

OBJS_OS9Linux+=$(c_SRC_OS9Linux:.c=.c.o)

CFLAGS_OS9Linux+=\
 -iquote TARGET_STD/\
 -iquote ../Platforms/LINUX/\
 -iquote ../Platforms/\
 -iquote ./\
 -iquote UAE68emulator/\
 -iquote ../OS9App/\
 -iquote ../OS9exec_core/os9defs/\
 -iquote ../OS9exec_core/\
 -I /usr/include/

DBG=-g

CFLAGS_OS9Linux+=-O3

clean_OS9Linux:
	rm -f ${OBJS_OS9Linux}

OS9Linux:
	$(MAKE) -f os9Linux.mk os9linux TARGET=os9linux\
 CC="gcc"  LD="gcc"  AR="ar -crs"  SIZE="size" LIBS+="/usr/lib/libm.a"

os9linux: ${OBJS_OS9Linux} 
	${CC} ${LDFLAGS} -m32 -o ../../Output/os9linux ${OBJS_OS9Linux} ${LIBS}


ifeq (${TARGET}, os9linux)

%.c.o: %.c
	gcc -c ${CFLAGS_OS9Linux} -m32 $< -o $@ -MMD
include $(c_SRC_OS9Linux:.c=.d)

%.d: %.c
	set -e; $(CC) -M $(CFLAGS_OS9Linux) $< \
 | sed 's!\($(*F)\)\.o[ :]*!$(*D)/\1.o $@ : !g' > $@; \
 [ -s $@ ] || rm -f $@ 



endif
##############################

# end Makefile
