# Microsoft Developer Studio Project File - Name="vipl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

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
!MESSAGE "vipl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vipl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\tbl" /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Release\vipl.lib"

!ELSEIF  "$(CFG)" == "vipl - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\tbl" /D "WIN32" /D "_DEBUG" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Debug\vipl.lib"

!ENDIF 

# Begin Target

# Name "vipl - Win32 Release"
# Name "vipl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Templates\vipl_accessors_vil_image+double-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_accessors_vil_image+float-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_accessors_vil_image+int-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_accessors_vil_image+short-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_accessors_vil_image+uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_accessors_vil_image+ushort-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_add_random_noise+vil_image.vil_image.uchar.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_convert+vil_image.vil_image.double.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_convert+vil_image.vil_image.float.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_convert+vil_image.vil_image.int.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_convert+vil_image.vil_image.short.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_convert+vil_image.vil_image.uchar.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_convert+vil_image.vil_image.vil_rgb+uchar-.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_dilate_disk+vil_image.vil_image.uchar.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_dyadic+vil_image.vil_image.float.float-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_erode_disk+vil_image.vil_image.uchar.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter_2d+vil_image.vil_image.double.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter_2d+vil_image.vil_image.float.float-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter_2d+vil_image.vil_image.float.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter_2d+vil_image.vil_image.int.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter_2d+vil_image.vil_image.short.short-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter_2d+vil_image.vil_image.short.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter_2d+vil_image.vil_image.uchar.float-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter_2d+vil_image.vil_image.uchar.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter_2d+vil_image.vil_image.ushort.ushort-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter_2d+vil_image.vil_image.vil_rgb+uchar-.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter_abs.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_gradient_mag+vil_image.vil_image.uchar.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_median+vil_image.vil_image.uchar.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_moment+vil_image.vil_image.uchar.float-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_monadic+vil_image.vil_image.float.float-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_container+double-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_container+float-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_container+int-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_container+short-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_container+uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_container+ushort-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_container+vil_rgb+uchar--.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_descriptor+double-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_descriptor+float-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_descriptor+int-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_descriptor+short-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_descriptor+uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_descriptor+ushort-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_descriptor+vil_rgb+uchar--.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_iterator+double-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_iterator+float-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_iterator+int-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_iterator+short-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_iterator+uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_iterator+ushort-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_section_iterator+vil_rgb+uchar--.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_sobel+vil_image.vil_image.uchar.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_threshold+vil_image.vil_image.float.float-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_threshold+vil_image.vil_image.uchar.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_threshold+vil_image.vil_image.ushort.ushort-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_x_gradient+vil_image.vil_image.uchar.uchar-.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\vipl_add_random_noise.txx
# End Source File
# Begin Source File

SOURCE=.\vipl_convert.txx
# End Source File
# Begin Source File

SOURCE=.\vipl_dilate_disk.txx
# End Source File
# Begin Source File

SOURCE=.\vipl_dyadic.txx
# End Source File
# Begin Source File

SOURCE=.\vipl_erode_disk.txx
# End Source File
# Begin Source File

SOURCE=.\vipl_gaussian_convolution.txx
# End Source File
# Begin Source File

SOURCE=.\vipl_gradient_dir.txx
# End Source File
# Begin Source File

SOURCE=.\vipl_gradient_mag.txx
# End Source File
# Begin Source File

SOURCE=.\vipl_histogram.txx
# End Source File
# Begin Source File

SOURCE=.\vipl_median.txx
# End Source File
# Begin Source File

SOURCE=.\vipl_moment.txx
# End Source File
# Begin Source File

SOURCE=.\vipl_monadic.txx
# End Source File
# Begin Source File

SOURCE=.\vipl_sobel.txx
# End Source File
# Begin Source File

SOURCE=.\vipl_threshold.txx
# End Source File
# Begin Source File

SOURCE=.\vipl_x_gradient.txx
# End Source File
# Begin Source File

SOURCE=.\vipl_y_gradient.txx
# End Source File
# End Group
# End Target
# End Project
