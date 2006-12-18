# Microsoft Developer Studio Project File - Name="vil1_io" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vil1_io  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vil1_io - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vil1_io.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vil1_io.mak" CFG="vil1_io - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vil1_io - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vil1_io - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vil1_io - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vil1_io_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vil1_io - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vil1_io_EXPORTS"
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

# Name "vil1_io - Win32 Release"
# Name "vil1_io - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\vil1_io_image.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_io_memory_image_of+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_io_memory_image_of+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_io_memory_image_of+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_io_memory_image_of+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_io_memory_image_of+short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_io_memory_image_of+signed_char-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_io_memory_image_of+unsigned_char-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_io_memory_image_of+unsigned_int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_io_memory_image_of+unsigned_short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_io_memory_image_of+vcl_complex+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_io_memory_image_of+vcl_complex+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_io_memory_image_of+vil1_rgb+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_io_memory_image_of+vil1_rgb+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_io_memory_image_of+vil1_rgb+uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_io_rgb+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_io_rgb+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_io_rgba+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_io_rgba+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_io_image_impl.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_io_memory_image_format.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_io_memory_image_impl.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_io_memory_image.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\vil1_io_image.h
# End Source File
# Begin Source File
SOURCE=.\vil1_io_memory_image.h
# End Source File
# Begin Source File
SOURCE=.\vil1_io_image_impl.h
# End Source File
# Begin Source File
SOURCE=.\vil1_io_memory_image_format.h
# End Source File
# Begin Source File
SOURCE=.\vil1_io_memory_image_impl.h
# End Source File
# Begin Source File
SOURCE=.\vil1_io_memory_image_of.h
# End Source File
# Begin Source File
SOURCE=.\vil1_io_memory_image_of.txx
# End Source File
# Begin Source File
SOURCE=.\vil1_io_rgb.h
# End Source File
# Begin Source File
SOURCE=.\vil1_io_rgb.txx
# End Source File
# Begin Source File
SOURCE=.\vil1_io_rgba.h
# End Source File
# Begin Source File
SOURCE=.\vil1_io_rgba.txx
# End Source File
# End Group
# End Target
# End Project
