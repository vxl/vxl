# Microsoft Developer Studio Project File - Name="dcmtk" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# dcmtk  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dcmtk - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dcmtk.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dcmtk.mak" CFG="OUTPUT_LIBNAME - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dcmtk - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "dcmtk - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dcmtk - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "dcmtk_EXPORTS"
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

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "dcmtk_EXPORTS"
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

# Name "dcmtk - Win32 Release"
# Name "dcmtk - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcbytstr.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcchrstr.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dccodec.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcdatset.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcdicent.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcdict.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcdictzz.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcdirrec.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcelem.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcerror.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcfilefo.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dchashdi.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcistrma.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcistrmf.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcitem.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dclist.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcmetinf.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcobject.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcostrma.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcostrmf.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcpcache.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcpixel.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcpixseq.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcpxitem.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcsequen.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcstack.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcswap.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dctag.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dctagkey.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dctypes.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcuid.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvm.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvr.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrae.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvras.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrat.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrcs.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrda.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrds.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrdt.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrfd.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrfl.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvris.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrlo.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrlt.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrobow.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrof.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrpn.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrpobw.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrsh.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrsl.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrss.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrst.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrtm.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrui.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrul.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrulup.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrus.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcvrut.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmdata/libsrc/dcxfer.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/dcmimage.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/dibaslut.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/didislut.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/didispfn.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/didocu.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/digsdfn.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/digsdlut.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/diimage.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/diinpx.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/diluptab.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/dimo1img.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/dimo2img.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/dimoimg.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/dimoimg3.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/dimoimg4.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/dimoimg5.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/dimomod.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/dimoopx.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/dimopx.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/diovdat.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/diovlay.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/diovlimg.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/diovpln.cxx
# End Source File
# Begin Source File
SOURCE=.\dcmimgle/libsrc/diutils.cxx
# End Source File
# Begin Source File
SOURCE=.\ofstd/libsrc/ofcond.cxx
# End Source File
# Begin Source File
SOURCE=.\ofstd/libsrc/ofconsol.cxx
# End Source File
# Begin Source File
SOURCE=.\ofstd/libsrc/ofcrc32.cxx
# End Source File
# Begin Source File
SOURCE=.\ofstd/libsrc/ofdate.cxx
# End Source File
# Begin Source File
SOURCE=.\ofstd/libsrc/ofdatime.cxx
# End Source File
# Begin Source File
SOURCE=.\ofstd/libsrc/oflist.cxx
# End Source File
# Begin Source File
SOURCE=.\ofstd/libsrc/ofstd.cxx
# End Source File
# Begin Source File
SOURCE=.\ofstd/libsrc/ofstring.cxx
# End Source File
# Begin Source File
SOURCE=.\ofstd/libsrc/ofthread.cxx
# End Source File
# Begin Source File
SOURCE=.\ofstd/libsrc/oftime.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
