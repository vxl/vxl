# Microsoft Developer Studio Project File - Name="vbl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vbl - Win32 DebugSTLPort
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vbl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vbl.mak" CFG="vbl - Win32 DebugSTLPort"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vbl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vbl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vbl - Win32 StaticDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "vbl - Win32 StaticRelease" (based on "Win32 (x86) Static Library")
!MESSAGE "vbl - Win32 ReleaseSTLPort" (based on "Win32 (x86) Static Library")
!MESSAGE "vbl - Win32 DebugSTLPort" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/vxl-2d3/vxl/vbl", YBECAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vbl - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)" /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Release\vbl.lib"

!ELSEIF  "$(CFG)" == "vbl - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Debug\vbl.lib"

!ELSEIF  "$(CFG)" == "vbl - Win32 StaticDebug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vbl___Win32_StaticDebug"
# PROP BASE Intermediate_Dir "vbl___Win32_StaticDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "StaticDebug"
# PROP Intermediate_Dir "StaticDebug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Debug\vbl.lib"
# ADD LIB32 /nologo /out:"..\StaticDebug\vbl.lib"

!ELSEIF  "$(CFG)" == "vbl - Win32 StaticRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vbl___Win32_StaticRelease"
# PROP BASE Intermediate_Dir "vbl___Win32_StaticRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "StaticRelease"
# PROP Intermediate_Dir "StaticRelease"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)" /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)" /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Release\vbl.lib"
# ADD LIB32 /nologo /out:"..\StaticRelease\vbl.lib"

!ELSEIF  "$(CFG)" == "vbl - Win32 ReleaseSTLPort"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vbl___Win32_ReleaseSTLPort"
# PROP BASE Intermediate_Dir "vbl___Win32_ReleaseSTLPort"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release-STLPort"
# PROP Intermediate_Dir "Release-STLPort"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)" /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(STLPORT)\stlport" /I "$(IUEROOT)\vcl\config.stlport.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Release\vbl.lib"
# ADD LIB32 /nologo /out:"..\Release-STLPort\vbl.lib"

!ELSEIF  "$(CFG)" == "vbl - Win32 DebugSTLPort"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vbl___Win32_DebugSTLPort"
# PROP BASE Intermediate_Dir "vbl___Win32_DebugSTLPort"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug-STLPort"
# PROP Intermediate_Dir "Debug-STLPort"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(STLPORT)\stlport" /I "$(IUEROOT)\vcl\config.stlport.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Debug\vbl.lib"
# ADD LIB32 /nologo /out:"..\Debug-STLPort\vbl.lib"

!ENDIF 

# Begin Target

# Name "vbl - Win32 Release"
# Name "vbl - Win32 Debug"
# Name "vbl - Win32 StaticDebug"
# Name "vbl - Win32 StaticRelease"
# Name "vbl - Win32 ReleaseSTLPort"
# Name "vbl - Win32 DebugSTLPort"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;txx"
# Begin Source File

SOURCE=.\vbl_arg.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_array_1d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_array_1d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_array_2d+bool-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_array_2d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_array_2d+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_array_2d+unsigned-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_array_2d+unsignedchar-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_array_3d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_array_3d+int-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vbl_awk.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_binary+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_binary+int-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vbl_bool_ostream.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_bounding_box+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_bounding_box+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_bounding_box+int-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vbl_canonical_path.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_file.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_file_iterator.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_get_timestamp.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_printf.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_psfile.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_qsort.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_redirector.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_ref_count.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_reg_exp.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_sequence_filename_map.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_sort.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array_1d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array_1d+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array_2d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array_3d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array_3d+void~-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vbl_sprintf.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_string.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_test.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_timer.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_timestamp.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_trace.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_user_info.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\dll.h
# End Source File
# Begin Source File

SOURCE=.\vbl_arg.h
# End Source File
# Begin Source File

SOURCE=.\vbl_array_1d.h
# End Source File
# Begin Source File

SOURCE=.\vbl_array_2d.h
# End Source File
# Begin Source File

SOURCE=.\vbl_array_3d.h
# End Source File
# Begin Source File

SOURCE=.\vbl_awk.h
# End Source File
# Begin Source File

SOURCE=.\vbl_base_array_3d.h
# End Source File
# Begin Source File

SOURCE=.\vbl_basic_optional.h
# End Source File
# Begin Source File

SOURCE=.\vbl_basic_relation.h
# End Source File
# Begin Source File

SOURCE=.\vbl_basic_relation_type.h
# End Source File
# Begin Source File

SOURCE=.\vbl_basic_relation_where.h
# End Source File
# Begin Source File

SOURCE=.\vbl_basic_tuple.h
# End Source File
# Begin Source File

SOURCE=.\vbl_bool_ostream.h
# End Source File
# Begin Source File

SOURCE=.\vbl_bounding_box.h
# End Source File
# Begin Source File

SOURCE=.\vbl_br_default.h
# End Source File
# Begin Source File

SOURCE=.\vbl_br_impl.h
# End Source File
# Begin Source File

SOURCE=.\vbl_canonical_path.h
# End Source File
# Begin Source File

SOURCE=.\vbl_clamp.h
# End Source File
# Begin Source File

SOURCE=.\vbl_file.h
# End Source File
# Begin Source File

SOURCE=.\vbl_file_iterator.h
# End Source File
# Begin Source File

SOURCE=.\vbl_fwd.h
# End Source File
# Begin Source File

SOURCE=.\vbl_get_timestamp.h
# End Source File
# Begin Source File

SOURCE=.\vbl_hash_map.h
# End Source File
# Begin Source File

SOURCE=.\vbl_printf.h
# End Source File
# Begin Source File

SOURCE=.\vbl_protection_traits.h
# End Source File
# Begin Source File

SOURCE=.\vbl_psfile.h
# End Source File
# Begin Source File

SOURCE=.\vbl_qsort.h
# End Source File
# Begin Source File

SOURCE=.\vbl_redirector.h
# End Source File
# Begin Source File

SOURCE=.\vbl_ref_count.h
# End Source File
# Begin Source File

SOURCE=.\vbl_reg_exp.h
# End Source File
# Begin Source File

SOURCE=.\vbl_rgb.h
# End Source File
# Begin Source File

SOURCE=.\vbl_rgba.h
# End Source File
# Begin Source File

SOURCE=.\vbl_sequence_filename_map.h
# End Source File
# Begin Source File

SOURCE=.\vbl_smart_ptr.h
# End Source File
# Begin Source File

SOURCE=.\vbl_sort.h
# End Source File
# Begin Source File

SOURCE=.\vbl_sparse_array_1d.h
# End Source File
# Begin Source File

SOURCE=.\vbl_sparse_array_2d.h
# End Source File
# Begin Source File

SOURCE=.\vbl_sparse_array_3d.h
# End Source File
# Begin Source File

SOURCE=.\vbl_sparse_array_base.h
# End Source File
# Begin Source File

SOURCE=.\vbl_sprintf.h
# End Source File
# Begin Source File

SOURCE=.\vbl_string.h
# End Source File
# Begin Source File

SOURCE=.\vbl_test.h
# End Source File
# Begin Source File

SOURCE=.\vbl_timer.h
# End Source File
# Begin Source File

SOURCE=.\vbl_timestamp.h
# End Source File
# Begin Source File

SOURCE=.\vbl_trace.h
# End Source File
# Begin Source File

SOURCE=.\vbl_types.h
# End Source File
# Begin Source File

SOURCE=.\vbl_user_info.h
# End Source File
# Begin Source File

SOURCE=.\vbl_whereami.h
# End Source File
# End Group
# End Target
# End Project
