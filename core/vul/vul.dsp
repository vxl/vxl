# Microsoft Developer Studio Project File - Name="vul" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vul - Win32 DebugSTLPort
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vul.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vul.mak" CFG="vul - Win32 DebugSTLPort"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vul - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vul - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vul - Win32 StaticDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "vul - Win32 StaticRelease" (based on "Win32 (x86) Static Library")
!MESSAGE "vul - Win32 ReleaseSTLPort" (based on "Win32 (x86) Static Library")
!MESSAGE "vul - Win32 DebugSTLPort" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/vxl-2d3/vxl/vul", YBECAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vul - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Release\vul.lib"

!ELSEIF  "$(CFG)" == "vul - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Debug\vul.lib"

!ELSEIF  "$(CFG)" == "vul - Win32 StaticDebug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /Ob2 /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\StaticDebug\vul.lib"

!ELSEIF  "$(CFG)" == "vul - Win32 StaticRelease"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\StaticRelease\vul.lib"

!ELSEIF  "$(CFG)" == "vul - Win32 ReleaseSTLPort"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseSTLPort"
# PROP BASE Intermediate_Dir "ReleaseSTLPort"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseSTLPort"
# PROP Intermediate_Dir "ReleaseSTLPort"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "$(STLPORT)\stlport" /I "$(IUEROOT)\vcl\config.stlport.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\ReleaseSTLPort\vul.lib"

!ELSEIF  "$(CFG)" == "vul - Win32 DebugSTLPort"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugSTLPort"
# PROP BASE Intermediate_Dir "DebugSTLPort"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugSTLPort"
# PROP Intermediate_Dir "DebugSTLPort"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /Ob2 /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(STLPORT)\stlport" /I "$(IUEROOT)\vcl\config.stlport.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\DebugSTLPort\vul.lib"

!ENDIF 

# Begin Target

# Name "vul - Win32 Release"
# Name "vul - Win32 Debug"
# Name "vul - Win32 StaticDebug"
# Name "vul - Win32 StaticRelease"
# Name "vul - Win32 ReleaseSTLPort"
# Name "vul - Win32 DebugSTLPort"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;txx"
# Begin Source File

SOURCE=.\vul_arg.cxx
# End Source File
# Begin Source File

SOURCE=.\vul_awk.cxx
# End Source File
# Begin Source File

SOURCE=.\vul_expand_path.cxx
# End Source File
# Begin Source File

SOURCE=.\vul_file.cxx
# End Source File
# Begin Source File

SOURCE=.\vul_file_iterator.cxx
# End Source File
# Begin Source File

SOURCE=.\vul_get_timestamp.cxx
# End Source File
# Begin Source File

SOURCE=.\vul_printf.cxx
# End Source File
# Begin Source File

SOURCE=.\vul_psfile.cxx
# End Source File
# Begin Source File

SOURCE=.\vul_redirector.cxx
# End Source File
# Begin Source File

SOURCE=.\vul_reg_exp.cxx
# End Source File
# Begin Source File

SOURCE=.\vul_sequence_filename_map.cxx
# End Source File
# Begin Source File

SOURCE=.\vul_sprintf.cxx
# End Source File
# Begin Source File

SOURCE=.\vul_string.cxx
# End Source File
# Begin Source File

SOURCE=.\vul_test.cxx
# End Source File
# Begin Source File

SOURCE=.\vul_timer.cxx
# End Source File
# Begin Source File

SOURCE=.\vul_timestamp.cxx
# End Source File
# Begin Source File

SOURCE=.\vul_trace.cxx
# End Source File
# Begin Source File

SOURCE=.\vul_user_info.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\vul_arg.h
# End Source File
# Begin Source File

SOURCE=.\vul_awk.h
# End Source File
# Begin Source File

SOURCE=.\vul_canonical_path.h
# End Source File
# Begin Source File

SOURCE=.\vul_deprecated.h
# End Source File
# Begin Source File

SOURCE=.\vul_expand_path.h
# End Source File
# Begin Source File

SOURCE=.\vul_file.h
# End Source File
# Begin Source File

SOURCE=.\vul_file_iterator.h
# End Source File
# Begin Source File

SOURCE=.\vul_fwd.h
# End Source File
# Begin Source File

SOURCE=.\vul_get_timestamp.h
# End Source File
# Begin Source File

SOURCE=.\vul_printf.h
# End Source File
# Begin Source File

SOURCE=.\vul_psfile.h
# End Source File
# Begin Source File

SOURCE=.\vul_redirector.h
# End Source File
# Begin Source File

SOURCE=.\vul_reg_exp.h
# End Source File
# Begin Source File

SOURCE=.\vul_sequence_filename_map.h
# End Source File
# Begin Source File

SOURCE=.\vul_sprintf.h
# End Source File
# Begin Source File

SOURCE=.\vul_string.h
# End Source File
# Begin Source File

SOURCE=.\vul_test.h
# End Source File
# Begin Source File

SOURCE=.\vul_timer.h
# End Source File
# Begin Source File

SOURCE=.\vul_timestamp.h
# End Source File
# Begin Source File

SOURCE=.\vul_trace.h
# End Source File
# Begin Source File

SOURCE=.\vul_user_info.h
# End Source File
# Begin Source File

SOURCE=.\vul_whereami.h
# End Source File
# End Group
# End Target
# End Project
