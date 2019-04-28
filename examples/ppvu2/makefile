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
# Copyright (c) 2000, 2001 Criterion Software Ltd.
# All Rights Reserved.
#
# RenderWare is a trademark of Canon Inc.
#
###########################################################################

# Define the projects to be built

DEMO = ppvu2

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

CSRC =  $(SRC)/main.c                           \
        $(SRC)/ppvu2.c                          \
        $(SRC)/ppsprite.c                       \
        $(SRC)/$(RWOS)/events.c                 \
        $(COMMON)/menu.c                        \
        $(COMMON)/camera.c

DSMS =                                          \
        $(SRC)/ppspriteNT.dsm                   \
        $(SRC)/ppspritePRSDev.dsm               \
        $(SRC)/ppspritePRSOpt.dsm               \

OBJ +=  $(patsubst %.dsm,$(OBJDIR)/%.$(O),$(notdir $(DSMS)))

DVPASFLAGS += -I$(SRC)/

$(OBJDIR)/%.$(O): $(SRC)/%.dsm
	$(DVPASM) $(DVPASFLAGS) \
	-alhs $(DVPAOUT)$@ $^ > $(OBJDIR)/$*.lsd


# Add skeleton files

CSRC += $(SKEL)/skeleton.c	\
	$(SKELPS)/$(RWOS).c     \
	$(SKELSRC)

ifeq ($(RWMETRICS), 1)
CSRC += $(SKEL)/vecfont.c   \
        $(SKEL)/metrics.c
endif

ifeq ("$(RWTARGET)", "sky2")
RWLIBS += $(RWLIBDIR)/$(LP)rppds.$(L)
endif # ifeq ("$(RWTARGET)", "sky2")

RWLIBS += $(RWLIBDIR)/$(LP)rtcharse.$(L) \
        $(RWLIBDIR)/$(LP)rtbmp.$(L)      \
        $(RWLIBDIR)/$(LP)rtpng.$(L)      \
        $(RWLIBDIR)/$(LP)rpworld.$(L)    \
	$(RWLIBDIR)/$(LP)rwcore.$(L)

include $(RWGDIR)/shared/maketarg

CFLAGS +=                                       \
        -I$(SRC)/skycfx



