# Microsoft Developer Studio Project File - Name="pcbuilder" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# CM DSP Header file
# This file is read by the build system of cm, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/conversions" /I "$(IUEROOT)/Image" /I "$(IUEROOT)/COOL" /I "$(IUEROOT)/Numerics" /I "$(IUEROOT)/GeneralUtility" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff" == include path
# $(IUEROOT)/bin/ == override in output directory
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF == compiler defines
# test_image_conversions  == name of output library
#  /LIBPATH:"$(IUEROOT)/lib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/lib/"  /LIBPATH:"$(IUEROOT)/bin/$(IntDir)"  /LIBPATH:"$(IUEROOT)/bin/"  /LIBPATH:"$(IUEROOT)/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/vxl/vil"  /LIBPATH:"$(IUEROOT)/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/vxl/vpl"  /LIBPATH:"$(IUEROOT)/Image/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/Image"  /LIBPATH:"$(IUEROOT)/COOL/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/COOL"  /LIBPATH:"$(IUEROOT)/Numerics/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/Numerics"  /LIBPATH:"$(IUEROOT)/GeneralUtility/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/GeneralUtility"  /LIBPATH:"$(IUEROOT)/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/jpeg"  /LIBPATH:"$(IUEROOT)/v3p/png/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/png"  /LIBPATH:"$(IUEROOT)/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/zlib"  /LIBPATH:"$(IUEROOT)/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/tiff"  /LIBPATH:"$(IUEROOT)/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/vcl"  "vil.lib" "vpl.lib" "ImageClasses.lib" "Basics.lib" "Numerics.lib" "TJCOOL.lib" "iue-math.lib" "jpeg.lib" "png.lib" "zlib.lib" "tiff.lib" "vcl.lib" /STACK:10000000  == libraries linked in 
# TARGTYPE "Win32 (x86) Application" 0x0103

CFG=test_image_conversions - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "test_image_conversions.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "test_image_conversions.mak" CFG="test_image_conversions - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "test_image_conversions - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "test_image_conversions - Win32 MinSizeRel" (based on "Win32 (x86) Application")
!MESSAGE "test_image_conversions - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "test_image_conversions - Win32 RelWithDebInfo" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "test_image_conversions - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "$(IUEROOT)/bin/Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32"  /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/conversions" /I "$(IUEROOT)/Image" /I "$(IUEROOT)/COOL" /I "$(IUEROOT)/Numerics" /I "$(IUEROOT)/GeneralUtility" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "test_image_conversions_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089

# ADD LINK32 /LIBPATH:"$(IUEROOT)/lib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/lib/" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/bin/$(IntDir)"  /LIBPATH:"$(IUEROOT)/bin/" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/Image/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/Image" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/COOL/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/COOL" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/Numerics/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/Numerics" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/GeneralUtility/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/GeneralUtility" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/png/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "ImageClasses.lib"
# ADD LINK32 "Basics.lib"
# ADD LINK32 "Numerics.lib"
# ADD LINK32 "TJCOOL.lib"
# ADD LINK32 "iue-math.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "test_image_conversions - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "$(IUEROOT)/bin/Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP   /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /GZ /c
# ADD CPP /nologo  /D "WIN32"  /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /GZ /c
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/conversions" /I "$(IUEROOT)/Image" /I "$(IUEROOT)/COOL" /I "$(IUEROOT)/Numerics" /I "$(IUEROOT)/GeneralUtility" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "test_image_conversions_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32   kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /IGNORE:4089

# ADD LINK32 /LIBPATH:"$(IUEROOT)/lib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/lib/" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/bin/$(IntDir)"  /LIBPATH:"$(IUEROOT)/bin/" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/Image/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/Image" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/COOL/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/COOL" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/Numerics/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/Numerics" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/GeneralUtility/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/GeneralUtility" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/png/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "ImageClasses.lib"
# ADD LINK32 "Basics.lib"
# ADD LINK32 "Numerics.lib"
# ADD LINK32 "TJCOOL.lib"
# ADD LINK32 "iue-math.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "test_image_conversions - Win32 MinSizeRel"
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MinSizeRel"
# PROP BASE Intermediate_Dir "MinSizeRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "$(IUEROOT)/bin/MinSizeRel"
# PROP Intermediate_Dir "MinSizeRel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/conversions" /I "$(IUEROOT)/Image" /I "$(IUEROOT)/COOL" /I "$(IUEROOT)/Numerics" /I "$(IUEROOT)/GeneralUtility" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/conversions" /I "$(IUEROOT)/Image" /I "$(IUEROOT)/COOL" /I "$(IUEROOT)/Numerics" /I "$(IUEROOT)/GeneralUtility" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "test_image_conversions_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O1
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089
# ADD LINK32  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089

# ADD LINK32 /LIBPATH:"$(IUEROOT)/lib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/lib/" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/bin/$(IntDir)"  /LIBPATH:"$(IUEROOT)/bin/" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/Image/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/Image" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/COOL/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/COOL" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/Numerics/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/Numerics" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/GeneralUtility/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/GeneralUtility" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/png/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "ImageClasses.lib"
# ADD LINK32 "Basics.lib"
# ADD LINK32 "Numerics.lib"
# ADD LINK32 "TJCOOL.lib"
# ADD LINK32 "iue-math.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "test_image_conversions - Win32 RelWithDebInfo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RelWithDebInfo"
# PROP BASE Intermediate_Dir "RelWithDebInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "$(IUEROOT)/bin/RelWithDebInfo"
# PROP Intermediate_Dir "RelWithDebInfo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32"  /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/conversions" /I "$(IUEROOT)/Image" /I "$(IUEROOT)/COOL" /I "$(IUEROOT)/Numerics" /I "$(IUEROOT)/GeneralUtility" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "test_image_conversions_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /Zi /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /IGNORE:4089

# ADD LINK32 /LIBPATH:"$(IUEROOT)/lib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/lib/" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/bin/$(IntDir)"  /LIBPATH:"$(IUEROOT)/bin/" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/Image/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/Image" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/COOL/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/COOL" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/Numerics/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/Numerics" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/GeneralUtility/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/GeneralUtility" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/png/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "ImageClasses.lib"
# ADD LINK32 "Basics.lib"
# ADD LINK32 "Numerics.lib"
# ADD LINK32 "TJCOOL.lib"
# ADD LINK32 "iue-math.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vcl.lib"


!ENDIF 

# Begin Target

# Name "test_image_conversions - Win32 Release"
# Name "test_image_conversions - Win32 Debug"
# Name "test_image_conversions - Win32 MinSizeRel"
# Name "test_image_conversions - Win32 RelWithDebInfo"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\test_image_conversions.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

