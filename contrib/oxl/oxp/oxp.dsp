# Microsoft Developer Studio Project File - Name="oxp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=oxp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "oxp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "oxp.mak" CFG="oxp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "oxp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "oxp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "oxp - Win32 ReleaseWithDBInfo" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "oxp - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/vxl/config.win32-vc60" /I "$(IUEROOT)/vxl" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Release\oxp.lib"

!ELSEIF  "$(CFG)" == "oxp - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/vxl/config.win32-vc60" /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Debug\oxp.lib"

!ELSEIF  "$(CFG)" == "oxp - Win32 ReleaseWithDBInfo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "oxp___Win32_ReleaseWithDBInfo"
# PROP BASE Intermediate_Dir "oxp___Win32_ReleaseWithDBInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseWithDBInfo"
# PROP Intermediate_Dir "ReleaseWithDBInfo"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/vxl/config.win32-vc60" /I "$(IUEROOT)/vxl" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /Ob2 /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/vxl/config.win32-vc60" /I "$(IUEROOT)/vxl" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Release\oxp.lib"
# ADD LIB32 /nologo /out:"..\ReleaseWithDBInfo\oxp.lib"

!ENDIF 

# Begin Target

# Name "oxp - Win32 Release"
# Name "oxp - Win32 Debug"
# Name "oxp - Win32 ReleaseWithDBInfo"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ByteConvertImage.cxx
# End Source File
# Begin Source File

SOURCE=.\GXFileVisitor.cxx
# End Source File
# Begin Source File

SOURCE=.\ImageSequenceMovieFile.cxx
# End Source File
# Begin Source File

SOURCE=.\ImageSequenceName.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\ImageWarp+byte-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\ImageWarp+RGB+byte--.cxx"
# End Source File
# Begin Source File

SOURCE=.\Mapping_2d_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\MedianReduceImage.cxx
# End Source File
# Begin Source File

SOURCE=.\MovieFile.cxx
# End Source File
# Begin Source File

SOURCE=.\MovieFileInterface.cxx
# End Source File
# Begin Source File

SOURCE=.\oxp_parse_seqname.cxx
# End Source File
# Begin Source File

SOURCE=.\RadialLensCorrection.cxx
# End Source File
# Begin Source File

SOURCE=.\SequenceFileName.cxx
# End Source File
# Begin Source File

SOURCE=.\SGIMovieFile.cxx
# End Source File
# Begin Source File

SOURCE=.\SGIMovieFilePrivates.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vector+SGIMV_Variables-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vsl_conic_as_matrix.cxx
# End Source File
# Begin Source File

SOURCE=.\vsl_conic_as_params.cxx
# End Source File
# Begin Source File

SOURCE=.\vsl_conic_points.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ByteConvertImage.h
# End Source File
# Begin Source File

SOURCE=.\GXFileVisitor.h
# End Source File
# Begin Source File

SOURCE=.\ImageSequenceMovieFile.h
# End Source File
# Begin Source File

SOURCE=.\ImageSequenceName.h
# End Source File
# Begin Source File

SOURCE=.\ImageWarp.h
# End Source File
# Begin Source File

SOURCE=.\JPEG_Decompressor.h
# End Source File
# Begin Source File

SOURCE=.\MapInverter.h
# End Source File
# Begin Source File

SOURCE=.\Mapping_2d_2d.h
# End Source File
# Begin Source File

SOURCE=.\MovieFile.h
# End Source File
# Begin Source File

SOURCE=.\MovieFileInterface.h
# End Source File
# Begin Source File

SOURCE=.\oxp_parse_seqname.h
# End Source File
# Begin Source File

SOURCE=.\POX.h
# End Source File
# Begin Source File

SOURCE=.\RadialLensCorrection.h
# End Source File
# Begin Source File

SOURCE=.\SequenceFileName.h
# End Source File
# Begin Source File

SOURCE=.\SGIMovieFile.h
# End Source File
# Begin Source File

SOURCE=.\SGIMovieFilePrivates.h
# End Source File
# Begin Source File

SOURCE=.\SGIMovieFileWrite.h
# End Source File
# Begin Source File

SOURCE=.\vsl_conic_as_matrix.h
# End Source File
# Begin Source File

SOURCE=.\vsl_conic_as_params.h
# End Source File
# Begin Source File

SOURCE=.\vsl_conic_fit.h
# End Source File
# Begin Source File

SOURCE=.\vsl_conic_points.h
# End Source File
# End Group
# End Target
# End Project
