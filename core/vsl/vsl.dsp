# Microsoft Developer Studio Project File - Name="vsl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vsl - Win32 StaticDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vsl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vsl.mak" CFG="vsl - Win32 StaticDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vsl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vsl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vsl - Win32 StaticDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "vsl - Win32 StaticRelease" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vsl - Win32 Release"

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
# ADD LIB32 /nologo /out:"..\Release\vsl.lib"

!ELSEIF  "$(CFG)" == "vsl - Win32 Debug"

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\mul" /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Debug\vsl.lib"

!ELSEIF  "$(CFG)" == "vsl - Win32 StaticDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "StaticDebug"
# PROP BASE Intermediate_Dir "StaticDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "StaticDebug"
# PROP Intermediate_Dir "StaticDebug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\mul" /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\StaticDebug\vsl.lib"

!ELSEIF  "$(CFG)" == "vsl - Win32 StaticRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "StaticRelease"
# PROP BASE Intermediate_Dir "StaticRelease"
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
# ADD BASE LIB32
# ADD LIB32 /nologo /out:"..\StaticRelease\vsl.lib"

!ENDIF 

# Begin Target

# Name "vsl - Win32 Release"
# Name "vsl - Win32 Debug"
# Name "vsl - Win32 StaticDebug"
# Name "vsl - Win32 StaticRelease"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;txx"
# Begin Source File

SOURCE=".\Templates\vcl_vector+vsl_binary_loader_base~-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vsl_binary_io.cxx
# End Source File
# Begin Source File

SOURCE=.\vsl_binary_loader_base.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_complex+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_complex+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_deque+int-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vsl_indent.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_list+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_list+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_pair+uint.uint-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_pair+vcl_pair+uint.uint-.double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_pair+vcl_pair+uint.uint-.int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_set+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_set+vcl_string-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_vector_io+vcl_string-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_vector_io+uint-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_vector_io+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_vector_io+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_vector_io+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_map_io+vcl_string.int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_map_io+int.vcl_string-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_map_io+int.int-.cxx"

# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_stack+int-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vsl_test.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\dll.h
# End Source File
# Begin Source File

SOURCE=.\vsl_binary_loader.txx
# End Source File
# Begin Source File

SOURCE=.\vsl_clipon_binary_loader.txx
# End Source File
# Begin Source File

SOURCE=.\vsl_complex_io.txx
# End Source File
# Begin Source File

SOURCE=.\vsl_deque_io.txx
# End Source File
# Begin Source File

SOURCE=.\vsl_list_io.txx
# End Source File
# Begin Source File

SOURCE=.\vsl_map_io.txx
# End Source File
# Begin Source File

SOURCE=.\vsl_pair_io.txx
# End Source File
# Begin Source File

SOURCE=.\vsl_set_io.txx
# End Source File
# Begin Source File

SOURCE=.\vsl_stack_io.txx
# End Source File
# Begin Source File

SOURCE=.\vsl_string_io.txx
# End Source File
# Begin Source File

SOURCE=.\vsl_vector_io.txx
# End Source File
# Begin Source File

SOURCE=.\vsl_binary_explicit_io.h
# End Source File
# Begin Source File

SOURCE=.\vsl_binary_io.h
# End Source File
# Begin Source File

SOURCE=.\vsl_binary_loader.h
# End Source File
# Begin Source File

SOURCE=.\vsl_binary_loader_base.h
# End Source File
# Begin Source File

SOURCE=.\vsl_clipon_binary_loader.h
# End Source File
# Begin Source File

SOURCE=.\vsl_complex_io.h
# End Source File
# Begin Source File

SOURCE=.\vsl_deque_io.h
# End Source File
# Begin Source File

SOURCE=.\vsl_indent.h
# End Source File
# Begin Source File

SOURCE=.\vsl_list_io.h
# End Source File
# Begin Source File

SOURCE=.\vsl_map_io.h
# End Source File
# Begin Source File

SOURCE=.\vsl_pair_io.h
# End Source File
# Begin Source File

SOURCE=.\vsl_set_io.h
# End Source File
# Begin Source File

SOURCE=.\vsl_stack_io.h
# End Source File
# Begin Source File

SOURCE=.\vsl_string_io.h
# End Source File
# Begin Source File

SOURCE=.\vsl_test.h
# End Source File
# Begin Source File

SOURCE=.\vsl_vector_io.h
# End Source File
# End Group
# End Target
# End Project
