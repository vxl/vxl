# Microsoft Developer Studio Project File - Name="vnl_test_all" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=vnl_test_all - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vnl_test_all.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vnl_test_all.mak" CFG="vnl_test_all - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vnl_test_all - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "vnl_test_all - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vnl_test_all - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "$(VXLROOT)\vcl\config.win32-vc60" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vxl" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 /libpath:"$(VXLROOT)/vcl/$(OUTDIR)" vcl.lib

!ELSEIF  "$(CFG)" == "vnl_test_all - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob0 /I "$(VXLROOT)\vcl\config.win32-vc60" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vxl" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /libpath:"$(VXLROOT)/vcl/$(OUTDIR)" vcl.lib

!ENDIF 

# Begin Target

# Name "vnl_test_all - Win32 Release"
# Name "vnl_test_all - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File
SOURCE=.\test_driver.cxx
# End Source File
# Begin Source File
SOURCE=.\test_include.cxx
# End Source File
# Begin Source File
SOURCE=.\test_util.cxx
# End Source File
# Begin Source File
SOURCE=.\test_amoeba.cxx
# End Source File
# Begin Source File
SOURCE=.\test_bignum.cxx
# End Source File
# Begin Source File
SOURCE=.\test_complex.cxx
# End Source File
# Begin Source File
SOURCE=.\test_complex_eigensystem.cxx
# End Source File
# Begin Source File
SOURCE=.\test_convolve.cxx
# End Source File
# Begin Source File
SOURCE=.\test_cpoly_roots.cxx
# End Source File
# Begin Source File
SOURCE=.\test_determinant.cxx
# End Source File
# Begin Source File
SOURCE=.\test_fft.cxx
# End Source File
# Begin Source File
SOURCE=.\test_fft1d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_fft2d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_file_matrix.cxx
# End Source File
# Begin Source File
SOURCE=.\test_finite.cxx
# End Source File
# Begin Source File
SOURCE=.\test_functions.cxx
# End Source File
# Begin Source File
SOURCE=.\test_generalized_eigensystem.cxx
# End Source File
# Begin Source File
SOURCE=.\test_levenberg_marquardt.cxx
# End Source File
# Begin Source File
SOURCE=.\test_math.cxx
# End Source File
# Begin Source File
SOURCE=.\test_matlab.cxx
# End Source File
# Begin Source File
SOURCE=.\test_matrix.cxx
# End Source File
# Begin Source File
SOURCE=.\test_matrix_exp.cxx
# End Source File
# Begin Source File
SOURCE=.\test_matrix_fixed.cxx
# End Source File
# Begin Source File
SOURCE=.\test_minimizers.cxx
# End Source File
# Begin Source File
SOURCE=.\test_numeric_limits.cxx
# End Source File
# Begin Source File
SOURCE=.\test_numeric_traits.cxx
# End Source File
# Begin Source File
SOURCE=.\test_qr.cxx
# End Source File
# Begin Source File
SOURCE=.\test_qsvd.cxx
# End Source File
# Begin Source File
SOURCE=.\test_rational.cxx
# End Source File
# Begin Source File
SOURCE=.\test_real_eigensystem.cxx
# End Source File
# Begin Source File
SOURCE=.\test_real_polynomial.cxx
# End Source File
# Begin Source File
SOURCE=.\test_resize.cxx
# End Source File
# Begin Source File
SOURCE=.\test_rnpoly_roots.cxx
# End Source File
# Begin Source File
SOURCE=.\test_rpoly_roots.cxx
# End Source File
# Begin Source File
SOURCE=.\test_sample.cxx
# End Source File
# Begin Source File
SOURCE=.\test_diag_matrix.cxx
# End Source File
# Begin Source File
SOURCE=.\test_sparse_matrix.cxx
# End Source File
# Begin Source File
SOURCE=.\test_svd.cxx
# End Source File
# Begin Source File
SOURCE=.\test_sym_matrix.cxx
# End Source File
# Begin Source File
SOURCE=.\test_symmetric_eigensystem.cxx
# End Source File
# Begin Source File
SOURCE=.\test_transpose.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vector.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File
SOURCE=.\test_util.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
