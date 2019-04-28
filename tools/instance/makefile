###########################################################################
#
# File     : Makefile
#
# Abstract : Command line makefile used to build a RenderWare tool program.
#            This is a GNU Make makefile. A copy of GNU Make is required in
#	     order to use this file.
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
# Copyright (c) 2001 Criterion Software Ltd.
# All Rights Reserved.
#
# RenderWare is a trademark of Canon Inc.
#
###########################################################################

ifndef RWGDIR
RWGDIR = $(CURDIR)/../..
endif 	# RWGDIR

include $(RWGDIR)/shared/makeopt

ifeq 	($(CDEBUG), 0)
ifeq ($(RWCOMPILER), net)
BUILDRELEASE = Release
else
BUILDRELEASE = - Win32 Release
endif
else 	# ($(CDEBUG), 0)
ifeq ($(RWCOMPILER), net)
BUILDRELEASE = Debug
else
BUILDRELEASE = - Win32 Debug
endif
endif 	# ($(CDEBUG), 0)

demo:
ifeq ($(RWTARGET), null)
ifeq ($(RWCOMPILER), net)
	devenv RwInstance.vcproj /rebuild "$(BUILDRELEASE)"
	devenv instancetool_$(RWTARGET).vcproj /rebuild "$(BUILDRELEASE)"	
else
	msdev RwInstance.dsp /MAKE "RwInstance $(BUILDRELEASE)" /REBUILD
	msdev instancetool_$(RWTARGET).dsp /MAKE "instancetool_$(RWTARGET) $(BUILDRELEASE)" /REBUILD
endif
endif

ifeq ($(RWTARGET), nullgcn)
ifeq ($(RWCOMPILER), net)
	devenv RwInstance_gcn.vcproj /rebuild "$(BUILDRELEASE)"
else
	msdev RwInstance_gcn.dsp /MAKE "RwInstance_gcn $(BUILDRELEASE)" /REBUILD
endif
endif

ifeq ($(RWTARGET), nullsky)
ifeq ($(RWCOMPILER), net)
	devenv RwInstance_sky.vcproj /rebuild "$(BUILDRELEASE)"
else
	msdev RwInstance_sky.dsp /MAKE "RwInstance_sky $(BUILDRELEASE)" /REBUILD
endif
endif

ifeq ($(RWTARGET), nullxbox)
ifeq ($(RWCOMPILER), net)
	devenv RwInstance_xbox.vcproj /rebuild "$(BUILDRELEASE)"
else
	msdev RwInstance_xbox.dsp /MAKE "RwInstance_xbox $(BUILDRELEASE)" /REBUILD
endif
endif

distclean clean:
ifeq ($(RWTARGET), null)
ifeq ($(RWCOMPILER), net)
	devenv RwInstance.vcproj /clean "Debug"
	devenv RwInstance.vcproj /clean "Release"
	devenv instancetool_$(RWTARGET).vcproj /clean "Debug"
	devenv instancetool_$(RWTARGET).vcproj /clean "Release"
else
	msdev instancetool_$(RWTARGET).dsp /MAKE "instancetool_$(RWTARGET) - All" /CLEAN
	msdev RwInstance.dsp /MAKE "RwInstance - All" /CLEAN
endif
endif

ifeq ($(RWTARGET), nullgcn)
ifeq ($(RWCOMPILER), net)
	devenv RwInstance_gcn.vcproj /clean "Debug"
	devenv RwInstance_gcn.vcproj /clean "Release"
else
	msdev RwInstance_gcn.dsp /MAKE "RwInstance_gcn - All" /CLEAN
endif
endif

ifeq ($(RWTARGET), nullsky)
ifeq ($(RWCOMPILER), net)
	devenv RwInstance_sky.vcproj /clean "Debug"
	devenv RwInstance_sky.vcproj /clean "Release"
else
	msdev RwInstance_sky.dsp /MAKE "RwInstance_sky - All" /CLEAN
endif
endif

ifeq ($(RWTARGET), nullxbox)
ifeq ($(RWCOMPILER), net)
	devenv RwInstance_xbox.vcproj /clean "Debug"
	devenv RwInstance_xbox.vcproj /clean "Release"
else
	msdev RwInstance_xbox.dsp /MAKE "RwInstance_xbox - All" /CLEAN
endif
endif
