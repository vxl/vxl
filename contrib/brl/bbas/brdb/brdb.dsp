# Microsoft Developer Studio Project File - Name="brdb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# brdb  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=brdb - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "brdb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "brdb.mak" CFG="brdb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "brdb - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "brdb - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "brdb - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "brdb_EXPORTS"
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

!ELSEIF  "$(CFG)" == "brdb - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "brdb_EXPORTS"
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
# Name "brdb - Win32 Release"
# Name "brdb - Win32 Debug"
# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\brdb_database.cxx
# End Source File
# Begin Source File
SOURCE=.\brdb_database_manager.cxx
# End Source File
# Begin Source File
SOURCE=.\brdb_query.cxx
# End Source File
# Begin Source File
SOURCE=.\brdb_relation.cxx
# End Source File
# Begin Source File
SOURCE=.\brdb_selection.cxx
# End Source File
# Begin Source File
SOURCE=.\brdb_tuple.cxx
# End Source File
# Begin Source File
SOURCE=.\brdb_value.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+long-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+vcl_string-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+brdb_database-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+brdb_relation-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+brdb_selection-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+brdb_tuple-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+brdb_value-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\brdb_database.h
# End Source File
# Begin Source File
SOURCE=.\brdb_database_manager.h
# End Source File
# Begin Source File
SOURCE=.\brdb_database_sptr.h
# End Source File
# Begin Source File
SOURCE=.\brdb_query.h
# End Source File
# Begin Source File
SOURCE=.\brdb_query_aptr.h
# End Source File
# Begin Source File
SOURCE=.\brdb_relation.h
# End Source File
# Begin Source File
SOURCE=.\brdb_relation_sptr.h
# End Source File
# Begin Source File
SOURCE=.\brdb_selection.h
# End Source File
# Begin Source File
SOURCE=.\brdb_selection_sptr.h
# End Source File
# Begin Source File
SOURCE=.\brdb_tuple.h
# End Source File
# Begin Source File
SOURCE=.\brdb_tuple_sptr.h
# End Source File
# Begin Source File
SOURCE=.\brdb_value.h
# End Source File
# Begin Source File
SOURCE=.\brdb_value.txx
# End Source File
# Begin Source File
SOURCE=.\brdb_value_sptr.h
# End Source File
# End Group
# End Target
# End Project
