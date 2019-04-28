# Microsoft Developer Studio Project File - Name="world" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=world - Win32 PS2 EE Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "world_sky2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "world_sky2.mak" CFG="world - Win32 PS2 EE Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "world - Win32 PS2 EE Release" (based on "Win32 (x86) Application")
!MESSAGE "world - Win32 PS2 EE Debug" (based on "Win32 (x86) Application")
!MESSAGE "world - Win32 PS2 EE Metrics" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "world_sky2"
# PROP Scc_LocalPath "..\.."
CPP=snCl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "world - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "obj/sky2"
# PROP BASE Intermediate_Dir "obj/sky2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj/sky2"
# PROP Intermediate_Dir "obj/sky2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "../../rwsdk/include/sky2" /I "../src" /I "../src/sky" /I "../../shared/democom" /I "../../shared/skel" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "RWLOGO" /YX /FD /c
# ADD CPP /nologo /W3 /O2 /I "../../rwsdk/include/sky2" /I "./src" /I "./src/sky" /I "../../shared/democom" /I "../../shared/skel" /D "SN_TARGET_PS2" /D "SKY" /D "RW_USE_SPF" /D "RWLOGO" /FD /D IOPPATH=c:/usr/local/sce/iop /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 libgraph.a libdma.a libdev.a libpad.a libpkt.a libvu0.a /nologo /subsystem:windows /machine:IX86
# ADD LINK32 librppds.a librtimport.a librtcharse.a librtbmp.a librtpng.a librpworld.a librwcore.a librplogo.a libgraph.a libdma.a libdev.a libpad.a libpkt.a libvu0.a /nologo /pdb:none /machine:IX86 /out:"./world_sky2.elf" /libpath:"../../rwsdk/lib/sky2/release" /D:SN_TARGET_PS2
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "world - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "obj/sky2d"
# PROP BASE Intermediate_Dir "obj/sky2d"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj/sky2d"
# PROP Intermediate_Dir "obj/sky2d"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../rwsdk/include/sky2" /I "../src" /I "../src/sky" /I "../../shared/democom" /I "../../shared/skel" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "RWLOGO" /D "RWDEBUG" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /ZI /Od /I "../../rwsdk/include/sky2" /I "./src" /I "./src/sky" /I "../../shared/democom" /I "../../shared/skel" /D "SN_TARGET_PS2" /D "SKY" /D "RW_USE_SPF" /D "RWLOGO" /D "RWDEBUG" /FD /GZ /debug /D IOPPATH=c:/usr/local/sce/iop /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 libgraph.a libdma.a libdev.a libpad.a libpkt.a libvu0.a /nologo /subsystem:windows /debug /machine:IX86 /pdbtype:sept
# ADD LINK32 librppds.a librtimport.a librtcharse.a librtbmp.a librtpng.a librpworld.a librwcore.a librplogo.a libgraph.a libdma.a libdev.a libpad.a libpkt.a libvu0.a /nologo /subsystem:windows /pdb:none /debug /machine:IX86 /out:"./world_sky2d.elf" /libpath:"../../rwsdk/lib/sky2/debug" /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "world - Win32 PS2 EE Metrics"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "obj/sky2m"
# PROP BASE Intermediate_Dir "obj/sky2m"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj/sky2m"
# PROP Intermediate_Dir "obj/sky2m"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "../../rwsdk/include/sky2" /I "../src" /I "../src/sky" /I "../../shared/democom" /I "../../shared/skel" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "RWLOGO" /D "RWMETRICS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../rwsdk/include/sky2" /I "./src" /I "./src/sky" /I "../../shared/democom" /I "../../shared/skel" /D "SN_TARGET_PS2" /D "SKY" /D "RW_USE_SPF" /D "RWLOGO" /D "RWMETRICS" /FD /D IOPPATH=c:/usr/local/sce/iop /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 libgraph.a libdma.a libdev.a libpad.a libpkt.a libvu0.a /nologo /subsystem:windows /machine:IX86
# ADD LINK32 librppds.a librtimport.a librtcharse.a librtbmp.a librtpng.a librpworld.a librwcore.a librplogo.a libgraph.a libdma.a libdev.a libpad.a libpkt.a libvu0.a /nologo /pdb:none /machine:IX86 /out:"./world_sky2m.elf" /libpath:"../../rwsdk/lib/sky2/metrics" /D:SN_TARGET_PS2

!ENDIF 

# Begin Target

# Name "world - Win32 PS2 EE Release"
# Name "world - Win32 PS2 EE Debug"
# Name "world - Win32 PS2 EE Metrics"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "demoskel"

# PROP Default_Filter ""
# Begin Group "sky"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../shared/skel\sky\sky.c
# End Source File
# Begin Source File

SOURCE=../../shared/skel\sky\skyfs.c
# End Source File
# End Group
# Begin Source File

SOURCE=../../shared/skel\metrics.c

!IF  "$(CFG)" == "world - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "world - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "world - Win32 PS2 EE Metrics"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=../../shared/skel\skeleton.c
# End Source File
# Begin Source File

SOURCE=../../shared/skel\vecfont.c

!IF  "$(CFG)" == "world - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "world - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "world - Win32 PS2 EE Metrics"

!ENDIF 

# End Source File
# End Group
# Begin Group "democom"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../shared/democom\camera.c
# End Source File
# Begin Source File

SOURCE=../../shared/democom\menu.c
# End Source File
# End Group
# Begin Group "demo"

# PROP Default_Filter ""
# Begin Group "demosky"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\sky\events.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\main.c
# End Source File
# Begin Source File

SOURCE=.\src\world.c
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Group "demoskel Hdrs"

# PROP Default_Filter ""
# Begin Group "sky Hdrs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../shared/skel\sky\skyfs.h
# End Source File
# End Group
# Begin Source File

SOURCE=../../shared/skel\events.h
# End Source File
# Begin Source File

SOURCE=../../shared/skel\metrics.h

!IF  "$(CFG)" == "world - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "world - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "world - Win32 PS2 EE Metrics"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=../../shared/skel\platform.h
# End Source File
# Begin Source File

SOURCE=../../shared/skel\skeleton.h
# End Source File
# Begin Source File

SOURCE=../../shared/skel\vecfont.h

!IF  "$(CFG)" == "world - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "world - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "world - Win32 PS2 EE Metrics"

!ENDIF 

# End Source File
# End Group
# Begin Group "democom Hdrs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../shared/democom\camera.h
# End Source File
# Begin Source File

SOURCE=../../shared/democom\menu.h
# End Source File
# End Group
# Begin Group "demo Hdrs"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=../../shared/sncommon\PS2_in_VC.h
# End Source File
# End Group
# Begin Group "Misc Files"

# PROP Default_Filter ""
# Begin Group "sky misc files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../shared/sncommon\app.cmd
# End Source File
# Begin Source File

SOURCE=../../shared/sncommon\crt0.s
# End Source File
# Begin Source File

SOURCE=.\PS2.lk
# End Source File
# End Group
# End Group
# Begin Group "Readme Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sky.txt
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Target
# End Project
