# Microsoft Developer Studio Project File - Name="vil_test_all" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=vil_test_all - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vil_test_all.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vil_test_all.mak" CFG="vil_test_all - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vil_test_all - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "vil_test_all - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vil_test_all - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "$(VXLROOT)\vcl\config.win32-vc60" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vxl" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 /libpath:"$(VXLROOT)/vxl/vil/$(OUTDIR)" /libpath:"$(VXLROOT)/vxl/vpl/$(OUTDIR)" /libpath:"$(VXLROOT)/vcl/$(OUTDIR)" /libpath:"$(VXLROOT)/v3p/jpeg/$(OUTDIR)" /libpath:"$(VXLROOT)/v3p/png/$(OUTDIR)" /libpath:"$(VXLROOT)/v3p/zlib/$(OUTDIR)" /libpath:"$(VXLROOT)/v3p/tiff/$(OUTDIR)" "vil.lib" "vpl.lib" "jpeg.lib" "png.lib" "zlib.lib" "tiff.lib" "vcl.lib"

!ELSEIF  "$(CFG)" == "vil_test_all - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob0 /I "$(VXLROOT)\vcl\config.win32-vc60" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vxl" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /libpath:"$(VXLROOT)/vxl/vil/$(OUTDIR)" /libpath:"$(VXLROOT)/vxl/vpl/$(OUTDIR)" /libpath:"$(VXLROOT)/vcl/$(OUTDIR)" /libpath:"$(VXLROOT)/v3p/jpeg/$(OUTDIR)" /libpath:"$(VXLROOT)/v3p/png/$(OUTDIR)" /libpath:"$(VXLROOT)/v3p/zlib/$(OUTDIR)" /libpath:"$(VXLROOT)/v3p/tiff/$(OUTDIR)" "vil.lib" "vpl.lib" "jpeg.lib" "png.lib" "zlib.lib" "tiff.lib" "vcl.lib"

!ENDIF 

# Begin Target

# Name "vil_test_all - Win32 Release"
# Name "vil_test_all - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File
SOURCE=.\test_driver.cxx
# End Source File
# Begin Source File
SOURCE=.\test_assign.cxx
# End Source File
# Begin Source File
SOURCE=.\test_colour_space.cxx
# End Source File
# Begin Source File
SOURCE=.\test_copy.cxx
# End Source File
# Begin Source File
SOURCE=.\test_file_format_read.cxx
# End Source File
# Begin Source File
SOURCE=.\test_flipud.cxx
# End Source File
# Begin Source File
SOURCE=.\test_include.cxx
# End Source File
# Begin Source File
SOURCE=.\test_interpolate.cxx
# End Source File
# Begin Source File
SOURCE=.\test_load.cxx
# End Source File
# Begin Source File
SOURCE=.\test_memory_image.cxx
# End Source File
# Begin Source File
SOURCE=.\test_memory_image_of.cxx
# End Source File
# Begin Source File
SOURCE=.\test_save_load_image.cxx
# End Source File
# Begin Source File
SOURCE=.\test_write_endian.cxx
# End Source File
# Begin Source File
SOURCE=.\test_load_gen.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
