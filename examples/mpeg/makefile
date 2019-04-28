###########################################################################
#
# File     : Makefile
#
# Abstract : Command line makefile used to build a RenderWare demo program.
#            This is a GNU Make makefile. A copy of GNU Make is required in
#            order to use this file.
#
###########################################################################
#
# This file is a product of Criterion Software Ltd.
#
# This file is provided as is with no warranties of any kind and is
# provided without any obligation on Criterion Software Ltd. or Canon Inc.
# to assist in its use or modification.
#
# Criterion Software Ltd. will not, under any circumstances, be liable for
# any lost revenue or other damages arising from the use of this file.
#
# Copyright (c) 2000 Criterion Software Ltd.
# All Rights Reserved.
#
# RenderWare is a trademark of Canon Inc.
#
###########################################################################

# Define the projects to be built

DEMO = mpeg

# include common makefile options

ifndef RWGDIR
RWGDIR = $(CURDIR)/../..
endif # RWGDIR

include $(RWGDIR)/shared/makeopt

unsupported:
	@echo =====================================
	@echo $(DEMO) is unsupported on $(RWTARGET)
	@echo =====================================

ifneq ("$(RWTARGET)", "sky2")
override TARGET := unsupported
override VALIDATE := unsupported
override OBJ := 
endif # ifneq ("$(RWTARGET)", "sky2")

ifeq ("$(RWOS)", "sky")
CFLAGS += -I./src/sky
endif # ifeq ("$(RWOS)", "sky")

# $(SRC)/geometry.c	\

CSRC =	$(SRC)/main.c				\
	$(SRC)/$(RWOS)/audiodec.c		\
	$(SRC)/$(RWOS)/disp.c			\
	$(SRC)/$(RWOS)/events.c			\
	$(SRC)/$(RWOS)/mpeg.c			\
	$(SRC)/$(RWOS)/read.c			\
	$(SRC)/$(RWOS)/readbuf.c		\
	$(SRC)/$(RWOS)/strfile.c		\
	$(SRC)/$(RWOS)/util.c			\
	$(SRC)/$(RWOS)/vibuf.c			\
	$(SRC)/$(RWOS)/videodec.c		\
	$(SRC)/$(RWOS)/vobuf.c			\
	$(COMMON)/camera.c			\
	$(COMMON)/menu.c

# Add skeleton files

CSRC +=						\
	$(SKEL)/skeleton.c			\
	$(SKELPS)/$(RWOS).c			\
	$(SKELSRC)

ifeq ($(RWMETRICS), 1)
CSRC +=						\
	$(SKEL)/vecfont.c			\
        $(SKEL)/metrics.c
endif

ifeq ("$(RWTARGET)", "sky2")
RWLIBS += $(RWLIBDIR)/$(LP)rppds.$(L)
endif # ifeq ("$(RWTARGET)", "sky2")

RWLIBS +=					\
	$(RWLIBDIR)/$(LP)rtcharse.$(L)		\
	$(RWLIBDIR)/$(LP)rtpng.$(L)		\
	$(RWLIBDIR)/$(LP)rtbmp.$(L)		\
	$(RWLIBDIR)/$(LP)rpworld.$(L)		\
	$(RWLIBDIR)/$(LP)rwcore.$(L)

include $(RWGDIR)/shared/maketarg

SYSLIBS +=					\
	$(SCELIBDIR)/libmpeg.a			\
	$(SCELIBDIR)/libcdvd.a			\
	$(SCELIBDIR)/libdma.a			\
	$(SCELIBDIR)/libipu.a			\
	$(SCELIBDIR)/libpkt.a			\
	$(SCELIBDIR)/libsdr.a
