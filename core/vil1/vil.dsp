# Microsoft Developer Studio Project File - Name="vil" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/v3p/jpeg" /I "c:/awf/src/vxl/v3p/png" /I "c:/awf/src/vxl/v3p/zlib" /I "c:/awf/src/vxl/v3p/tiff"  == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF == compiler defines
#  == override in output directory
# vil  == name of output library

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
!MESSAGE "vil - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vil - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "vil - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vil - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
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
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/v3p/jpeg" /I "c:/awf/src/vxl/v3p/png" /I "c:/awf/src/vxl/v3p/zlib" /I "c:/awf/src/vxl/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "vil_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O2
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
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /GZ /c
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/v3p/jpeg" /I "c:/awf/src/vxl/v3p/png" /I "c:/awf/src/vxl/v3p/zlib" /I "c:/awf/src/vxl/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "vil_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ELSEIF  "$(CFG)" == "vil - Win32 MinSizeRel"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MinSizeRel"
# PROP BASE Intermediate_Dir "MinSizeRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinSizeRel"
# PROP Intermediate_Dir "MinSizeRel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "_MBCS" /D "_ATL_DLL"  /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FD /c
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/v3p/jpeg" /I "c:/awf/src/vxl/v3p/png" /I "c:/awf/src/vxl/v3p/zlib" /I "c:/awf/src/vxl/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "vil_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O1
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "vil - Win32 RelWithDebInfo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RelWithDebInfo"
# PROP BASE Intermediate_Dir "RelWithDebInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "RelWithDebInfo"
# PROP Intermediate_Dir "RelWithDebInfo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/v3p/jpeg" /I "c:/awf/src/vxl/v3p/png" /I "c:/awf/src/vxl/v3p/zlib" /I "c:/awf/src/vxl/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "vil_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /Zi /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "vil - Win32 Release"
# Name "vil - Win32 Debug"
# Name "vil - Win32 MinSizeRel"
# Name "vil - Win32 RelWithDebInfo"


# Begin Source File

SOURCE=".\CMakeLists.txt"

!IF  "$(CFG)" == "vil - Win32 Release"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/config.cmake/Modules/FindJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativePNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeZLIB.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindPNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindZLIB.cmake"\
	"c:/awf/src/vxl/v3p/jpeg/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/png/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/tiff/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/zlib/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/vil/CMakeLists.txt"
# Begin Custom Build

"vil.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/vxl/vil/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/vxl/vil" -O"c:/awf/src/vxl/vxl/vil" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "vil - Win32 Debug"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/config.cmake/Modules/FindJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativePNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeZLIB.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindPNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindZLIB.cmake"\
	"c:/awf/src/vxl/v3p/jpeg/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/png/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/tiff/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/zlib/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/vil/CMakeLists.txt"
# Begin Custom Build

"vil.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/vxl/vil/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/vxl/vil" -O"c:/awf/src/vxl/vxl/vil" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "vil - Win32 MinSizeRel"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/config.cmake/Modules/FindJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativePNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeZLIB.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindPNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindZLIB.cmake"\
	"c:/awf/src/vxl/v3p/jpeg/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/png/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/tiff/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/zlib/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/vil/CMakeLists.txt"
# Begin Custom Build

"vil.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/vxl/vil/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/vxl/vil" -O"c:/awf/src/vxl/vxl/vil" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "vil - Win32 RelWithDebInfo"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/config.cmake/Modules/FindJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativePNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeZLIB.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindPNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindZLIB.cmake"\
	"c:/awf/src/vxl/v3p/jpeg/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/png/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/tiff/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/zlib/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/vil/CMakeLists.txt"
# Begin Custom Build

"vil.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/vxl/vil/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/vxl/vil" -O"c:/awf/src/vxl/vxl/vil" -B"c:/awf/src/vxl"

# End Custom Build

!ENDIF

# End Source File
# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Templates/vcl_vector+vil_image-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vil_memory_image_of+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vil_memory_image_of+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vil_memory_image_of+uchar--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_clamp+vil_rgb+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_convolve_1d+float.float.float.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_convolve_1d+float.int.float.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_convolve_1d+float.uchar.float.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_convolve_signal.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_convolve_simple+vil_byte.double.double.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_convolve_simple+vil_byte.float.float.vil_byte-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_convolve_simple+vil_byte.float.vil_byte.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_convolve_simple+vil_byte.vil_byte.int.int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_interpolate+double.double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_interpolate+float.double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_interpolate+float.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_interpolate+uchar.double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_interpolate+uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_interpolate+vil_rgb+uchar-.vil_rgb+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_interpolate+vil_rgb+uchar-.vil_rgb+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_interpolate+vil_rgb+uchar-.vil_rgb+uchar--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_memory_image_of+bool-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_memory_image_of+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_memory_image_of+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_memory_image_of+signed_char-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_memory_image_of+signed_int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_memory_image_of+signed_short-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_memory_image_of+unsigned_char-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_memory_image_of+unsigned_int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_memory_image_of+unsigned_short-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_memory_image_of+vcl_complex+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_memory_image_of+vcl_complex+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_memory_image_of+vil_rgb+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_memory_image_of+vil_rgb+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_memory_image_of+vil_rgb+uchar--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_memory_image_of+vil_rgb+vxl_uint_16--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_ncc+float.float.double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_ncc+uchar.uchar.double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_resample_image+vil_byte.unsigned-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_resample_image+vil_rgb+vil_byte-.vil_rgb+int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_rgb+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_rgb+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_rgb+long-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_rgb+uint-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_rgb+unsignedchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_rgb+vxl_uint_16-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_rgba+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_rgba+unsignedchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_ssd+float.float.double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_ssd+uchar.uchar.int-.cxx

# End Source File
# Begin Source File

SOURCE=.\file_formats/vil_bmp.cxx

# End Source File
# Begin Source File

SOURCE=.\file_formats/vil_bmp_core_header.cxx

# End Source File
# Begin Source File

SOURCE=.\file_formats/vil_bmp_file_header.cxx

# End Source File
# Begin Source File

SOURCE=.\file_formats/vil_bmp_info_header.cxx

# End Source File
# Begin Source File

SOURCE=.\file_formats/vil_gen.cxx

# End Source File
# Begin Source File

SOURCE=.\file_formats/vil_gif.cxx

# End Source File
# Begin Source File

SOURCE=.\file_formats/vil_iris.cxx

# End Source File
# Begin Source File

SOURCE=.\file_formats/vil_jpeg.cxx

# End Source File
# Begin Source File

SOURCE=.\file_formats/vil_jpeg_compressor.cxx

# End Source File
# Begin Source File

SOURCE=.\file_formats/vil_jpeg_decompressor.cxx

# End Source File
# Begin Source File

SOURCE=.\file_formats/vil_jpeg_destination_mgr.cxx

# End Source File
# Begin Source File

SOURCE=.\file_formats/vil_jpeg_source_mgr.cxx

# End Source File
# Begin Source File

SOURCE=.\file_formats/vil_mit.cxx

# End Source File
# Begin Source File

SOURCE=.\file_formats/vil_png.cxx

# End Source File
# Begin Source File

SOURCE=.\file_formats/vil_pnm.cxx

# End Source File
# Begin Source File

SOURCE=.\file_formats/vil_tiff.cxx

# End Source File
# Begin Source File

SOURCE=.\file_formats/vil_viff.cxx

# End Source File
# Begin Source File

SOURCE=.\file_formats/vil_viff_support.c

# End Source File
# Begin Source File

SOURCE=.\vil_16bit.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_32bit.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_block_cache_image_impl.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_byte_swap.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_clamp.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_clamp_image.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_clamp_image_impl.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_colour_space.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_copy.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_crop.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_crop_image_impl.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_file_format.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_file_image.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_flipud.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_flipud_impl.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_image.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_image_as.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_image_impl.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_image_proxy.cxx

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

SOURCE=.\vil_memory_image_window.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_new.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_pixel.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_pyramid.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_resample.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_resample_image_impl.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_rgb_byte.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_save.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_scale_intensities.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_scale_intensities_image.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_scale_intensities_image_impl.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_skip.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_skip_image_impl.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_smooth.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_stream.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_stream_core.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_stream_fstream.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_stream_section.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_stream_url.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_test.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_warp.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

