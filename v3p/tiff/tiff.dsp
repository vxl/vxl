# Microsoft Developer Studio Project File - Name="tiff" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl/config.win32" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# tiff  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=tiff - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tiff.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tiff.mak" CFG="tiff - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tiff - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "tiff - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tiff - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "tiff_EXPORTS"
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

!ELSEIF  "$(CFG)" == "tiff - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "tiff_EXPORTS"
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

# Name "tiff - Win32 Release"
# Name "tiff - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\tif_aux.c

# End Source File
# Begin Source File

SOURCE=.\tif_close.c

# End Source File
# Begin Source File

SOURCE=.\tif_codec.c

# End Source File
# Begin Source File

SOURCE=.\tif_compress.c

# End Source File
# Begin Source File

SOURCE=.\tif_dir.c

# End Source File
# Begin Source File

SOURCE=.\tif_dirinfo.c

# End Source File
# Begin Source File

SOURCE=.\tif_dirread.c

# End Source File
# Begin Source File

SOURCE=.\tif_dirwrite.c

# End Source File
# Begin Source File

SOURCE=.\tif_dumpmode.c

# End Source File
# Begin Source File

SOURCE=.\tif_error.c

# End Source File
# Begin Source File

SOURCE=.\tif_fax3.c

# End Source File
# Begin Source File

SOURCE=.\tif_fax3sm.c

# End Source File
# Begin Source File

SOURCE=.\tif_flush.c

# End Source File
# Begin Source File

SOURCE=.\tif_getimage.c

# End Source File
# Begin Source File

SOURCE=.\tif_jpeg.c

# End Source File
# Begin Source File

SOURCE=.\tif_luv.c

# End Source File
# Begin Source File

SOURCE=.\tif_lzw.c

# End Source File
# Begin Source File

SOURCE=.\tif_next.c

# End Source File
# Begin Source File

SOURCE=.\tif_open.c

# End Source File
# Begin Source File

SOURCE=.\tif_packbits.c

# End Source File
# Begin Source File

SOURCE=.\tif_pixarlog.c

# End Source File
# Begin Source File

SOURCE=.\tif_predict.c

# End Source File
# Begin Source File

SOURCE=.\tif_print.c

# End Source File
# Begin Source File

SOURCE=.\tif_read.c

# End Source File
# Begin Source File

SOURCE=.\tif_strip.c

# End Source File
# Begin Source File

SOURCE=.\tif_swab.c

# End Source File
# Begin Source File

SOURCE=.\tif_thunder.c

# End Source File
# Begin Source File

SOURCE=.\tif_tile.c

# End Source File
# Begin Source File

SOURCE=.\tif_version.c

# End Source File
# Begin Source File

SOURCE=.\tif_warning.c

# End Source File
# Begin Source File

SOURCE=.\tif_win32.c

# End Source File
# Begin Source File

SOURCE=.\tif_write.c

# End Source File
# Begin Source File

SOURCE=.\tif_zip.c

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\format.h

# End Source File
# Begin Source File

SOURCE=.\machdep.h

# End Source File
# Begin Source File

SOURCE=.\port.h

# End Source File
# Begin Source File

SOURCE=.\prototypes.h

# End Source File
# Begin Source File

SOURCE=.\t4.h

# End Source File
# Begin Source File

SOURCE=.\tif_dir.h

# End Source File
# Begin Source File

SOURCE=.\tif_fax3.h

# End Source File
# Begin Source File

SOURCE=.\tif_predict.h

# End Source File
# Begin Source File

SOURCE=.\tiff.h

# End Source File
# Begin Source File

SOURCE=.\tiffcomp.h

# End Source File
# Begin Source File

SOURCE=.\tiffcompat.h

# End Source File
# Begin Source File

SOURCE=.\tiffconf.h

# End Source File
# Begin Source File

SOURCE=.\tiffio.h

# End Source File
# Begin Source File

SOURCE=.\tiffiop.h

# End Source File
# Begin Source File

SOURCE=.\uvcode.h

# End Source File
# Begin Source File

SOURCE=.\version.h

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

