# Microsoft Developer Studio Project File - Name="vpdfl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vpdfl - Win32 StaticDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vpdfl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vpdfl.mak" CFG="vpdfl - Win32 StaticDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vpdfl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vpdfl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vpdfl - Win32 StaticDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "vpdfl - Win32 StaticRelease" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vpdfl - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\mul" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Release\vpdfl.lib"

!ELSEIF  "$(CFG)" == "vpdfl - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\mul" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Debug\vpdfl.lib"

!ELSEIF  "$(CFG)" == "vpdfl - Win32 StaticDebug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vpdfl___Win32_StaticDebug"
# PROP BASE Intermediate_Dir "vpdfl___Win32_StaticDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "StaticDebug"
# PROP Intermediate_Dir "StaticDebug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /Zi /Od /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\mul" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Debug\vpdfl.lib"
# ADD LIB32 /nologo /out:"..\StaticDebug\vpdfl.lib"

!ELSEIF  "$(CFG)" == "vpdfl - Win32 StaticRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vpdfl___Win32_StaticRelease"
# PROP BASE Intermediate_Dir "vpdfl___Win32_StaticRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "StaticRelease"
# PROP Intermediate_Dir "StaticRelease"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\mul" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Release\vpdfl.lib"
# ADD LIB32 /nologo /out:"..\StaticRelease\vpdfl.lib"

!ENDIF 

# Begin Target

# Name "vpdfl - Win32 Release"
# Name "vpdfl - Win32 Debug"
# Name "vpdfl - Win32 StaticDebug"
# Name "vpdfl - Win32 StaticRelease"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;txx"
# Begin Source File

SOURCE=.\vpdfl_axis_gaussian.cxx
# End Source File
# Begin Source File

SOURCE=.\vpdfl_axis_gaussian_builder.cxx
# End Source File
# Begin Source File

SOURCE=.\vpdfl_axis_gaussian_sampler.cxx
# End Source File
# Begin Source File

SOURCE=.\vpdfl_builder_base.cxx
# End Source File
# Begin Source File

SOURCE=.\vpdfl_gaussian.cxx
# End Source File
# Begin Source File

SOURCE=.\vpdfl_gaussian_builder.cxx
# End Source File
# Begin Source File

SOURCE=.\vpdfl_gaussian_sampler.cxx
# End Source File
# Begin Source File

SOURCE=.\vpdfl_mixture.cxx
# End Source File
# Begin Source File

SOURCE=.\vpdfl_mixture_builder.cxx
# End Source File
# Begin Source File

SOURCE=.\vpdfl_mixture_sampler.cxx
# End Source File
# Begin Source File

SOURCE=.\vpdfl_pc_gaussian.cxx
# End Source File
# Begin Source File

SOURCE=.\vpdfl_pc_gaussian_builder.cxx
# End Source File
# Begin Source File

SOURCE=.\vpdfl_pdf_base.cxx
# End Source File
# Begin Source File

SOURCE=.\vpdfl_prob_chi2.cxx
# End Source File
# Begin Source File

SOURCE=.\vpdfl_sampler_base.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_binary_loader+vpdf_builder_base-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_binary_loader+vpdf_pdf_base-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_vector+vpdfl_builder_base~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vsl_vector+vpdfl_pdf_base~-.cxx"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\vpdfl_axis_gaussian.h
# End Source File
# Begin Source File

SOURCE=.\vpdfl_axis_gaussian_builder.h
# End Source File
# Begin Source File

SOURCE=.\vpdfl_axis_gaussian_sampler.h
# End Source File
# Begin Source File

SOURCE=.\vpdfl_builder_base.h
# End Source File
# Begin Source File

SOURCE=.\vpdfl_gaussian.h
# End Source File
# Begin Source File

SOURCE=.\vpdfl_gaussian_builder.h
# End Source File
# Begin Source File

SOURCE=.\vpdfl_gaussian_sampler.h
# End Source File
# Begin Source File

SOURCE=.\vpdfl_mixture.h
# End Source File
# Begin Source File

SOURCE=.\vpdfl_mixture_builder.h
# End Source File
# Begin Source File

SOURCE=.\vpdfl_mixture_sampler.h
# End Source File
# Begin Source File

SOURCE=.\vpdfl_pc_gaussian.h
# End Source File
# Begin Source File

SOURCE=.\vpdfl_pc_gaussian_builder.h
# End Source File
# Begin Source File

SOURCE=.\vpdfl_pdf_base.h
# End Source File
# Begin Source File

SOURCE=.\vpdfl_prob_chi2.h
# End Source File
# Begin Source File

SOURCE=.\vpdfl_sampler_base.h
# End Source File
# End Group
# End Target
# End Project
