# Microsoft Developer Studio Project File - Name="awfmfc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=awfmfc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "awfmfc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "awfmfc.mak" CFG="awfmfc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "awfmfc - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "awfmfc - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "awfmfc - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(VXLROOT)\vcl\config.win32-vc60" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vxl" /I "$(VXLROOT)\oxl" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 vgui_mfc.lib vgui_glut.lib vgui.lib vnl_algo.lib netlib.lib vnl.lib vbl.lib vil.lib vul.lib vgl.lib Qv.lib vcl.lib glu32.lib opengl32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"nafxcwd.lib" /libpath:"$(VXLROOT)\oxl\vgui\impl\mfc\Release" /libpath:"$(VXLROOT)\oxl\vgui\impl\glut\Release" /libpath:"$(VXLROOT)\oxl\vgui\Release" /libpath:"$(VXLROOT)\vxl\vnl\algo\Release" /libpath:"$(VXLROOT)\v3p\netlib\Release" /libpath:"$(VXLROOT)\vxl\vnl\Release" /libpath:"$(VXLROOT)\vxl\vbl\Release" /libpath:"$(VXLROOT)\vxl\vil\Release" /libpath:"$(VXLROOT)\vxl\vul\Release" /libpath:"$(VXLROOT)\vxl\vgl\Release" /libpath:"$(VXLROOT)\v3p\Qv\Release" /libpath:"$(VXLROOT)\vcl\Release" /libpath:"$(GLUT)\Release"

!ELSEIF  "$(CFG)" == "awfmfc - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(VXLROOT)\vcl\config.win32-vc60" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vxl" /I "$(VXLROOT)\oxl" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 vgui_mfc.lib vgui_glut.lib vgui.lib vnl_algo.lib netlib.lib vnl.lib vbl.lib vil.lib vul.lib vgl.lib Qv.lib vcl.lib glu32.lib opengl32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"nafxcwd.lib" /pdbtype:sept /libpath:"$(VXLROOT)\oxl\vgui\impl\mfc\Debug" /libpath:"$(VXLROOT)\oxl\vgui\impl\glut\Debug" /libpath:"$(VXLROOT)\oxl\vgui\Debug" /libpath:"$(VXLROOT)\vxl\vnl\algo\Debug" /libpath:"$(VXLROOT)\v3p\netlib\Debug" /libpath:"$(VXLROOT)\vxl\vnl\Debug" /libpath:"$(VXLROOT)\vxl\vbl\Debug" /libpath:"$(VXLROOT)\vxl\vil\Debug" /libpath:"$(VXLROOT)\vxl\vul\Debug" /libpath:"$(VXLROOT)\vxl\vgl\Debug" /libpath:"$(VXLROOT)\v3p\Qv\Debug" /libpath:"$(VXLROOT)\vcl\Debug" /libpath:"$(GLUT)\Debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "awfmfc - Win32 Release"
# Name "awfmfc - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\awfmfc.cpp
# End Source File
# Begin Source File

SOURCE=.\awfmfc.rc
# End Source File
# Begin Source File

SOURCE=.\awfmfcDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\awfmfcView.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\awfmfc.h
# End Source File
# Begin Source File

SOURCE=.\awfmfcDoc.h
# End Source File
# Begin Source File

SOURCE=.\awfmfcView.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\awfmfc.ico
# End Source File
# Begin Source File

SOURCE=.\res\awfmfc.rc2
# End Source File
# Begin Source File

SOURCE=.\res\awfmfcDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\awfmfc.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Target
# End Project
