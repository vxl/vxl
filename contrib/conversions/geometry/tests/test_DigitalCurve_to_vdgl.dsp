# Microsoft Developer Studio Project File - Name="test_DigitalCurve_to_vdgl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# CM DSP Header file
# This file is read by the build system of cm, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/conversions" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/gel" /I "$(VXLROOT)/SpatialObjects" /I "$(VXLROOT)/GeneralUtility" /I "$(VXLROOT)/Numerics" /I "$(VXLROOT)/COOL" == include path
# $(VXLROOT)/bin/ == override in output directory
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
# test_DigitalCurve_to_vdgl  == name of output library
#  /libpath:"$(VXLROOT)/vxl/vnl/$(OUTDIR)" /libpath:"$(VXLROOT)/vxl/vnl/$(OUTDIR)" /libpath:"$(VXLROOT)/vcl/$(OUTDIR)" /libpath:"$(VXLROOT)/vcl/$(OUTDIR)"  "vnl.lib" "vcl.lib" /STACK:10000000  == libraries linked in 
# TARGTYPE "Win32 (x86) Application" 0x0103

CFG=test_DigitalCurve_to_vdgl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "test_DigitalCurve_to_vdgl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "test_DigitalCurve_to_vdgl.mak" CFG="test_DigitalCurve_to_vdgl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "test_DigitalCurve_to_vdgl - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "test_DigitalCurve_to_vdgl - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "test_DigitalCurve_to_vdgl - Win32 Release"

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
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32"  /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/conversions" /I "$(VXLROOT)/gel" /I "$(VXLROOT)/SpatialObjects" /I "$(VXLROOT)/GeneralUtility" /I "$(VXLROOT)/Numerics" /I "$(VXLROOT)/COOL" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "test_DigitalCurve_to_vdgl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089
# ADD LINK32 /libpath:"$(VXLROOT)/SpatialObjects/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/COOL/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/Numerics/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/GeneralUtility/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/vxl/vnl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/vcl/$(OUTDIR)"
# ADD LINK32 "vnl.lib" "vcl.lib" /STACK:10000000 

!ELSEIF  "$(CFG)" == "test_DigitalCurve_to_vdgl - Win32 Debug"

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
# ADD BASE CPP   /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /GZ /c
# ADD CPP /nologo  /D "WIN32"  /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /GZ /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/conversions" /I "$(VXLROOT)/gel" /I "$(VXLROOT)/SpatialObjects" /I "$(VXLROOT)/GeneralUtility" /I "$(VXLROOT)/Numerics" /I "$(VXLROOT)/COOL" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "test_DigitalCurve_to_vdgl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /IGNORE:4089
# ADD LINK32 /libpath:"$(VXLROOT)/SpatialObjects/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/COOL/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/Numerics/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/GeneralUtility/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/vxl/vnl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/vcl/$(OUTDIR)"
# ADD LINK32 "vnl.lib" "vcl.lib" /STACK:10000000 

!ENDIF 

# Begin Target

# Name "test_DigitalCurve_to_vdgl - Win32 Release"
# Name "test_DigitalCurve_to_vdgl - Win32 Debug"

# Begin Group "Source Files"
# Begin Source File

SOURCE=.\test_driver.cxx

# End Source File
# Begin Source File

SOURCE=.\test_DigitalCurve_to_vdgl.cxx

# End Source File
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

