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

DEMO = vclgenp

# Define the pipelines to build

VCLPIPES = 		\
#	<pipe>		\

VCLPROGS = 			\
#	<pipe>/<pipe>PER	\

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

CSRC = $(SRC)/main.c				\
	$(SRC)/$(RWOS)/events.c			\
	$(COMMON)/camera.c			\
	$(COMMON)/menu.c

# Add skeleton files

CSRC += $(SKEL)/skeleton.c			\
	$(SKELPS)/$(RWOS).c     		\
	$(SKELSRC)

ifeq ($(RWMETRICS), 1)
CSRC += $(SKEL)/vecfont.c   			\
        $(SKEL)/metrics.c
endif

RWLIBS += $(RWLIBDIR)/$(LP)rtcharse.$(L)    \
	$(RWLIBDIR)/$(LP)rtbmp.$(L)	            \
	$(RWLIBDIR)/$(LP)rtpng.$(L)	            \
 	$(RWLIBDIR)/$(LP)rppds.$(L)	            \
	$(RWLIBDIR)/$(LP)rpmatfx.$(L)   	    \
	$(RWLIBDIR)/$(LP)rpskin.$(L)    	    \
	$(RWLIBDIR)/$(LP)rphanim.$(L)   	    \
	$(RWLIBDIR)/$(LP)rpmorph.$(L)   	    \
	$(RWLIBDIR)/$(LP)rprandom.$(L)          \
	$(RWLIBDIR)/$(LP)rtquat.$(L)    	    \
	$(RWLIBDIR)/$(LP)rtanim.$(L)    	    \
	$(RWLIBDIR)/$(LP)rpworld.$(L)   	    \
	$(RWLIBDIR)/$(LP)rwcore.$(L)

VCL = vcl
VCLFLAGS = -G
DVPASFLAGS += -I$(SRC)/
PDS = $(RWGDIR)/rwsdk/plugin/pds

VPATH += $(addprefix $(SRC)/, $(VCLPIPES))
VCLPIPES_C = $(addprefix $(SRC)/, $(addsuffix _Node.c, $(join $(addsuffix /, $(VCLPIPES)), $(VCLPIPES))))
VCLPROGS_VSM = $(addsuffix .vsm, $(addprefix $(SRC)/, $(VCLPROGS)))
VCLPROGS_OBJ = $(addsuffix .$(O), $(addprefix $(OBJDIR)/, $(notdir $(VCLPROGS))))
VCLPIPES_OBJ = $(addsuffix _Node.$(O), $(addprefix $(OBJDIR)/, $(VCLPIPES)))

CFLAGS += -I$(PDS)/sky2/ $(addprefix -I$(SRC)/, $(VCLPIPES))
CSRC += $(VCLPIPES_NODEC)
OBJ += $(VCLPROGS_OBJ) $(VCLPIPES_OBJ)

$(VCLPROGS_OBJ) : $(OBJDIR)/%.$(O) : %.dsm %.vsm
	$(DVPASM) $(DVPASMFLAGS) -alhs $(DVPAOUT)$@ $< > $(OBJDIR)/$*.lsd
$(VCLPROGS_VSM) : %.vsm : %.vcl
	$(VCL) $(VCLFLAGS) -o$@ $< > $(OBJDIR)/$(notdir $*).lsv
$(VCLPIPES_OBJ) : $(OBJDIR)/%.$(O) : %.c
	$(CC) $(CFLAGS) $(CODEGENCFLAGS) $(COUT)$@ $<

include $(RWGDIR)/shared/maketarg

