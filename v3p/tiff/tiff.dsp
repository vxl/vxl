# Microsoft Developer Studio Project File - Name="tiff" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=tiff - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tiff.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tiff.mak" CFG="tiff - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tiff - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tiff - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tiff - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_LIB" /D "JPEG_SUPPORT" /D "HAVE_IEEEFP" /D USE_VARARGS=0 /D "USE_PROTOTYPES" /D "USE_CONST" /YX /Zl /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Release\tiff.lib"

!ELSEIF  "$(CFG)" == "tiff - Win32 Debug"

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(IUEROOT)\v3p\jpeg" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_LIB" /D "JPEG_SUPPORT" /D "HAVE_IEEEFP" /D USE_VARARGS=0 /D "USE_PROTOTYPES" /D "USE_CONST" /YX /Zl /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Debug\tiff.lib"

!ENDIF 

# Begin Target

# Name "tiff - Win32 Release"
# Name "tiff - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\tif_aux.c
# End Source File
# Begin Source File

SOURCE=.\tif_close.c
# End Source File
# Begin Source File

SOURCE=.\tif_codec.c
# End Source File
# Begin Source File

SOURCE=.\tif_compress.c
# End Source File
# Begin Source File

SOURCE=.\tif_dir.c
# End Source File
# Begin Source File

SOURCE=.\tif_dirinfo.c
# End Source File
# Begin Source File

SOURCE=.\tif_dirread.c
# End Source File
# Begin Source File

SOURCE=.\tif_dirwrite.c
# End Source File
# Begin Source File

SOURCE=.\tif_dumpmode.c
# End Source File
# Begin Source File

SOURCE=.\tif_error.c
# End Source File
# Begin Source File

SOURCE=.\tif_fax3.c
# End Source File
# Begin Source File

SOURCE=.\tif_fax3sm.c
# End Source File
# Begin Source File

SOURCE=.\tif_flush.c
# End Source File
# Begin Source File

SOURCE=.\tif_getimage.c
# End Source File
# Begin Source File

SOURCE=.\tif_jpeg.c
# End Source File
# Begin Source File

SOURCE=.\tif_luv.c
# End Source File
# Begin Source File

SOURCE=.\tif_lzw.c
# End Source File
# Begin Source File

SOURCE=.\tif_next.c
# End Source File
# Begin Source File

SOURCE=.\tif_open.c
# End Source File
# Begin Source File

SOURCE=.\tif_packbits.c
# End Source File
# Begin Source File

SOURCE=.\tif_pixarlog.c
# End Source File
# Begin Source File

SOURCE=.\tif_predict.c
# End Source File
# Begin Source File

SOURCE=.\tif_print.c
# End Source File
# Begin Source File

SOURCE=.\tif_read.c
# End Source File
# Begin Source File

SOURCE=.\tif_strip.c
# End Source File
# Begin Source File

SOURCE=.\tif_swab.c
# End Source File
# Begin Source File

SOURCE=.\tif_thunder.c
# End Source File
# Begin Source File

SOURCE=.\tif_tile.c
# End Source File
# Begin Source File

SOURCE=.\tif_version.c
# End Source File
# Begin Source File

SOURCE=.\tif_warning.c
# End Source File
# Begin Source File

SOURCE=.\tif_win32.c
# End Source File
# Begin Source File

SOURCE=.\tif_write.c
# End Source File
# Begin Source File

SOURCE=.\tif_zip.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\format.h
# End Source File
# Begin Source File

SOURCE=.\machdep.h
# End Source File
# Begin Source File

SOURCE=.\port.h
# End Source File
# Begin Source File

SOURCE=.\prototypes.h
# End Source File
# Begin Source File

SOURCE=.\t4.h
# End Source File
# Begin Source File

SOURCE=.\tif_dir.h
# End Source File
# Begin Source File

SOURCE=.\tif_fax3.h
# End Source File
# Begin Source File

SOURCE=.\tif_predict.h
# End Source File
# Begin Source File

SOURCE=.\tiff.h
# End Source File
# Begin Source File

SOURCE=.\tiffcomp.h
# End Source File
# Begin Source File

SOURCE=.\tiffcompat.h
# End Source File
# Begin Source File

SOURCE=.\tiffconf.h
# End Source File
# Begin Source File

SOURCE=.\tiffio.h
# End Source File
# Begin Source File

SOURCE=.\tiffiop.h
# End Source File
# Begin Source File

SOURCE=.\uvcode.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\makefile
# End Source File
# Begin Source File

SOURCE=.\template.tex
# End Source File
# End Target
# End Project
