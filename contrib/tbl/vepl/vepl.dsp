# Microsoft Developer Studio Project File - Name="vepl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vepl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vepl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vepl.mak" CFG="vepl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vepl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vepl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vepl - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\tbl" /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Release\vepl.lib"

!ELSEIF  "$(CFG)" == "vepl - Win32 Debug"

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
# ADD LIB32 /nologo /out:"..\Debug\vepl.lib"

!ENDIF 

# Begin Target

# Name "vepl - Win32 Release"
# Name "vepl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\vepl_threshold.cxx
# End Source File
# Begin Source File

SOURCE=.\vepl_add_random_noise.cxx
# End Source File
# Begin Source File

SOURCE=.\vepl_monadic.cxx
# End Source File
# Begin Source File

SOURCE=.\vepl_dyadic.cxx
# End Source File
# Begin Source File

SOURCE=.\vepl_erode_disk.cxx
# End Source File
# Begin Source File

SOURCE=.\vepl_dilate_disk.cxx
# End Source File
# Begin Source File

SOURCE=.\vepl_sobel.cxx
# End Source File
# Begin Source File

SOURCE=.\vepl_gaussian_convolution.cxx
# End Source File
# Begin Source File

SOURCE=.\vepl_x_gradient.cxx
# End Source File
# Begin Source File

SOURCE=.\vepl_y_gradient.cxx
# End Source File
# Begin Source File

SOURCE=.\vepl_gradient_mag.cxx
# End Source File
# Begin Source File

SOURCE=.\vepl_gradient_dir.cxx
# End Source File
# Begin Source File

SOURCE=.\vepl_median.cxx
# End Source File
# Begin Source File

SOURCE=.\vepl_moment.cxx
# End Source File
# Begin Source File

SOURCE=.\vepl_histogram.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vepl_convert+double-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vepl_convert+float-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vepl_convert+int-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vepl_convert+short-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vepl_convert+uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vepl_convert+vil_rgb+uchar--.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_add_random_noise+vil_image.vil_image.double.double-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_add_random_noise+vil_image.vil_image.float.float-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_add_random_noise+vil_image.vil_image.vil_rgb+uchar-.vil_rgb+uchar--.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.double.double-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.double.float-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.double.int-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.double.short-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.double.vil_rgb+uchar--.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.float.double-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.float.float-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.float.int-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.float.short-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.float.vil_rgb+uchar--.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.short.short-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.uchar.double-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.uchar.int-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.uchar.short-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.uchar.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.uchar.vil_rgb+uchar--.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.vil_rgb+uchar-.double-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.vil_rgb+uchar-.float-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.vil_rgb+uchar-.int-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.vil_rgb+uchar-.short-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.vil_rgb+uchar-.uchar-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vipl_filter+vil_image.vil_image.vil_rgb+uchar-.vil_rgb+uchar--.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\vepl_add_random_noise.h
# End Source File
# Begin Source File

SOURCE=.\vepl_convert.h
# End Source File
# Begin Source File

SOURCE=.\vepl_convert.txx
# End Source File
# Begin Source File

SOURCE=.\vepl_dilate_disk.h
# End Source File
# Begin Source File

SOURCE=.\vepl_dyadic.h
# End Source File
# Begin Source File

SOURCE=.\vepl_erode_disk.h
# End Source File
# Begin Source File

SOURCE=.\vepl_gaussian_convolution.h
# End Source File
# Begin Source File

SOURCE=.\vepl_gradient_dir.h
# End Source File
# Begin Source File

SOURCE=.\vepl_gradient_mag.h
# End Source File
# Begin Source File

SOURCE=.\vepl_histogram.h
# End Source File
# Begin Source File

SOURCE=.\vepl_median.h
# End Source File
# Begin Source File

SOURCE=.\vepl_moment.h
# End Source File
# Begin Source File

SOURCE=.\vepl_monadic.h
# End Source File
# Begin Source File

SOURCE=.\vepl_sobel.h
# End Source File
# Begin Source File

SOURCE=.\vepl_threshold.h
# End Source File
# Begin Source File

SOURCE=.\vepl_x_gradient.h
# End Source File
# Begin Source File

SOURCE=.\vepl_y_gradient.h
# End Source File
# End Group
# End Target
# End Project
