# Microsoft Developer Studio Project File - Name="glmovie_example" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=glmovie_example - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "glmovie_example.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "glmovie_example.mak" CFG="glmovie_example - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "glmovie_example - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "glmovie_example - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "glmovie_example - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\vcl" /I "$(GLUT)\include" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\oxl" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 vgui_mfc.lib vgui_glut.lib vgui.lib vnl_algo.lib netlib.lib vnl.lib vbl.lib vil.lib vul.lib vgl.lib Qv.lib vcl.lib glu32.lib opengl32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"nafxcwd.lib" /libpath:"$(VXLROOT)\lib\Release"

!ELSEIF  "$(CFG)" == "glmovie_example - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\vcl" /I "$(GLUT)\include" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\oxl" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 vgui_mfc.lib vgui_glut.lib vgui.lib vnl_algo.lib netlib.lib vnl.lib vbl.lib vil.lib vul.lib vgl.lib Qv.lib vcl.lib glu32.lib opengl32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"nafxcwd.lib" /pdbtype:sept /libpath:"$(VXLROOT)\lib\Debug"

!ENDIF 

# Begin Target

# Name "glmovie_example - Win32 Release"
# Name "glmovie_example - Win32 Debug"
# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\glmovie.cxx
# End Source File
# End Group
# End Target
# End Project
