# Microsoft Developer Studio Project File - Name="vbl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vbl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vbl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vbl.mak" CFG="vbl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vbl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vbl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
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
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)\vxl\config.win32-VC60" /I "$(IUEROOT)/vxl" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VCL_USE_NATIVE_STL" /YX /FD /c
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
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(IUEROOT)\vxl\config.win32-VC60" /I "$(IUEROOT)/vxl" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Debug\vbl.lib"

!ENDIF 

# Begin Target

# Name "vbl - Win32 Release"
# Name "vbl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\vbl_arg.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_array_2d+double-.cxx"
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

SOURCE=".\Templates\vbl_array_3d+unsignedchar-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vbl_awk.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_base_array_3d.cxx
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

SOURCE=.\vbl_file.cxx
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

SOURCE=".\Templates\vbl_sparse_array+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array_2d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vbl_sparse_array_2d_base.cxx
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

SOURCE=.\vbl_timer.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_timestamp.cxx
# End Source File
# Begin Source File

SOURCE=.\vbl_user_info.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\vbl_arg.h
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

SOURCE=.\vbl_clamp.h
# End Source File
# Begin Source File

SOURCE=.\vbl_file.h
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

SOURCE=.\vbl_smart_ptr.h
# End Source File
# Begin Source File

SOURCE=.\vbl_sort.h
# End Source File
# Begin Source File

SOURCE=.\vbl_sparse_array.h
# End Source File
# Begin Source File

SOURCE=.\vbl_sparse_array_2d.h
# End Source File
# Begin Source File

SOURCE=.\vbl_sparse_array_2d_base.h
# End Source File
# Begin Source File

SOURCE=.\vbl_sparse_array_3d.h
# End Source File
# Begin Source File

SOURCE=.\vbl_sprintf.h
# End Source File
# Begin Source File

SOURCE=.\vbl_string.h
# End Source File
# Begin Source File

SOURCE=.\vbl_timer.h
# End Source File
# Begin Source File

SOURCE=.\vbl_timestamp.h
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
