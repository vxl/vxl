# Microsoft Developer Studio Project File - Name="vnl_io" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vnl_io - Win32 StaticDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vnl_io.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vnl_io.mak" CFG="vnl_io - Win32 StaticDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vnl_io - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vnl_io - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vnl_io - Win32 StaticDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "vnl_io - Win32 StaticRelease" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vnl_io - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)" /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Release\vnl_io.lib"

!ELSEIF  "$(CFG)" == "vnl_io - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FD /GZ "
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Debug\vnl_io.lib"

!ELSEIF  "$(CFG)" == "vnl_io - Win32 StaticDebug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vnl_io___Win32_StaticDebug"
# PROP BASE Intermediate_Dir "vnl_io___Win32_StaticDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "StaticDebug"
# PROP Intermediate_Dir "StaticDebug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FD /GZ "
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ "
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\Debug\vnl_io.lib"
# ADD LIB32 /nologo /out:"..\..\StaticDebug\vnl_io.lib"

!ELSEIF  "$(CFG)" == "vnl_io - Win32 StaticRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vnl_io___Win32_StaticRelease"
# PROP BASE Intermediate_Dir "vnl_io___Win32_StaticRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "StaticRelease"
# PROP Intermediate_Dir "StaticRelease"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)" /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)" /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\Release\vnl_io.lib"
# ADD LIB32 /nologo /out:"..\..\StaticRelease\vnl_io.lib"

!ENDIF 

# Begin Target

# Name "vnl_io - Win32 Release"
# Name "vnl_io - Win32 Debug"
# Name "vnl_io - Win32 StaticDebug"
# Name "vnl_io - Win32 StaticRelease"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;txx"
# Begin Source File

SOURCE=".\Templates\vnl_io_diag_matrix+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_diag_matrix+double_complex-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_diag_matrix+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_diag_matrix+float_complex-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_diag_matrix+int-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vnl_io_diag_matrix.txx
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix+double_complex-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix+float_complex-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix+long-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix+schar-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix+uchar-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix+uint-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix+ulong-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vnl_io_matrix.txx
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix_fixed+double.2.2-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix_fixed+double.2.3-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix_fixed+double.2.6-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix_fixed+double.3.12-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix_fixed+double.3.3-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix_fixed+double.3.4-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix_fixed+double.4.3-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix_fixed+double.4.4-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_matrix_fixed+float.3.3-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vnl_io_matrix_fixed.txx
# End Source File
# Begin Source File

SOURCE=.\vnl_io_nonlinear_minimizer.cxx
# End Source File
# Begin Source File

SOURCE=.\vnl_io_real_npolynomial.cxx
# End Source File
# Begin Source File

SOURCE=.\vnl_io_real_polynomial.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_sparse_matrix+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_sparse_matrix+float-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vnl_io_sparse_matrix.txx
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_vcl_vector+vnl_vector+double--.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_vector+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_vector+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_vector+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_vector+long-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_vector+schar-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_vector+uchar-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_vector+uint-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vnl_io_vector+ulong-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vnl_io_vector.txx
# End Source File
# Begin Source File

SOURCE=.\vnl_io_vector_fixed.txx
# End Source File
# Begin Source File

SOURCE=.\Templates\vnl_io_vector_fixed_instances.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\vnl_io_diag_matrix.h
# End Source File
# Begin Source File

SOURCE=.\vnl_io_matrix.h
# End Source File
# Begin Source File

SOURCE=.\vnl_io_matrix_fixed.h
# End Source File
# Begin Source File

SOURCE=.\vnl_io_nonlinear_minimizer.h
# End Source File
# Begin Source File

SOURCE=.\vnl_io_real_npolynomial.h
# End Source File
# Begin Source File

SOURCE=.\vnl_io_real_polynomial.h
# End Source File
# Begin Source File

SOURCE=.\vnl_io_sparse_matrix.h
# End Source File
# Begin Source File

SOURCE=.\vnl_io_vector.h
# End Source File
# Begin Source File

SOURCE=.\vnl_io_vector_fixed.h
# End Source File
# End Group
# End Target
# End Project
