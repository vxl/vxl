# Microsoft Developer Studio Project File - Name="vdgl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vdgl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vdgl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vdgl.mak" CFG="vdgl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vdgl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vdgl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vdgl - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "vdgl - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\gel" /D "WIN32" /D "_DEBUG" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "vdgl - Win32 Release"
# Name "vdgl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=".\Templates\vbl_smart_ptr+vdgl_edgel_chain-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_smart_ptr+vdgl_interpolator-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+vdgl_edgel-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vdgl_digital_curve.cxx
# End Source File
# Begin Source File

SOURCE=.\vdgl_edgel.cxx
# End Source File
# Begin Source File

SOURCE=.\vdgl_edgel_chain.cxx
# End Source File
# Begin Source File

SOURCE=.\vdgl_fit_line.cxx
# End Source File
# Begin Source File

SOURCE=.\vdgl_interpolator.cxx
# End Source File
# Begin Source File

SOURCE=.\vdgl_interpolator_cubic.cxx
# End Source File
# Begin Source File

SOURCE=.\vdgl_interpolator_linear.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\vdgl_digital_curve.h
# End Source File
# Begin Source File

SOURCE=.\vdgl_edgel.h
# End Source File
# Begin Source File

SOURCE=.\vdgl_edgel_chain.h
# End Source File
# Begin Source File

SOURCE=.\vdgl_edgel_chain_sptr.h
# End Source File
# Begin Source File

SOURCE=.\vdgl_fit_line.h
# End Source File
# Begin Source File

SOURCE=.\vdgl_interpolator.h
# End Source File
# Begin Source File

SOURCE=.\vdgl_interpolator_cubic.h
# End Source File
# Begin Source File

SOURCE=.\vdgl_interpolator_linear.h
# End Source File
# Begin Source File

SOURCE=.\vdgl_interpolator_sptr.h
# End Source File
# End Group
# End Target
# End Project
