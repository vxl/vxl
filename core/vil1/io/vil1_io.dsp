# Microsoft Developer Studio Project File - Name="vil_io" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vil_io - Win32 StaticDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vil_io.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vil_io.mak" CFG="vil_io - Win32 StaticDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vil_io - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vil_io - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vil_io - Win32 StaticDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "vil_io - Win32 StaticRelease" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vil_io - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\mul" /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Release\vil_io.lib"

!ELSEIF  "$(CFG)" == "vil_io - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\mul" /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Debug\vil_io.lib"

!ELSEIF  "$(CFG)" == "vil_io - Win32 StaticDebug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vil_io___Win32_StaticDebug"
# PROP BASE Intermediate_Dir "vil_io___Win32_StaticDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "StaticDebug"
# PROP Intermediate_Dir "StaticDebug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\mul" /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\Debug\vil_io.lib"
# ADD LIB32 /nologo /out:"..\..\StaticDebug\vil_io.lib"

!ELSEIF  "$(CFG)" == "vil_io - Win32 StaticRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vil_io___Win32_StaticRelease"
# PROP BASE Intermediate_Dir "vil_io___Win32_StaticRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "StaticRelease"
# PROP Intermediate_Dir "StaticRelease"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\mul" /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\Release\vil_io.lib"
# ADD LIB32 /nologo /out:"..\..\StaticRelease\vil_io.lib"

!ENDIF 

# Begin Target

# Name "vil_io - Win32 Release"
# Name "vil_io - Win32 Debug"
# Name "vil_io - Win32 StaticDebug"
# Name "vil_io - Win32 StaticRelease"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;txx"
# Begin Source File

SOURCE=.\vil_io_image.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_io_image_impl.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_io_memory_image.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_io_memory_image_format.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_io_memory_image_impl.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_io_memory_image_of+bool-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_io_memory_image_of+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_io_memory_image_of+double_complex-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_io_memory_image_of+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_io_memory_image_of+float_complex-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_io_memory_image_of+signed_char-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_io_memory_image_of+signed_int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_io_memory_image_of+signed_short-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_io_memory_image_of+unsigned_char-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_io_memory_image_of+unsigned_int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_io_memory_image_of+unsigned_short-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_io_memory_image_of+vil_rgb+byte--.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_io_memory_image_of+vil_rgb+double--.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_io_memory_image_of+vil_rgb+float--.cxx"
# End Source File
# Begin Source File

SOURCE=.\vil_io_memory_image_of.txx
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_io_rgb+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_io_rgb+float-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vil_io_rgb.txx
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_io_rgba+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_io_rgba+float-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vil_io_rgba.txx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\dll.h
# End Source File
# Begin Source File

SOURCE=.\vil_io_image.h
# End Source File
# Begin Source File

SOURCE=.\vil_io_image_impl.h
# End Source File
# Begin Source File

SOURCE=.\vil_io_memory_image.h
# End Source File
# Begin Source File

SOURCE=.\vil_io_memory_image_format.h
# End Source File
# Begin Source File

SOURCE=.\vil_io_memory_image_impl.h
# End Source File
# Begin Source File

SOURCE=.\vil_io_memory_image_of.h
# End Source File
# Begin Source File

SOURCE=.\vil_io_rgb.h
# End Source File
# Begin Source File

SOURCE=.\vil_io_rgba.h
# End Source File
# End Group
# End Target
# End Project
