# Microsoft Developer Studio Project File - Name="vidl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

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
!MESSAGE NMAKE /f "vidl.mak" CFG="vidl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vidl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vidl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)" /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c /Zl
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
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
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /I "$(IUEROOT)" /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /YX /FD /GZ /c /Zl
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Debug\vidl.lib"

!ENDIF 

# Begin Target

# Name "vidl - Win32 Release"
# Name "vidl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=".\Templates\vbl_smart_ptr+vidl_clip.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_smart_ptr+vidl_codec.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_smart_ptr+vidl_frame.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_smart_ptr+vidl_image_list_codec.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_smart_ptr+vidl_movie.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_list+vbl_smart_ptr+vidl_clip.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_list+vbl_smart_ptr+vidl_codec.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+vbl_smart_ptr+vidl_frame.cxx"
# End Source File
# Begin Source File

SOURCE=.\vidl_avicodec.cxx
# End Source File
# Begin Source File

SOURCE=.\vidl_clip.cxx
# End Source File
# Begin Source File

SOURCE=.\vidl_frame.cxx
# End Source File
# Begin Source File

SOURCE=.\vidl_frame_as_image.cxx
# End Source File
# Begin Source File

SOURCE=.\vidl_image_list_codec.cxx
# End Source File
# Begin Source File

SOURCE=.\vidl_io.cxx
# End Source File
# Begin Source File

SOURCE=.\vidl_movie.cxx
# End Source File
# Begin Source File

SOURCE=.\vidl_test.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\vidl_avicodec.h
# End Source File
# Begin Source File

SOURCE=.\vidl_avicodec_sptr.h
# End Source File
# Begin Source File

SOURCE=.\vidl_clip.h
# End Source File
# Begin Source File

SOURCE=.\vidl_clip_sptr.h
# End Source File
# Begin Source File

SOURCE=.\vidl_codec.h
# End Source File
# Begin Source File

SOURCE=.\vidl_codec_sptr.h
# End Source File
# Begin Source File

SOURCE=.\vidl_frame.h
# End Source File
# Begin Source File

SOURCE=.\vidl_frame_as_image.h
# End Source File
# Begin Source File

SOURCE=.\vidl_frame_as_image_sptr.h
# End Source File
# Begin Source File

SOURCE=.\vidl_frame_sptr.h
# End Source File
# Begin Source File

SOURCE=.\vidl_image_list_codec.h
# End Source File
# Begin Source File

SOURCE=.\vidl_image_list_codec_sptr.h
# End Source File
# Begin Source File

SOURCE=.\vidl_io.h
# End Source File
# Begin Source File

SOURCE=.\vidl_movie.h
# End Source File
# Begin Source File

SOURCE=.\vidl_movie_sptr.h
# End Source File
# End Group
# End Target
# End Project
