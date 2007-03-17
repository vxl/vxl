# Microsoft Developer Studio Project File - Name="geotiff" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
#
# /I "$(VXLROOT)/vcl/config.win32" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# geotiff  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=geotiff - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "geotiff.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "geotiff.mak" CFG="geotiff - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "geotiff - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "geotiff - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "geotiff - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "geotiff_EXPORTS"
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

!ELSEIF  "$(CFG)" == "geotiff - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "geotiff_EXPORTS"
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
# Name "geotiff - Win32 Release"
# Name "geotiff - Win32 Debug"
# Begin Group "Source Files"
# PROP Default_Filter "c"
# Begin Source File
SOURCE=.\cpl_csv.c
# End Source File
# Begin Source File
SOURCE=.\cpl_serv.c
# End Source File
# Begin Source File
SOURCE=.\geo_extra.c
# End Source File
# Begin Source File
SOURCE=.\geo_free.c
# End Source File
# Begin Source File
SOURCE=.\geo_get.c
# End Source File
# Begin Source File
SOURCE=.\geo_names.c
# End Source File
# Begin Source File
SOURCE=.\geo_new.c
# End Source File
# Begin Source File
SOURCE=.\geo_normalize.c
# End Source File
# Begin Source File
SOURCE=.\geo_print.c
# End Source File
# Begin Source File
SOURCE=.\geo_set.c
# End Source File
# Begin Source File
SOURCE=.\geo_tiffp.c
# End Source File
# Begin Source File
SOURCE=.\geo_trans.c
# End Source File
# Begin Source File
SOURCE=.\geo_write.c
# End Source File
# Begin Source File
SOURCE=.\geotiff_proj4.c
# End Source File
# Begin Source File
SOURCE=.\xtiff.c
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h"
# Begin Source File
SOURCE=.\cpl_serv.h
# End Source File
# Begin Source File
SOURCE=.\defs.h
# End Source File
# Begin Source File
SOURCE=.\geo_config.h
# End Source File
# Begin Source File
SOURCE=.\geo_keyp.h
# End Source File
# Begin Source File
SOURCE=.\geo_normalize.h
# End Source File
# Begin Source File
SOURCE=.\geo_tiffp.h
# End Source File
# Begin Source File
SOURCE=.\geokeys.h
# End Source File
# Begin Source File
SOURCE=.\geonames.h
# End Source File
# Begin Source File
SOURCE=.\geotiff.h
# End Source File
# Begin Source File
SOURCE=.\geotiffio.h
# End Source File
# Begin Source File
SOURCE=.\geovalues.h
# End Source File
# Begin Source File
SOURCE=.\xtiffio.h
# End Source File
# End Group
# End Target
# End Project
