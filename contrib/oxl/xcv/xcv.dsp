# Microsoft Developer Studio Project File - Name="xcv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=xcv - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xcv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xcv.mak" CFG="xcv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xcv - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "xcv - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xcv - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)" /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/Oxford" /I "$(IUEROOT)/vxl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "HAS_MVOX" /FR /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 nafxcw.lib mvox.lib alloxl.lib allvxl.lib opengl32.lib glu32.lib /nologo /subsystem:console /machine:I386 /libpath:"$(IUEROOT)/vxl/Release" /libpath:"$(IUEROOT)/v3p/Release" /libpath:"$(IUEROOT)/oxl/Release" /libpath:"$(IUEROOT)/Oxford/Release"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "xcv - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/Oxford" /I "$(IUEROOT)/vxl" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 mvl.lib Qv.lib glu32.lib opengl32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"$(IUEROOT)/vxl/Debug" /libpath:"$(IUEROOT)/v3p/Debug" /libpath:"$(IUEROOT)/oxl/Debug" /libpath:"$(IUEROOT)/Oxford/Debug"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "xcv - Win32 Release"
# Name "xcv - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=".\Templates\vcl_vector+vgui_easy2D~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+vgui_rubberbander~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+xcv_image_tableau~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+xcv_mview_manager~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+xcv_threeview_manager~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+xcv_twoview_manager~-.cxx"
# End Source File
# Begin Source File

SOURCE=.\xcv.cxx
# End Source File
# Begin Source File

SOURCE=.\xcv_display.cxx
# End Source File
# Begin Source File

SOURCE=.\xcv_file.cxx
# End Source File
# Begin Source File

SOURCE=.\xcv_geometry.cxx
# End Source File
# Begin Source File

SOURCE=.\xcv_image_tableau.cxx
# End Source File
# Begin Source File

SOURCE=.\xcv_multiview.cxx
# End Source File
# Begin Source File

SOURCE=.\xcv_processing.cxx
# End Source File
# Begin Source File

SOURCE=.\xcv_segmentation.cxx
# End Source File
# Begin Source File

SOURCE=.\xcv_threeview_manager.cxx
# End Source File
# Begin Source File

SOURCE=.\xcv_tjunction.cxx
# End Source File
# Begin Source File

SOURCE=.\xcv_twoview_manager.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\xcv_display.h
# End Source File
# Begin Source File

SOURCE=.\xcv_file.h
# End Source File
# Begin Source File

SOURCE=.\xcv_geometry.h
# End Source File
# Begin Source File

SOURCE=.\xcv_image_tableau.h
# End Source File
# Begin Source File

SOURCE=.\xcv_image_tableau_ref.h
# End Source File
# Begin Source File

SOURCE=.\xcv_multiview.h
# End Source File
# Begin Source File

SOURCE=.\xcv_mview_manager.h
# End Source File
# Begin Source File

SOURCE=.\xcv_segmentation.h
# End Source File
# Begin Source File

SOURCE=.\xcv_threeview_manager.h
# End Source File
# Begin Source File

SOURCE=.\xcv_tjunction.h
# End Source File
# Begin Source File

SOURCE=.\xcv_twoview_manager.h
# End Source File
# End Group
# End Target
# End Project
