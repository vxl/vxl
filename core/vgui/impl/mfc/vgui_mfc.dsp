# Microsoft Developer Studio Project File - Name="vgui_mfc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vgui_mfc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vgui_mfc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vgui_mfc.mak" CFG="vgui_mfc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vgui_mfc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vgui_mfc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vgui_mfc - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)" /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/oxl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\Release\vgui_mfc.lib"

!ELSEIF  "$(CFG)" == "vgui_mfc - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/oxl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\Debug\vgui_mfc.lib"

!ENDIF 

# Begin Target

# Name "vgui_mfc - Win32 Release"
# Name "vgui_mfc - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\vgui_mfc.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_adaptor.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_app.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_app_init.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_dialog_impl.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_doc.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_mainfrm.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_statusbar.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_tag.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_utils.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_view.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_window.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc.h
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_adaptor.h
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_app.h
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_app_init.h
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_dialog_impl.h
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_doc.h
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_mainfrm.h
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_statusbar.h
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_utils.h
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_view.h
# End Source File
# Begin Source File

SOURCE=.\vgui_mfc_window.h
# End Source File
# End Group
# Begin Group "Resource files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
