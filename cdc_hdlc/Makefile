######################################################################
#  Project Makefile
######################################################################

BINARY		= cdc_hdlc
SRCFILES	= main.c \
		usbcdc.c rtos/heap_4.c \
		rtos/list.c rtos/port.c \
		rtos/queue.c \
		rtos/tasks.c \
		rtos/opencm3.c


#all: elf bin
#LDSCRIPT	= stm32f103c8t6.ld makefile.incl defines

# DEPS		= 	# Any additional dependencies for your build
# CLOBBER	+= 	# Any additional files to be removed with "make clobber"

include ../Makefile.incl
#include ../Makefile.rtos
TGT_CFLAGS	+= -I./rtos -I.
TGT_CXXFLAGS	+= -I./rtos -I.

######################################################################
