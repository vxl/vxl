# Microsoft Developer Studio Project File - Name="gmvl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=gmvl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gmvl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gmvl.mak" CFG="gmvl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gmvl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gmvl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gmvl - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(IUEROOT)/gel" /I "$(IUEROOT)/vcl/config.win32-vc60" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)" /I "$(IUEROOT)/vxl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "gmvl - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(IUEROOT)/gel" /I "$(IUEROOT)/vcl/config.win32-vc60" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)" /I "$(IUEROOT)/vxl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Debug\gmvl.lib"

!ENDIF 

# Begin Target

# Name "gmvl - Win32 Release"
# Name "gmvl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\gmvl_connection.cxx
# End Source File
# Begin Source File

SOURCE=.\gmvl_connection_cache.cxx
# End Source File
# Begin Source File

SOURCE=.\gmvl_corner_node.cxx
# End Source File
# Begin Source File

SOURCE=.\gmvl_database.cxx
# End Source File
# Begin Source File

SOURCE=.\gmvl_helpers.cxx
# End Source File
# Begin Source File

SOURCE=.\gmvl_image_node.cxx
# End Source File
# Begin Source File

SOURCE=.\gmvl_node.cxx
# End Source File
# Begin Source File

SOURCE=.\gmvl_node_cache.cxx
# End Source File
# Begin Source File

SOURCE=.\gmvl_tag_node.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_smart_ptr+gmvl_connection-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_smart_ptr+gmvl_node-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+gmvl_connection_ref-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+gmvl_node_ref-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+vcl_pair+vcl_string.vcl_vector+gmvl_node_ref---.cxx"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\gmvl_connection.h
# End Source File
# Begin Source File

SOURCE=.\gmvl_connection_cache.h
# End Source File
# Begin Source File

SOURCE=.\gmvl_connection_ref.h
# End Source File
# Begin Source File

SOURCE=.\gmvl_corner_node.h
# End Source File
# Begin Source File

SOURCE=.\gmvl_database.h
# End Source File
# Begin Source File

SOURCE=.\gmvl_helpers.h
# End Source File
# Begin Source File

SOURCE=.\gmvl_image_node.h
# End Source File
# Begin Source File

SOURCE=.\gmvl_node.h
# End Source File
# Begin Source File

SOURCE=.\gmvl_node_cache.h
# End Source File
# Begin Source File

SOURCE=.\gmvl_node_ref.h
# End Source File
# Begin Source File

SOURCE=.\gmvl_tag_node.h
# End Source File
# End Group
# End Target
# End Project
