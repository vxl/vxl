# Microsoft Developer Studio Project File - Name="pdf1d" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/mul"  == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# pdf1d  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=pdf1d - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pdf1d.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pdf1d.mak" CFG="pdf1d - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pdf1d - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "pdf1d - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pdf1d - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/mul"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "pdf1d_EXPORTS"
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

!ELSEIF  "$(CFG)" == "pdf1d - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/mul"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "pdf1d_EXPORTS"
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

# Name "pdf1d - Win32 Release"
# Name "pdf1d - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Templates/mbl_cloneable_ptr+pdf1d_builder-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mbl_cloneable_ptr+pdf1d_pdf-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+pdf1d_builder~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+pdf1d_pdf~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+pdf1d_sampler~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_binary_loader+pdf1d_builder-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_binary_loader+pdf1d_pdf-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_vector_io+pdf1d_builder~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_vector_io+pdf1d_pdf~-.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_bhat_overlap.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_calc_mean_var.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_compare_samples.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_compare_to_pdf.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_compare_to_pdf_bhat.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_compare_to_pdf_ks.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_epanech_kernel_pdf.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_epanech_kernel_pdf_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_epanech_kernel_pdf_sampler.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_exponential.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_exponential_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_exponential_sampler.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_flat.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_flat_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_flat_sampler.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_gaussian.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_gaussian_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_gaussian_kernel_pdf.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_gaussian_kernel_pdf_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_gaussian_kernel_pdf_sampler.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_gaussian_sampler.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_kernel_pdf.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_kernel_pdf_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_mixture.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_mixture_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_mixture_sampler.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_pdf.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_prob_chi2.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_prob_ks.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_resample.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_sampler.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_select_pdf.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_weighted_epanech_kernel_pdf.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_weighted_epanech_kernel_sampler.cxx

# End Source File
# Begin Source File

SOURCE=.\pdf1d_weighted_kernel_pdf.cxx

# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pdf1d_all.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_bhat_overlap.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_builder.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_calc_mean_var.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_compare_samples.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_compare_to_pdf.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_compare_to_pdf_bhat.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_compare_to_pdf_ks.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_epanech_kernel_pdf.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_epanech_kernel_pdf_builder.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_epanech_kernel_pdf_sampler.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_exponential.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_exponential_builder.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_exponential_sampler.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_flat.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_flat_builder.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_flat_sampler.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_gaussian.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_gaussian_builder.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_gaussian_kernel_pdf.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_gaussian_kernel_pdf_builder.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_gaussian_kernel_pdf_sampler.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_gaussian_sampler.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_kernel_pdf.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_kernel_pdf_builder.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_mixture.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_mixture_builder.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_mixture_sampler.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_pdf.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_prob_chi2.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_prob_ks.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_resample.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_sampler.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_select_pdf.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_weighted_epanech_kernel_pdf.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_weighted_epanech_kernel_sampler.h

# End Source File
# Begin Source File

SOURCE=.\pdf1d_weighted_kernel_pdf.h

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
