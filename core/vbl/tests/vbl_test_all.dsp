# Microsoft Developer Studio Project File - Name="vbl_test_all" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=vbl_test_all - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vbl_test_all.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vbl_test_all.mak" CFG="vbl_test_all - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vbl_test_all - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "vbl_test_all - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vbl_test_all - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 vcl.lib /libpath:"$(VXLROOT)/vcl/$(OUTDIR)" /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "vbl_test_all - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob0 /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 vcl.lib /libpath:"$(VXLROOT)/vcl/$(OUTDIR)" /nologo /subsystem:console /machine:I386

!ENDIF 

# Begin Target

# Name "vbl_test_all - Win32 Release"
# Name "vbl_test_all - Win32 Debug"
# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\test_driver.cxx
# End Source File
# Begin Source File
SOURCE=.\vbl_test_classes.cxx
# End Source File
# Begin Source File
SOURCE=.\vbl_test_array_x.cxx
# End Source File
# Begin Source File
SOURCE=.\vbl_test_bounding_box.cxx
# End Source File
# Begin Source File
SOURCE=.\vbl_test_triple.cxx
# End Source File
# Begin Source File
SOURCE=.\vbl_test_quadruple.cxx
# End Source File
# Begin Source File
SOURCE=.\vbl_test_qsort.cxx
# End Source File
# Begin Source File
SOURCE=.\vbl_test_array.cxx
# End Source File
# Begin Source File
SOURCE=.\vbl_test_bit_array.cxx
# End Source File
# Begin Source File
SOURCE=.\vbl_test_smart_ptr.cxx
# End Source File
# Begin Source File
SOURCE=.\vbl_test_sparse_array_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\vbl_test_sparse_array_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\vbl_test_ref_count.cxx
# End Source File
# Begin Source File
SOURCE=.\vbl_test_scoped_ptr.cxx
# End Source File
# Begin Source File
SOURCE=.\vbl_test_shared_pointer.cxx
# End Source File
# Begin Source File
SOURCE=.\vbl_test_local_minima.cxx
# End Source File
# Begin Source File
SOURCE=.\vbl_test_attributes.cxx
# End Source File
# Begin Source File
SOURCE=.\test_include.cxx
# End Source File
# Begin Source File
SOURCE=.\test_template_include.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\vbl_test_classes.h
# End Source File
# Begin Source File
SOURCE=.\vbl_test_array_x.h
# End Source File
# End Group
# End Target
# End Project
