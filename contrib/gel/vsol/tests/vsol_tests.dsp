# Microsoft Developer Studio Project File - Name="vsol_tests" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# CM DSP Header file
# This file is read by the build system of cm, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/core" /I "$(VXLROOT)/contrib/gel" == include path
# $(VXLROOT)/bin/ == override in output directory
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL == compiler defines
# vsol_tests  == name of output library
# /libpath:"$(VXLROOT)/contrib/gel/vsol/$(OUTDIR)" /libpath:"$(VXLROOT)/core/vgl/algo/$(OUTDIR)" /libpath:"$(VXLROOT)/core/vgl/$(OUTDIR)" /libpath:"$(VXLROOT)/vcl/$(OUTDIR)" /libpath:"$(VXLROOT)/core/vnl/algo/$(OUTDIR)" /libpath:"$(VXLROOT)/v3p/netlib/$(OUTDIR)" /libpath:"$(VXLROOT)/core/vnl/$(OUTDIR)" /libpath:"$(VXLROOT)/core/vul/$(OUTDIR)" "opengl32.lib" "glu32.lib" "vsol.lib" "vgl_algo.lib" "vgl.lib" "vnl_algo.lib" "netlib.lib" "vnl.lib" "vbl.lib" "vpl.lib" "vul.lib" "vsl.lib" "vcl.lib" /STACK:10000000  == libraries linked in 
# TARGTYPE "Win32 (x86) Application" 0x0103

CFG=vsol_tests - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vsol_tests.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vsol_tests.mak" CFG="vsol_tests - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vsol_tests - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "vsol_tests - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vsol_tests - Win32 Release"

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
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# ADD CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/core" /I "$(VXLROOT)/contrib/gel" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "vsol_tests_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089

# ADD LINK32 /libpath:"$(VXLROOT)/contrib/gel/vsol/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vgl/algo/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vgl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/vcl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vnl/algo/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/v3p/netlib/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vnl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vbl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vpl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vul/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vsl/$(OUTDIR)"
# ADD LINK32 "opengl32.lib" "glu32.lib" "vsol.lib" "vgl_algo.lib" "vgl.lib" "vnl_algo.lib" "netlib.lib" "vnl.lib" "vbl.lib" "vpl.lib" "vul.lib" "vsl.lib" "vcl.lib" /STACK:10000000 


!ELSEIF  "$(CFG)" == "vsol_tests - Win32 Debug"

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
# ADD BASE CPP   /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /GZ /c
# ADD CPP /nologo  /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /GZ /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/core" /I "$(VXLROOT)/contrib/gel" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "vsol_tests_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /IGNORE:4089

# ADD LINK32 /libpath:"$(VXLROOT)/contrib/gel/vsol/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vgl/algo/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vgl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/vcl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vnl/algo/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/v3p/netlib/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vnl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vbl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vul/$(OUTDIR)"
# ADD LINK32 "opengl32.lib" "glu32.lib" "vsol.lib" "vgl_algo.lib" "vgl.lib" "vnl_algo.lib" "netlib.lib" "vnl.lib" "vbl.lib" "vpl.lib" "vul.lib" "vsl.lib" "vcl.lib" /STACK:10000000 

!ENDIF 

# Begin Target

# Name "vsol_tests - Win32 Release"
# Name "vsol_tests - Win32 Debug"

# Begin Group "Source Files"
# Begin Source File
SOURCE=.\test_driver.cxx
# End Source File
# Begin Source File
SOURCE=.\test_include.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vsol_conic_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vsol_group_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vsol_group_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vsol_line_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vsol_line_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vsol_point_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vsol_point_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vsol_polygon_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vsol_polygon_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vsol_rectangle_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vsol_rectangle_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vsol_tetrahedron.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vsol_triangle_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vsol_triangle_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vsol_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vsol_digital_curve_2d.cxx
# End Source File
# End Group
# End Target
# End Project
