# Microsoft Developer Studio Project File - Name="vnl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl"  == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vnl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vnl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vnl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vnl.mak" CFG="vnl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vnl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vnl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vnl - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vnl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vnl - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vnl_EXPORTS"
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

# Name "vnl - Win32 Release"
# Name "vnl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Templates/vcl_list+vnl_double_4-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_vector+vnl_double_3x4--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_vector+vnl_float_2--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_vector+vnl_sparse_matrix_pair+double---.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_vector+vnl_sparse_matrix_pair+float---.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_double_2-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_double_3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_double_3x3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_double_4-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_float_2-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_float_3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_int_3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_matrix+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_matrix+double-~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_matrix+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_matrix_fixed+double.3.4--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_matrix_fixed+float.3.4--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_real_npolynomial~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_sparse_matrix_pair+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_sparse_matrix_pair+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_vector+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_vector+double-~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_vector+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_vector+float-~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_vector+vcl_complex+double---.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_c_vector+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_bignum.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_c_vector+double_complex-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_c_vector+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_c_vector+float_complex-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_c_vector+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_c_vector+long-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_c_vector+long_double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_c_vector+long_double_complex-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_c_vector+schar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_c_vector+uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_c_vector+uint-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_c_vector+ulong-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_complex_ops+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_complex_ops+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_complex_ops+long_double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_diag_matrix+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_diag_matrix+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_diag_matrix+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_diag_matrix+long_double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_diag_matrix+vcl_complex+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_diag_matrix+vcl_complex+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_file_matrix+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_file_matrix+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_file_matrix+long_double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_file_vector+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_fortran_copy+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_fortran_copy+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_fortran_copy+long_double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_fortran_copy+vcl_complex+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_fortran_copy+vcl_complex+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_fortran_copy+vcl_complex+long_double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matlab_print+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matlab_print+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matlab_print+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matlab_print+long_double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matlab_print+vcl_complex+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matlab_print+vcl_complex+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matlab_print+vcl_complex+long_double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix+long-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix+long_double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix+schar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix+uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix+uint-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix+ulong-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix+vcl_complex+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix+vcl_complex+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix+vcl_complex+long_double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_exp+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed+double.2.2-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed+double.2.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed+double.2.6-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed+double.3.12-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed+double.3.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed+double.3.4-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed+double.4.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed+double.4.4-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed+float.3.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed_pairwise_ops.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed_ref+double.2.2-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed_ref+double.2.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed_ref+double.3.12-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed_ref+double.3.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed_ref+double.3.4-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed_ref+double.4.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed_ref+double.4.4-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_ref+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_ref+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_quaternion+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_sparse_matrix+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_sparse_matrix+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_sym_matrix+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_sym_matrix+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+vnl_rational.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+vnl_rational.2-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+vnl_rational.1-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+int.4-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+int.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+int.2-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+int.1-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+float.4-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+float.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+float.2-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+float.1-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+double.6-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+double.4-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+double.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+double.2-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+double.1-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+vnl_rational.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+vnl_rational.2-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+vnl_rational.1-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector+vnl_rational-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector+vcl_complex+vnl_rational--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_sparse_matrix+vnl_rational-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_ref+vnl_rational-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed_ref+vnl_rational.3.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed+vnl_rational.3.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix+vnl_rational-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix+vcl_complex+vnl_rational--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_diag_matrix+vnl_rational-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_det+vnl_rational-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_det+vcl_complex+long_double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_det+vcl_complex+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_det+vcl_complex+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_det+long_double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_det+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_det+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_c_vector+vnl_rational-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_c_vector+vcl_complex+vnl_rational--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_rational~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_rational-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_complex+vnl_rational-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+vnl_rational-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector+long-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector+long_double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector+schar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector+uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector+uint-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector+ulong-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector+vcl_complex+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector+vcl_complex+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector+vcl_complex+long_double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+double.1-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+double.2-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+double.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+double.4-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+double.6-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+float.1-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+float.2-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+float.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+float.4-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+int.1-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+int.2-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+int.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+int.4-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_ref+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_ref+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_alloc.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_block.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_complex.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_complex_traits.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_copy.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_cost_function.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_cross_product_matrix.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_double_2.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_double_2x3.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_double_3.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_double_3x2.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_double_4.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_error.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_fastops.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_float_2.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_float_3.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_float_4.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_identity_3x3.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_int_2.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_int_3.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_int_4.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_int_matrix.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_least_squares_cost_function.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_least_squares_function.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_linear_operators_3.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_linear_system.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_math.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_matlab_filewrite.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_matlab_header.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_matlab_print.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_matlab_print2.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_matlab_print_format.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_matlab_print_scalar.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_matlab_read.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_matlab_write.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_matops.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_nonlinear_minimizer.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_numeric_limits.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_numeric_traits.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_rational.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_real_npolynomial.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_real_polynomial.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_rotation_matrix.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_sample.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_sparse_matrix_linear_system.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_trace.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_transpose.cxx

# End Source File
# Begin Source File

SOURCE=.\vnl_unary_function.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+vnl_bignum-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_complex+vnl_bignum-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_bignum-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vnl_bignum~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_c_vector+vcl_complex+vnl_bignum--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_c_vector+vnl_bignum-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_det+vnl_bignum-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_diag_matrix+vnl_bignum-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix+vcl_complex+vnl_bignum--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix+vnl_bignum-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed+vnl_bignum.3.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_fixed_ref+vnl_bignum.3.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matrix_ref+vnl_bignum-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_sparse_matrix+vnl_bignum-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector+vcl_complex+vnl_bignum--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector+vnl_bignum-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+vnl_bignum.1-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+vnl_bignum.2-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed+vnl_bignum.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+vnl_bignum.1-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+vnl_bignum.2-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_vector_fixed_ref+vnl_bignum.3-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vnl_matlab_print+uint-.cxx

# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\vnl_alloc.h

# End Source File
# Begin Source File

SOURCE=.\vnl_block.h

# End Source File
# Begin Source File

SOURCE=.\vnl_bignum.h

# End Source File
# Begin Source File

SOURCE=.\vnl_c_vector.h

# End Source File
# Begin Source File

SOURCE=.\vnl_c_vector.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_complex.h

# End Source File
# Begin Source File

SOURCE=.\vnl_complex_ops.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_complex_traits.h

# End Source File
# Begin Source File

SOURCE=.\vnl_copy.h

# End Source File
# Begin Source File

SOURCE=.\vnl_cost_function.h

# End Source File
# Begin Source File

SOURCE=.\vnl_cross_product_matrix.h

# End Source File
# Begin Source File

SOURCE=.\vnl_det.h

# End Source File
# Begin Source File

SOURCE=.\vnl_det.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_diag_matrix.h

# End Source File
# Begin Source File

SOURCE=.\vnl_diag_matrix.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_double_2.h

# End Source File
# Begin Source File

SOURCE=.\vnl_double_2x3.h

# End Source File
# Begin Source File

SOURCE=.\vnl_double_3.h

# End Source File
# Begin Source File

SOURCE=.\vnl_double_3x2.h

# End Source File
# Begin Source File

SOURCE=.\vnl_double_4.h

# End Source File
# Begin Source File

SOURCE=.\vnl_error.h

# End Source File
# Begin Source File

SOURCE=.\vnl_fastops.h

# End Source File
# Begin Source File

SOURCE=.\vnl_file_matrix.h

# End Source File
# Begin Source File

SOURCE=.\vnl_file_matrix.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_file_vector.h

# End Source File
# Begin Source File

SOURCE=.\vnl_file_vector.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_float_2.h

# End Source File
# Begin Source File

SOURCE=.\vnl_float_3.h

# End Source File
# Begin Source File

SOURCE=.\vnl_float_4.h

# End Source File
# Begin Source File

SOURCE=.\vnl_fortran_copy.h

# End Source File
# Begin Source File

SOURCE=.\vnl_fortran_copy.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_identity_3x3.h

# End Source File
# Begin Source File

SOURCE=.\vnl_int_2.h

# End Source File
# Begin Source File

SOURCE=.\vnl_int_3.h

# End Source File
# Begin Source File

SOURCE=.\vnl_int_4.h

# End Source File
# Begin Source File

SOURCE=.\vnl_int_matrix.h

# End Source File
# Begin Source File

SOURCE=.\vnl_least_squares_cost_function.h

# End Source File
# Begin Source File

SOURCE=.\vnl_least_squares_function.h

# End Source File
# Begin Source File

SOURCE=.\vnl_linear_operators_3.h

# End Source File
# Begin Source File

SOURCE=.\vnl_linear_system.h

# End Source File
# Begin Source File

SOURCE=.\vnl_math.h

# End Source File
# Begin Source File

SOURCE=.\vnl_matlab_filewrite.h

# End Source File
# Begin Source File

SOURCE=.\vnl_matlab_header.h

# End Source File
# Begin Source File

SOURCE=.\vnl_matlab_print.h

# End Source File
# Begin Source File

SOURCE=.\vnl_matlab_print.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_matlab_print2.h

# End Source File
# Begin Source File

SOURCE=.\vnl_matlab_print_format.h

# End Source File
# Begin Source File

SOURCE=.\vnl_matlab_print_scalar.h

# End Source File
# Begin Source File

SOURCE=.\vnl_matlab_read.h

# End Source File
# Begin Source File

SOURCE=.\vnl_matlab_write.h

# End Source File
# Begin Source File

SOURCE=.\vnl_matops.h

# End Source File
# Begin Source File

SOURCE=.\vnl_matrix.h

# End Source File
# Begin Source File

SOURCE=.\vnl_matrix.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_matrix_exp.h

# End Source File
# Begin Source File

SOURCE=.\vnl_matrix_exp.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_matrix_fixed.h

# End Source File
# Begin Source File

SOURCE=.\vnl_matrix_fixed.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_matrix_fixed_ref.h

# End Source File
# Begin Source File

SOURCE=.\vnl_matrix_fixed_ref.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_nonlinear_minimizer.h

# End Source File
# Begin Source File

SOURCE=.\vnl_numeric_limits.h

# End Source File
# Begin Source File

SOURCE=.\vnl_numeric_traits.h

# End Source File
# Begin Source File

SOURCE=.\vnl_quaternion.h

# End Source File
# Begin Source File

SOURCE=.\vnl_quaternion.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_rational.h

# End Source File
# Begin Source File

SOURCE=.\vnl_real_npolynomial.h

# End Source File
# Begin Source File

SOURCE=.\vnl_real_polynomial.h

# End Source File
# Begin Source File

SOURCE=.\vnl_rotation_matrix.h

# End Source File
# Begin Source File

SOURCE=.\vnl_sample.h

# End Source File
# Begin Source File

SOURCE=.\vnl_scalar_join_iterator.h

# End Source File
# Begin Source File

SOURCE=.\vnl_scalar_join_iterator.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_sparse_matrix.h

# End Source File
# Begin Source File

SOURCE=.\vnl_sparse_matrix.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_sparse_matrix_linear_system.h

# End Source File
# Begin Source File

SOURCE=.\vnl_sym_matrix.h

# End Source File
# Begin Source File

SOURCE=.\vnl_sym_matrix.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_trace.h

# End Source File
# Begin Source File

SOURCE=.\vnl_transpose.h

# End Source File
# Begin Source File

SOURCE=.\vnl_unary_function.h

# End Source File
# Begin Source File

SOURCE=.\vnl_vector.h

# End Source File
# Begin Source File

SOURCE=.\vnl_vector.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_vector_fixed.h

# End Source File
# Begin Source File

SOURCE=.\vnl_vector_fixed.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_vector_fixed_ref.h

# End Source File
# Begin Source File

SOURCE=.\vnl_vector_fixed_ref.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_vector_ref.h

# End Source File
# Begin Source File

SOURCE=.\vnl_vector_ref.txx

# End Source File
# Begin Source File

SOURCE=.\vnl_T_n.h

# End Source File
# Begin Source File

SOURCE=.\vnl_config.h

# End Source File
# Begin Source File

SOURCE=.\vnl_double_2x2.h

# End Source File
# Begin Source File

SOURCE=.\vnl_double_3x3.h

# End Source File
# Begin Source File

SOURCE=.\vnl_double_3x4.h

# End Source File
# Begin Source File

SOURCE=.\vnl_double_4x3.h

# End Source File
# Begin Source File

SOURCE=.\vnl_double_4x4.h

# End Source File
# Begin Source File

SOURCE=.\vnl_double_5.h

# End Source File
# Begin Source File

SOURCE=.\vnl_fwd.h

# End Source File
# Begin Source File

SOURCE=.\vnl_identity.h

# End Source File
# Begin Source File

SOURCE=.\vnl_matrix_ref.h

# End Source File
# Begin Source File

SOURCE=.\vnl_operators.h

# End Source File
# Begin Source File

SOURCE=.\vnl_tag.h

# End Source File
# Begin Source File

SOURCE=.\vnl_vector_dereference.h

# End Source File
# Begin Source File

SOURCE=.\vnl_finite.h

# End Source File
# Begin Source File

SOURCE=.\vnl_complexify.h

# End Source File
# Begin Source File

SOURCE=.\vnl_imag.h

# End Source File
# Begin Source File

SOURCE=.\vnl_real.h

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

