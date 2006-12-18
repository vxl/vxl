# Microsoft Developer Studio Project File - Name="vipl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/tbl" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vipl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vipl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vipl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vipl.mak" CFG="vipl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vipl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vipl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vipl - Win32 Release"

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
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/tbl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vipl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vipl - Win32 Debug"

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
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /GZ /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/tbl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vipl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ENDIF 

# Begin Target

# Name "vipl - Win32 Release"
# Name "vipl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\filter/vipl_filter_abs.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_accessors_vcl_vector+uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter_2d+vil1_image.vcl_vector+uint-.uchar.uint.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_histogram+vil1_image.vcl_vector+uint-.uchar.uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vnl_matrix/Templates/vipl_accessors_vnl_vector+uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vnl_matrix/Templates/vipl_filter_2d+vil1_image.vnl_vector+uint-.uchar.uint.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vnl_matrix/Templates/vipl_histogram+vil1_image.vnl_vector+uint-.uchar.uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_accessors_vil1_image+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_accessors_vil1_image+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_accessors_vil1_image+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_accessors_vil1_image+short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_accessors_vil1_image+uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_accessors_vil1_image+ushort-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_accessors_vil1_image+vil1_rgb+uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_add_random_noise+vil1_image.vil1_image.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.double.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.float.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.int.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.short.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.vil1_rgb+uchar-.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_dilate_disk+vil1_image.vil1_image.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_dyadic+vil1_image.vil1_image.float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_erode_disk+vil1_image.vil1_image.float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_erode_disk+vil1_image.vil1_image.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_erode_disk+vil1_image.vil1_image.ushort.ushort-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter_2d+vil1_image.vil1_image.double.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter_2d+vil1_image.vil1_image.float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter_2d+vil1_image.vil1_image.float.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter_2d+vil1_image.vil1_image.int.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter_2d+vil1_image.vil1_image.short.short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter_2d+vil1_image.vil1_image.short.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter_2d+vil1_image.vil1_image.uchar.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter_2d+vil1_image.vil1_image.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter_2d+vil1_image.vil1_image.ushort.ushort-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter_2d+vil1_image.vil1_image.vil1_rgb+uchar-.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_gradient_mag+vil1_image.vil1_image.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_median+vil1_image.vil1_image.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_moment+vil1_image.vil1_image.uchar.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_monadic+vil1_image.vil1_image.float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_container+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_container+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_container+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_container+short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_container+uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_container+ushort-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_container+vil1_rgb+uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_descriptor+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_descriptor+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_descriptor+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_descriptor+short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_descriptor+uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_descriptor+ushort-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_descriptor+vil1_rgb+uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_iterator+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_iterator+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_iterator+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_iterator+short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_iterator+uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_iterator+ushort-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_section_iterator+vil1_rgb+uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_sobel+vil1_image.vil1_image.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_threshold+vil1_image.vil1_image.float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_threshold+vil1_image.vil1_image.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_threshold+vil1_image.vil1_image.ushort.ushort-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_x_gradient+vil1_image.vil1_image.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_section/Templates/vipl_accessors_section+int.2-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_section/Templates/vipl_accessors_section+uchar.2-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_section/Templates/vipl_accessors_section+vil1_rgb+uchar-.2-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_section/Templates/vipl_erode_disk+section+uchar.2-.section+uchar.2-.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_section/Templates/vipl_filter_2d+section+uchar.2-.section+int.2-.uchar.int-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_section/Templates/vipl_filter_2d+section+uchar.2-.section+uchar.2-.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_section/Templates/vipl_filter_2d+section+vil1_rgb+uchar-.2-.section+vil1_rgb+uchar-.2-.vil1_rgb+uchar-.vil1_rgb+uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_section/Templates/vipl_histogram+section+uchar.2-.section+int.2-.uchar.int-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_section/Templates/vipl_threshold+section+uchar.2-.section+uchar.2-.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_section/Templates/vipl_x_gradient+section+vil1_rgb+uchar-.2-.section+vil1_rgb+uchar-.2-.vil1_rgb+uchar-.vil1_rgb+uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vbl_array_2d/Templates/vipl_accessors_vbl_array_2d+uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vbl_array_2d/Templates/vipl_filter_2d+vbl_array_2d+uchar-.vbl_array_2d+uchar-.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vbl_array_2d/Templates/vipl_gradient_mag+vbl_array_2d+uchar-.vbl_array_2d+uchar-.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vbl_array_2d/Templates/vipl_sobel+vbl_array_2d+uchar-.vbl_array_2d+uchar-.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vnl_matrix/Templates/vipl_accessors_vnl_matrix+uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vnl_matrix/Templates/vipl_filter_2d+vnl_matrix+uchar-.vnl_matrix+uchar-.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vnl_matrix/Templates/vipl_median+vnl_matrix+uchar-.vnl_matrix+uchar-.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vnl_matrix/Templates/vipl_threshold+vnl_matrix+uchar-.vnl_matrix+uchar-.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vnl_matrix/Templates/vipl_gaussian_convolution+vnl_matrix+uchar-.vnl_matrix+uchar-.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_section/Templates/vipl_filter_2d+section+uchar.2-.vcl_vector+uint-.uchar.uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_section/Templates/vipl_histogram+section+uchar.2-.vcl_vector+uint-.uchar.uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_add_random_noise+vil1_image.vil1_image.double.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_add_random_noise+vil1_image.vil1_image.float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.double.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.double.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.double.int--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.double.short--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.double.vil1_rgb+uchar---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.float.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.float.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.float.int--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.float.short--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.float.vil1_rgb+uchar---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.uchar.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.uchar.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.uchar.int--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.uchar.short--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.uchar.vil1_rgb+uchar---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.vil1_rgb+uchar-.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.vil1_rgb+uchar-.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.vil1_rgb+uchar-.int--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.vil1_rgb+uchar-.short--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_convert+vil1_image.vil1_image.vil1_rgb+uchar-.vil1_rgb+uchar---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_dilate_disk+vil1_image.vil1_image.double.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_dilate_disk+vil1_image.vil1_image.float.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_dyadic+vil1_image.vil1_image.double.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_dyadic+vil1_image.vil1_image.uchar.uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_dyadic+vil1_image.vil1_image.vil1_rgb+uchar-.vil1_rgb+uchar---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_erode_disk+vil1_image.vil1_image.double.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.double.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.double.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.double.int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.double.short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.double.vil1_rgb+uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.float.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.float.int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.float.short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.float.vil1_rgb+uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.short.short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.uchar.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.uchar.int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.uchar.short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.uchar.vil1_rgb+uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.vil1_rgb+uchar-.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.vil1_rgb+uchar-.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.vil1_rgb+uchar-.int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.vil1_rgb+uchar-.short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.vil1_rgb+uchar-.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter+vil1_image.vil1_image.vil1_rgb+uchar-.vil1_rgb+uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter_2d+vil1_image.vcl_vector+uint-.double.uint.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter_2d+vil1_image.vcl_vector+uint-.float.uint.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_filter_2d+vil1_image.vcl_vector+uint-.ushort.uint.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_gaussian_convolution+vil1_image.vil1_image.double.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_gaussian_convolution+vil1_image.vil1_image.float.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_gaussian_convolution+vil1_image.vil1_image.uchar.uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_gradient_dir+vil1_image.vil1_image.double.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_gradient_dir+vil1_image.vil1_image.float.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_gradient_dir+vil1_image.vil1_image.uchar.uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_gradient_mag+vil1_image.vil1_image.double.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_gradient_mag+vil1_image.vil1_image.float.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_histogram+vil1_image.vcl_vector+uint-.double.uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_histogram+vil1_image.vcl_vector+uint-.float.uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_histogram+vil1_image.vcl_vector+uint-.ushort.uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_histogram+vil1_image.vil1_image.double.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_histogram+vil1_image.vil1_image.float.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_histogram+vil1_image.vil1_image.uchar.uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_median+vil1_image.vil1_image.double.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_median+vil1_image.vil1_image.float.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_moment+vil1_image.vil1_image.double.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_moment+vil1_image.vil1_image.float.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_moment+vil1_image.vil1_image.uchar.uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_monadic+vil1_image.vil1_image.double.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_monadic+vil1_image.vil1_image.uchar.uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_sobel+vil1_image.vil1_image.double.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_sobel+vil1_image.vil1_image.float.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_sobel+vil1_image.vil1_image.vil1_rgb+uchar-.vil1_rgb+uchar---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_threshold+vil1_image.vil1_image.double.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_x_gradient+vil1_image.vil1_image.double.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_x_gradient+vil1_image.vil1_image.float.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_x_gradient+vil1_image.vil1_image.vil1_rgb+uchar-.vil1_rgb+uchar---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_y_gradient+vil1_image.vil1_image.double.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_y_gradient+vil1_image.vil1_image.float.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_y_gradient+vil1_image.vil1_image.uchar.uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_y_gradient+vil1_image.vil1_image.vil1_rgb+uchar-.vil1_rgb+uchar---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_gaussian_convolution+vil_image_view+double-.vil_image_view+double-.double.double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_gaussian_convolution+vil_image_view+float-.vil_image_view+float-.float.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vipl_gaussian_convolution+vil_image_view+uchar-.vil_image_view+uchar-.uchar.uchar--.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\accessors/vipl_accessors_vil_image_view.h
# End Source File
# Begin Source File
SOURCE=.\accessors/vipl_accessors_vil_image_view.txx
# End Source File
# Begin Source File
SOURCE=.\accessors/vipl_accessors_vil1_image.h
# End Source File
# Begin Source File
SOURCE=.\accessors/vipl_accessors_vil1_image.txx
# End Source File
# Begin Source File
SOURCE=.\filter/vipl_filter.h
# End Source File
# Begin Source File
SOURCE=.\filter/vipl_filter.txx
# End Source File
# Begin Source File
SOURCE=.\filter/vipl_filter_2d.h
# End Source File
# Begin Source File
SOURCE=.\filter/vipl_filter_2d.txx
# End Source File
# Begin Source File
SOURCE=.\filter/vipl_filter_abs.h
# End Source File
# Begin Source File
SOURCE=.\section/vipl_section_container.h
# End Source File
# Begin Source File
SOURCE=.\section/vipl_section_container.txx
# End Source File
# Begin Source File
SOURCE=.\section/vipl_section_descriptor.h
# End Source File
# Begin Source File
SOURCE=.\section/vipl_section_descriptor.txx
# End Source File
# Begin Source File
SOURCE=.\section/vipl_section_descriptor_2d.h
# End Source File
# Begin Source File
SOURCE=.\section/vipl_section_descriptor_2d.txx
# End Source File
# Begin Source File
SOURCE=.\section/vipl_section_iterator.h
# End Source File
# Begin Source File
SOURCE=.\section/vipl_section_iterator.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_add_random_noise.h
# End Source File
# Begin Source File
SOURCE=.\vipl_add_random_noise.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_convert.h
# End Source File
# Begin Source File
SOURCE=.\vipl_convert.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_dilate_disk.h
# End Source File
# Begin Source File
SOURCE=.\vipl_dilate_disk.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_dyadic.h
# End Source File
# Begin Source File
SOURCE=.\vipl_dyadic.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_erode_disk.h
# End Source File
# Begin Source File
SOURCE=.\vipl_erode_disk.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_gaussian_convolution.h
# End Source File
# Begin Source File
SOURCE=.\vipl_gaussian_convolution.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_gradient_dir.h
# End Source File
# Begin Source File
SOURCE=.\vipl_gradient_dir.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_gradient_mag.h
# End Source File
# Begin Source File
SOURCE=.\vipl_gradient_mag.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_histogram.h
# End Source File
# Begin Source File
SOURCE=.\vipl_histogram.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_median.h
# End Source File
# Begin Source File
SOURCE=.\vipl_median.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_moment.h
# End Source File
# Begin Source File
SOURCE=.\vipl_moment.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_monadic.h
# End Source File
# Begin Source File
SOURCE=.\vipl_monadic.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_sobel.h
# End Source File
# Begin Source File
SOURCE=.\vipl_sobel.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_threshold.h
# End Source File
# Begin Source File
SOURCE=.\vipl_threshold.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_section/accessors/vipl_accessors_section.h
# End Source File
# Begin Source File
SOURCE=.\vipl_with_section/accessors/vipl_accessors_section.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vbl_array_2d/accessors/vipl_accessors_vbl_array_2d.h
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vbl_array_2d/accessors/vipl_accessors_vbl_array_2d.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_matrix.h
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_matrix.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_x_gradient.h
# End Source File
# Begin Source File
SOURCE=.\vipl_x_gradient.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_y_gradient.h
# End Source File
# Begin Source File
SOURCE=.\vipl_y_gradient.txx
# End Source File
# Begin Source File
SOURCE=.\filter/vipl_filter_helper.h
# End Source File
# Begin Source File
SOURCE=.\filter/vipl_trivial_pixeliter.h
# End Source File
# Begin Source File
SOURCE=.\section/vipl_filterable_section_container_generator_vil_image_view.txx
# End Source File
# Begin Source File
SOURCE=.\section/vipl_filterable_section_container_generator_vil1_image.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_section/vipl_filterable_section_container_generator_section.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vbl_array_2d/vipl_filterable_section_container_generator_vbl_array_2d.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vnl_matrix/vipl_filterable_section_container_generator_vnl_matrix.txx
# End Source File
# Begin Source File
SOURCE=.\accessors/vipl_accessors_vcl_vector.h
# End Source File
# Begin Source File
SOURCE=.\accessors/vipl_accessors_vcl_vector.txx
# End Source File
# Begin Source File
SOURCE=.\section/vipl_filterable_section_container_generator_vcl_vector.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_vector.h
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_vector.txx
# End Source File
# Begin Source File
SOURCE=.\vipl_with_vnl_matrix/vipl_filterable_section_container_generator_vnl_vector.txx
# End Source File
# End Group
# End Target
# End Project
