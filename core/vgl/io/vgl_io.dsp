# Microsoft Developer Studio Project File - Name="vgl_io" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vgl_io - Win32 StaticDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vgl_io.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vgl_io.mak" CFG="vgl_io - Win32 StaticDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vgl_io - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vgl_io - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vgl_io - Win32 StaticDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "vgl_io - Win32 StaticRelease" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vgl_io - Win32 Release"

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)" /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Release\vgl_io.lib"

!ELSEIF  "$(CFG)" == "vgl_io - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FD /GZ "
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Debug\vgl_io.lib"

!ELSEIF  "$(CFG)" == "vgl_io - Win32 StaticDebug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vgl_io___Win32_StaticDebug"
# PROP BASE Intermediate_Dir "vgl_io___Win32_StaticDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "StaticDebug"
# PROP Intermediate_Dir "StaticDebug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FD /GZ "
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ "
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\Debug\vgl_io.lib"
# ADD LIB32 /nologo /out:"..\..\StaticDebug\vgl_io.lib"

!ELSEIF  "$(CFG)" == "vgl_io - Win32 StaticRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vgl_io___Win32_StaticRelease"
# PROP BASE Intermediate_Dir "vgl_io___Win32_StaticRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "StaticRelease"
# PROP Intermediate_Dir "StaticRelease"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)" /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)" /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\Release\vgl_io.lib"
# ADD LIB32 /nologo /out:"..\..\StaticRelease\vgl_io.lib"

!ENDIF 

# Begin Target

# Name "vgl_io - Win32 Release"
# Name "vgl_io - Win32 Debug"
# Name "vgl_io - Win32 StaticDebug"
# Name "vgl_io - Win32 StaticRelease"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;txx"
# Begin Source File

SOURCE=".\Templates\vgl_io_box_2d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_box_2d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_box_2d+int-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vgl_io_box_2d.txx
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_box_3d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_box_3d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vgl_io_box_3d.txx
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_homg_line_2d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_homg_line_2d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vgl_io_homg_line_2d.txx
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_homg_line_3d_2_points+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_homg_line_3d_2_points+float-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vgl_io_homg_line_3d_2_points.txx
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_homg_plane_3d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_homg_plane_3d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vgl_io_homg_plane_3d.txx
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_homg_point_2d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_homg_point_2d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vgl_io_homg_point_2d.txx
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_homg_point_3d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_homg_point_3d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vgl_io_homg_point_3d.txx
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_line_2d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_line_2d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vgl_io_line_2d.txx
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_line_segment_2d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_line_segment_2d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vgl_io_line_segment_2d.txx
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_line_segment_3d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_line_segment_3d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vgl_io_line_segment_3d.txx
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_point_2d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_point_2d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vgl_io_point_2d.txx
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_point_3d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_io_point_3d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vgl_io_point_3d.txx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\dll.h
# End Source File
# Begin Source File

SOURCE=.\vgl_io_box_2d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_io_box_3d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_io_homg_line_2d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_io_homg_line_3d_2_points.h
# End Source File
# Begin Source File

SOURCE=.\vgl_io_homg_plane_3d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_io_homg_point_2d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_io_homg_point_3d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_io_line_2d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_io_line_segment_2d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_io_line_segment_3d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_io_point_2d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_io_point_3d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_io_polygon.cxx
# End Source File
# Begin Source File

SOURCE=.\vgl_io_polygon.h
# End Source File
# End Group
# End Target
# End Project
