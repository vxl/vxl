# Microsoft Developer Studio Project File - Name="oxp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/oxl" /I "$(VXLROOT)/v3p/jpeg"  == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL == compiler defines
#  == override in output directory
# oxp  == name of output library

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
!MESSAGE "oxp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "oxp - Win32 Release" (based on "Win32 (x86) Static Library")
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
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/oxl" /I "$(VXLROOT)/v3p/jpeg"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "oxp_EXPORTS"
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

!ELSEIF  "$(CFG)" == "oxp - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/oxl" /I "$(VXLROOT)/v3p/jpeg"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "oxp_EXPORTS"
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

# Name "oxp - Win32 Release"
# Name "oxp - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
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

SOURCE=.\JPEG_Decompressor.cxx

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

SOURCE=.\RadialLensCorrection.cxx

# End Source File
# Begin Source File

SOURCE=.\SGIMovieFile.cxx

# End Source File
# Begin Source File

SOURCE=.\SGIMovieFilePrivates.cxx

# End Source File
# Begin Source File

SOURCE=.\SGIMovieFileWrite.cxx

# End Source File
# Begin Source File

SOURCE=.\SequenceFileName.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/ImageWarp+byte-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+SGIMV_FrameIndex-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+SGIMV_FrameIndexArray-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+SGIMV_Variables-.cxx

# End Source File
# Begin Source File

SOURCE=.\oxp_parse_seqname.cxx

# End Source File
# Begin Source File

SOURCE=.\oxp_bunch_of_files.cxx

# End Source File
# Begin Source File

SOURCE=.\oxp_mpeg_codec.cxx

# End Source File
# Begin Source File

SOURCE=.\oxp_vidl_moviefile.cxx

# End Source File
# Begin Source File

SOURCE=.\oxp_vidl_mpeg_codec.cxx

# End Source File
# Begin Source File

SOURCE=.\oxp_yuv_to_rgb.cxx

# End Source File
# Begin Source File

SOURCE=.\oxp_vob_frame_index.cxx

# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
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

SOURCE=.\ImageWarp.txx

# End Source File
# Begin Source File

SOURCE=.\JPEG_Decompressor.h

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

SOURCE=.\RadialLensCorrection.h

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

SOURCE=.\SequenceFileName.h

# End Source File
# Begin Source File

SOURCE=.\oxp_bunch_of_files.h

# End Source File
# Begin Source File

SOURCE=.\oxp_mpeg_codec.h

# End Source File
# Begin Source File

SOURCE=.\oxp_parse_seqname.h

# End Source File
# Begin Source File

SOURCE=.\oxp_vidl_moviefile.h

# End Source File
# Begin Source File

SOURCE=.\oxp_vidl_mpeg_codec.h

# End Source File
# Begin Source File

SOURCE=.\MapInverter.h

# End Source File
# Begin Source File

SOURCE=.\MedianReduceImage.h

# End Source File
# Begin Source File

SOURCE=.\oxp_yuv_to_rgb.h

# End Source File
# Begin Source File

SOURCE=.\oxp_vob_frame_index.h

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

