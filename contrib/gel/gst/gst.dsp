# Microsoft Developer Studio Project File - Name="gst" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=gst - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gst.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gst.mak" CFG="gst - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gst - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gst - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gst - Win32 Release"

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

!ELSEIF  "$(CFG)" == "gst - Win32 Debug"

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
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Debug\gst.lib"

!ENDIF 

# Begin Target

# Name "gst - Win32 Release"
# Name "gst - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\gst_edge_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\gst_face_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\gst_polygon_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\gst_polygon_2d_operators.cxx
# End Source File
# Begin Source File

SOURCE=.\gst_vertex_2d.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_smart_ptr+gst_edge_2d-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_smart_ptr+gst_polygon_2d-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_smart_ptr+gst_vertex_2d-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+gst_edge_2d_ref-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+gst_polygon_2d_ref-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+gst_vertex_2d_ref-.cxx"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\gst_edge_2d.h
# End Source File
# Begin Source File

SOURCE=.\gst_edge_2d_ref.h
# End Source File
# Begin Source File

SOURCE=.\gst_face_2d.h
# End Source File
# Begin Source File

SOURCE=.\gst_polygon_2d.h
# End Source File
# Begin Source File

SOURCE=.\gst_polygon_2d_operators.h
# End Source File
# Begin Source File

SOURCE=.\gst_polygon_2d_ref.h
# End Source File
# Begin Source File

SOURCE=.\gst_vertex_2d.h
# End Source File
# Begin Source File

SOURCE=.\gst_vertex_2d_ref.h
# End Source File
# End Group
# End Target
# End Project
