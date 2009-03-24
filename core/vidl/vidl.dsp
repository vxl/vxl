# Microsoft Developer Studio Project File - Name="vidl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vidl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vidl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vidl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vidl.mak" CFG="vil2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vidl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vidl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vidl - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vidl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vidl - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vidl_EXPORTS"
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

# Name "vidl - Win32 Release"
# Name "vidl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\vidl_ffmpeg_init.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_ffmpeg_istream.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_ffmpeg_ostream.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_ffmpeg_ostream_params.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_image_list_istream.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_image_list_ostream.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_dc1394_istream.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_iidc1394_params.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_pixel_format.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_frame.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_convert.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_ffmpeg_convert.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_color.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_dshow.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_dshow_file_istream.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_dshow_istream_params.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_dshow_live_istream.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_pixel_iterator.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_dshow_istream_params_esf.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_v4l_istream.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_v4l_params.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_istream_image_resource.cxx
# End Source File
# Begin Source File
SOURCE=.\v4l2_pixel_format.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_v4l2_device.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_v4l2_devices.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_v4l2_istream.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_v4l2_control.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_smart_ptr+vidl_frame-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_smart_ptr+vidl_istream-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_smart_ptr+vidl_ostream-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_smart_ptr+vidl_v4l2_device-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx;in"
# Begin Source File
SOURCE=.\vidl_ffmpeg_init.h
# End Source File
# Begin Source File
SOURCE=.\vidl_ffmpeg_istream.h
# End Source File
# Begin Source File
SOURCE=.\vidl_ffmpeg_ostream.h
# End Source File
# Begin Source File
SOURCE=.\vidl_ffmpeg_ostream_params.h
# End Source File
# Begin Source File
SOURCE=.\vidl_image_list_istream.h
# End Source File
# Begin Source File
SOURCE=.\vidl_image_list_ostream.h
# End Source File
# Begin Source File
SOURCE=.\vidl_istream.h
# End Source File
# Begin Source File
SOURCE=.\vidl_ostream.h
# End Source File
# Begin Source File
SOURCE=.\vidl_dc1394_istream.h
# End Source File
# Begin Source File
SOURCE=.\vidl_iidc1394_params.h
# End Source File
# Begin Source File
SOURCE=.\vidl_pixel_format.h
# End Source File
# Begin Source File
SOURCE=.\vidl_convert.h
# End Source File
# Begin Source File
SOURCE=.\vidl_frame.h
# End Source File
# Begin Source File
SOURCE=.\vidl_frame_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vidl_pixel_iterator.h
# End Source File
# Begin Source File
SOURCE=.\vidl_color.h
# End Source File
# Begin Source File
SOURCE=.\vidl_ffmpeg_convert.h
# End Source File
# Begin Source File
SOURCE=.\vidl_dshow.h
# End Source File
# Begin Source File
SOURCE=.\vidl_dshow_file_istream.h
# End Source File
# Begin Source File
SOURCE=.\vidl_dshow_istream_params.h
# End Source File
# Begin Source File
SOURCE=.\vidl_dshow_istream_params_esf.h
# End Source File
# Begin Source File
SOURCE=.\vidl_dshow_live_istream.h
# End Source File
# Begin Source File
SOURCE=.\vidl_exception.h
# End Source File
# Begin Source File
SOURCE=.\vidl_pixel_iterator.txx
# End Source File
# Begin Source File
SOURCE=.\vidl_v4l_istream.h
# End Source File
# Begin Source File
SOURCE=.\vidl_v4l_params.h
# End Source File
# Begin Source File
SOURCE=.\vidl_istream_image_resource.h
# End Source File
# Begin Source File
SOURCE=.\vidl_istream_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vidl_ostream_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vidl_ffmpeg_istream_stub.txx
# End Source File
# Begin Source File
SOURCE=.\vidl_ffmpeg_istream_v1.txx
# End Source File
# Begin Source File
SOURCE=.\vidl_ffmpeg_istream_v2.txx
# End Source File
# Begin Source File
SOURCE=.\vidl_ffmpeg_ostream_stub.txx
# End Source File
# Begin Source File
SOURCE=.\vidl_ffmpeg_ostream_v1.txx
# End Source File
# Begin Source File
SOURCE=.\vidl_ffmpeg_ostream_v2.txx
# End Source File
# Begin Source File
SOURCE=.\v4l2_pixel_format.h
# End Source File
# Begin Source File
SOURCE=.\vidl_v4l2_device.h
# End Source File
# Begin Source File
SOURCE=.\vidl_v4l2_device_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vidl_v4l2_devices.h
# End Source File
# Begin Source File
SOURCE=.\vidl_v4l2_istream.h
# End Source File
# Begin Source File
SOURCE=.\vidl_v4l2_control.h
# End Source File
# Begin Source File
SOURCE=.\vidl_ffmpeg_ostream_v3.txx
# End Source File
# Begin Source File
SOURCE=.\vidl_config.h.in
# End Source File
# End Group
# End Target
# End Project
