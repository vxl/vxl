# Microsoft Developer Studio Project File - Name="clsfy" - Package Owner=<4>
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
# clsfy  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=clsfy - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "clsfy.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "clsfy.mak" CFG="clsfy - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "clsfy - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "clsfy - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "clsfy - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/mul"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "clsfy_EXPORTS"
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

!ELSEIF  "$(CFG)" == "clsfy - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/mul"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "clsfy_EXPORTS"
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

# Name "clsfy - Win32 Release"
# Name "clsfy - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\clsfy_binary_hyperplane.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_binary_hyperplane_ls_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_binary_pdf_classifier.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_binary_threshold_1d.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_builder_1d.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_builder_base.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_classifier_1d.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_classifier_base.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_k_nearest_neighbour.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_knn_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_parzen_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_random_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_random_classifier.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_rbf_parzen.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_simple_adaboost.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+clsfy_classifier_1d~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_binary_loader+clsfy_builder_base-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_binary_loader+clsfy_classifier_base-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_binary_loader+clsfy_builder_1d-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_binary_loader+clsfy_classifier_1d-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_vector_io+clsfy_classifier_1d~-.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_rbf_svm.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_rbf_svm_smo_1_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_smo_1.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_smo_base.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_adaboost_sorted_trainer.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_adaboost_trainer.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_binary_threshold_1d_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\clsfy_binary_threshold_1d_sorted_builder.cxx

# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\clsfy_binary_hyperplane.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_binary_hyperplane_ls_builder.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_binary_pdf_classifier.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_binary_threshold_1d.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_builder_1d.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_builder_base.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_classifier_1d.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_classifier_base.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_k_nearest_neighbour.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_knn_builder.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_parzen_builder.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_random_builder.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_random_classifier.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_rbf_parzen.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_simple_adaboost.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_rbf_svm.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_rbf_svm_smo_1_builder.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_smo_1.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_smo_base.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_adaboost_sorted_trainer.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_adaboost_trainer.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_binary_threshold_1d_builder.h

# End Source File
# Begin Source File

SOURCE=.\clsfy_binary_threshold_1d_sorted_builder.h

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
