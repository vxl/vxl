# Microsoft Developer Studio Project File - Name="rtvl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
#
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/rpl" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# rtvl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=rtvl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "rtvl.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "rtvl.mak" CFG="rtvl - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "rtvl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "rtvl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rtvl - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/rpl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "rtvl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "rtvl - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/rpl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "rtvl_EXPORTS"
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

# Name "rtvl - Win32 Release"
# Name "rtvl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\Templates/rtvl_tensor+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rtvl_tensor+3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rtvl_weight_original+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rtvl_refine+3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rtvl_weight_smooth+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rtvl_voter+3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rtvl_voter+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rtvl_vote+3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rtvl_votee_d+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rtvl_refine+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rtvl_weight_smooth+3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rtvl_tensor_d+3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rtvl_vote+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rtvl_votee+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rtvl_weight_original+3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rtvl_tensor_d+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rtvl_votee_d+3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rtvl_votee+3-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\rtvl_refine.hxx
# End Source File
# Begin Source File
SOURCE=.\rtvl_votee.hxx
# End Source File
# Begin Source File
SOURCE=.\rtvl_weight_smooth.txx
# End Source File
# Begin Source File
SOURCE=.\rtvl_tensor_d.hxx
# End Source File
# Begin Source File
SOURCE=.\rtvl_voter.txx
# End Source File
# Begin Source File
SOURCE=.\rtvl_vote.hxx
# End Source File
# Begin Source File
SOURCE=.\rtvl_intpow.hxx
# End Source File
# Begin Source File
SOURCE=.\rtvl_weight_original.txx
# End Source File
# Begin Source File
SOURCE=.\rtvl_votee.txx
# End Source File
# Begin Source File
SOURCE=.\rtvl_weight.hxx
# End Source File
# Begin Source File
SOURCE=.\rtvl_voter.hxx
# End Source File
# Begin Source File
SOURCE=.\rtvl_tensor.txx
# End Source File
# Begin Source File
SOURCE=.\rtvl_votee_d.hxx
# End Source File
# Begin Source File
SOURCE=.\rtvl_terms.hxx
# End Source File
# Begin Source File
SOURCE=.\rtvl_tensor_d.txx
# End Source File
# Begin Source File
SOURCE=.\rtvl_refine.txx
# End Source File
# Begin Source File
SOURCE=.\rtvl_weight_smooth.hxx
# End Source File
# Begin Source File
SOURCE=.\rtvl_tensor.hxx
# End Source File
# Begin Source File
SOURCE=.\rtvl_vote.txx
# End Source File
# Begin Source File
SOURCE=.\rtvl_votee_d.txx
# End Source File
# Begin Source File
SOURCE=.\rtvl_tokens.hxx
# End Source File
# Begin Source File
SOURCE=.\rtvl_weight_original.hxx
# End Source File
# End Group
# End Target
# End Project
