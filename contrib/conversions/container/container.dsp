# Microsoft Developer Studio Project File - Name="container" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=container - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "container.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "container.mak" CFG="container - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "container - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "container - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "container - Win32 Release"

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
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "container - Win32 Debug"

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
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ENDIF 

# Begin Target

# Name "container - Win32 Release"
# Name "container - Win32 Debug"

# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\CoolArray_to_CoolList.h
# End Source File
# Begin Source File
SOURCE=.\CoolArray_to_vcl_list.h
# End Source File
# Begin Source File
SOURCE=.\CoolArray_to_vcl_vector.h
# End Source File
# Begin Source File
SOURCE=.\CoolList_to_CoolArray.h
# End Source File
# Begin Source File
SOURCE=.\CoolList_to_vcl_list.h
# End Source File
# Begin Source File
SOURCE=.\CoolList_to_vcl_vector.h
# End Source File
# Begin Source File
SOURCE=.\vcl_list_to_CoolArray.h
# End Source File
# Begin Source File
SOURCE=.\vcl_list_to_CoolList.h
# End Source File
# Begin Source File
SOURCE=.\vcl_list_to_vcl_vector.h
# End Source File
# Begin Source File
SOURCE=.\vcl_vector_to_CoolArray.h
# End Source File
# Begin Source File
SOURCE=.\vcl_vector_to_CoolList.h
# End Source File
# Begin Source File
SOURCE=.\vcl_vector_to_vcl_list.h
# End Source File
# Begin Source File
SOURCE=.\vcl_vector_to_vnl_vector.h
# End Source File
# Begin Source File
SOURCE=.\vnl_vector_to_vcl_vector.h
# End Source File
# End Group
# End Target
# End Project
