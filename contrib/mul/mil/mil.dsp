# Microsoft Developer Studio Project File - Name="mil" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul"  == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
# $(IUEROOT)/WIN32-VC60/lib/ == override in output directory
# mil  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mil - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mil.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mil.mak" CFG="mil - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mil - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mil - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "mil - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mil - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mil - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "$(IUEROOT)/WIN32-VC60/lib/Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "mil_EXPORTS"
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

!ELSEIF  "$(CFG)" == "mil - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "$(IUEROOT)/WIN32-VC60/lib/Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /GZ /c
# ADD CPP /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "mil_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ELSEIF  "$(CFG)" == "mil - Win32 MinSizeRel"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MinSizeRel"
# PROP BASE Intermediate_Dir "MinSizeRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "$(IUEROOT)/WIN32-VC60/lib/MinSizeRel"
# PROP Intermediate_Dir "MinSizeRel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "_MBCS" /D "_ATL_DLL"  /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FD /c
# ADD CPP /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "mil_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O1
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mil - Win32 RelWithDebInfo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RelWithDebInfo"
# PROP BASE Intermediate_Dir "RelWithDebInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "$(IUEROOT)/WIN32-VC60/lib/RelWithDebInfo"
# PROP Intermediate_Dir "RelWithDebInfo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "mil_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /Zi /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "mil - Win32 Release"
# Name "mil - Win32 Debug"
# Name "mil - Win32 MinSizeRel"
# Name "mil - Win32 RelWithDebInfo"


# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Templates/mil_gaussian_pyramid_builder+vil_byte-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mil_gaussian_pyramid_builder_2d+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mil_gaussian_pyramid_builder_2d_general+vil_byte-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mil_image_2d_of+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mil_image_2d_of+vil_byte-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mil_image_data+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mil_image_data+vil_byte-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mil_sample_profile_2d+float.double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mil_sample_profile_2d+uchar.double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_io_smart_ptr+mil_image_data+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_io_smart_ptr+mil_image_data+vil_byte--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+mil_image_data+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+mil_image_data+vil_byte--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+mil_image_io~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+mil_image_pyramid_builder~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+mil_image~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_binary_loader+mil_image-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_binary_loader+mil_image_io-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_binary_loader+mil_image_pyramid_builder-.cxx

# End Source File
# Begin Source File

SOURCE=.\mil_byte_image_2d_io.cxx

# End Source File
# Begin Source File

SOURCE=.\mil_convert_image_2d.cxx

# End Source File
# Begin Source File

SOURCE=.\mil_gauss_reduce_2d.cxx

# End Source File
# Begin Source File

SOURCE=.\mil_gaussian_pyramid_builder_2d.cxx

# End Source File
# Begin Source File

SOURCE=.\mil_gaussian_pyramid_builder_2d_general.cxx

# End Source File
# Begin Source File

SOURCE=.\mil_image.cxx

# End Source File
# Begin Source File

SOURCE=.\mil_image_2d_of.cxx

# End Source File
# Begin Source File

SOURCE=.\mil_image_data.cxx

# End Source File
# Begin Source File

SOURCE=.\mil_image_io.cxx

# End Source File
# Begin Source File

SOURCE=.\mil_image_pyramid.cxx

# End Source File
# Begin Source File

SOURCE=.\mil_image_pyramid_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\mil_transform_2d.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

