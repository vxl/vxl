# Microsoft Developer Studio Project File - Name="vil" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vil - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vil.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vil.mak" CFG="vil - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vil - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vil - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vil - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(IUEROOT)\vxl\config.win32-VC60" /I "$(IUEROOT)/vxl" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VCL_USE_NATIVE_STL" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "vil - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(IUEROOT)\vxl\config.win32-VC60" /I "$(IUEROOT)/vxl" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"D:\target\target\vxl\Debug\vil.lib"

!ENDIF 

# Begin Target

# Name "vil - Win32 Release"
# Name "vil - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\vil_block_cache_image.cxx
# End Source File
# Begin Source File

SOURCE=.\file_formats\vil_bmp.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_convolve.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_convolve_simple+byte.byte.int.int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_convolve_simple+byte.float.byte.float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_convolve_simple+byte.float.float.float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_convolve_simple+byte.float.int.float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_convolve_simple+byte.float.short.float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_convolve_simple+byte.short.short.int-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vil_copy.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_crop.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_crop_image.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_file_format.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_file_image.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_image.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_image_impl.cxx
# End Source File
# Begin Source File

SOURCE=.\file_formats\vil_iris.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_load.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_memory_image.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_memory_image_impl.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_memory_image_of+bool-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_memory_image_of+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_memory_image_of+double_complex-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_memory_image_of+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_memory_image_of+float_complex-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_memory_image_of+rgb+byte--.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_memory_image_of+rgb_byte-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_memory_image_of+signed_char-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_memory_image_of+signed_int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_memory_image_of+signed_short-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_memory_image_of+unsigned_char-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_memory_image_of+unsigned_int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_memory_image_of+unsigned_short-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vil_memory_image_window.cxx
# End Source File
# Begin Source File

SOURCE=.\file_formats\vil_mit.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_new.cxx
# End Source File
# Begin Source File

SOURCE=.\file_formats\vil_pnm.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_rgb_byte.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_save.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_skip_image.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vil_ssd+byte.byte.int-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vil_stream.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_stream_core.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_stream_FILE_ptr.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_stream_fstream.cxx
# End Source File
# Begin Source File

SOURCE=.\vil_stream_section.cxx
# End Source File
# Begin Source File

SOURCE=.\file_formats\vil_viff.cxx
# End Source File
# Begin Source File

SOURCE=.\file_formats\vil_viff_support.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\vil_block_cache_image.h
# End Source File
# Begin Source File

SOURCE=.\file_formats\vil_bmp.h
# End Source File
# Begin Source File

SOURCE=.\vil_byte.h
# End Source File
# Begin Source File

SOURCE=.\vil_convolve.h
# End Source File
# Begin Source File

SOURCE=.\vil_copy.h
# End Source File
# Begin Source File

SOURCE=.\vil_crop.h
# End Source File
# Begin Source File

SOURCE=.\vil_crop_image.h
# End Source File
# Begin Source File

SOURCE=.\vil_file_format.h
# End Source File
# Begin Source File

SOURCE=.\vil_file_image.h
# End Source File
# Begin Source File

SOURCE=.\vil_fwd.h
# End Source File
# Begin Source File

SOURCE=.\vil_image.h
# End Source File
# Begin Source File

SOURCE=.\vil_image_impl.h
# End Source File
# Begin Source File

SOURCE=.\vil_ip_traits.h
# End Source File
# Begin Source File

SOURCE=.\file_formats\vil_iris.h
# End Source File
# Begin Source File

SOURCE=.\file_formats\vil_jpeg.h
# End Source File
# Begin Source File

SOURCE=.\vil_jpeglib.h
# End Source File
# Begin Source File

SOURCE=.\vil_load.h
# End Source File
# Begin Source File

SOURCE=.\vil_memory_image.h
# End Source File
# Begin Source File

SOURCE=.\vil_memory_image_impl.h
# End Source File
# Begin Source File

SOURCE=.\vil_memory_image_of.h
# End Source File
# Begin Source File

SOURCE=.\vil_memory_image_window.h
# End Source File
# Begin Source File

SOURCE=.\file_formats\vil_mit.h
# End Source File
# Begin Source File

SOURCE=.\vil_new.h
# End Source File
# Begin Source File

SOURCE=.\file_formats\vil_png.h
# End Source File
# Begin Source File

SOURCE=.\file_formats\vil_pnm.h
# End Source File
# Begin Source File

SOURCE=.\vil_rgb_byte.h
# End Source File
# Begin Source File

SOURCE=.\vil_save.h
# End Source File
# Begin Source File

SOURCE=.\vil_skip_image.h
# End Source File
# Begin Source File

SOURCE=.\vil_ssd.h
# End Source File
# Begin Source File

SOURCE=.\vil_stream.h
# End Source File
# Begin Source File

SOURCE=.\vil_stream_core.h
# End Source File
# Begin Source File

SOURCE=.\vil_stream_FILE_ptr.h
# End Source File
# Begin Source File

SOURCE=.\vil_stream_fstream.h
# End Source File
# Begin Source File

SOURCE=.\vil_stream_section.h
# End Source File
# Begin Source File

SOURCE=.\file_formats\vil_tiff.h
# End Source File
# Begin Source File

SOURCE=.\file_formats\vil_viff.h
# End Source File
# Begin Source File

SOURCE=.\file_formats\vil_viffheader.h
# End Source File
# End Group
# End Target
# End Project
