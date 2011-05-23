# Microsoft Developer Studio Project File - Name="openjpeg2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl/config.win32" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# openjpeg2  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=openjpeg2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "openjpeg2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "openjpeg2.mak" CFG="openjpeg2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "openjpeg2 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "openjpeg2 - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "openjpeg2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "openjpeg2_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "openjpeg2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /GZ /c
# ADD CPP /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "openjpeg2_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ENDIF 

# Begin Target

# Name "openjpeg2 - Win32 Release"
# Name "openjpeg2 - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "c"
# Begin Source File
SOURCE=.\pi.c
# End Source File
# Begin Source File
SOURCE=.\event.c
# End Source File
# Begin Source File
SOURCE=.\raw.c
# End Source File
# Begin Source File
SOURCE=.\image.c
# End Source File
# Begin Source File
SOURCE=.\mct.c
# End Source File
# Begin Source File
SOURCE=.\mqc.c
# End Source File
# Begin Source File
SOURCE=.\t1.c
# End Source File
# Begin Source File
SOURCE=.\bio.c
# End Source File
# Begin Source File
SOURCE=.\function_list.c
# End Source File
# Begin Source File
SOURCE=.\j2k_lib.c
# End Source File
# Begin Source File
SOURCE=.\tcd.c
# End Source File
# Begin Source File
SOURCE=.\profile.c
# End Source File
# Begin Source File
SOURCE=.\j2k.c
# End Source File
# Begin Source File
SOURCE=.\tgt.c
# End Source File
# Begin Source File
SOURCE=.\cio.c
# End Source File
# Begin Source File
SOURCE=.\invert.c
# End Source File
# Begin Source File
SOURCE=.\jp2.c
# End Source File
# Begin Source File
SOURCE=.\t2.c
# End Source File
# Begin Source File
SOURCE=.\jpt.c
# End Source File
# Begin Source File
SOURCE=.\dwt.c
# End Source File
# Begin Source File
SOURCE=.\openjpeg.c
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h"
# Begin Source File
SOURCE=.\raw.h
# End Source File
# Begin Source File
SOURCE=.\invert.h
# End Source File
# Begin Source File
SOURCE=.\function_list.h
# End Source File
# Begin Source File
SOURCE=.\openjpeg.h
# End Source File
# Begin Source File
SOURCE=.\int.h
# End Source File
# Begin Source File
SOURCE=.\t1_luts.h
# End Source File
# Begin Source File
SOURCE=.\jp2.h
# End Source File
# Begin Source File
SOURCE=.\dwt.h
# End Source File
# Begin Source File
SOURCE=.\j2k.h
# End Source File
# Begin Source File
SOURCE=.\opj_configure.h
# End Source File
# Begin Source File
SOURCE=.\j2k_lib.h
# End Source File
# Begin Source File
SOURCE=.\bio.h
# End Source File
# Begin Source File
SOURCE=.\event.h
# End Source File
# Begin Source File
SOURCE=.\mct.h
# End Source File
# Begin Source File
SOURCE=.\tgt.h
# End Source File
# Begin Source File
SOURCE=.\mqc.h
# End Source File
# Begin Source File
SOURCE=.\profile.h
# End Source File
# Begin Source File
SOURCE=.\t1.h
# End Source File
# Begin Source File
SOURCE=.\jpt.h
# End Source File
# Begin Source File
SOURCE=.\pi.h
# End Source File
# Begin Source File
SOURCE=.\fix.h
# End Source File
# Begin Source File
SOURCE=.\tcd.h
# End Source File
# Begin Source File
SOURCE=.\opj_includes.h
# End Source File
# Begin Source File
SOURCE=.\image.h
# End Source File
# Begin Source File
SOURCE=.\opj_malloc.h
# End Source File
# Begin Source File
SOURCE=.\cio.h
# End Source File
# Begin Source File
SOURCE=.\t2.h
# End Source File
# End Group
# End Target
# End Project
