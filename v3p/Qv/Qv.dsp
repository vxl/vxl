# Microsoft Developer Studio Project File - Name="Qv" - Package Owner=<4>
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
# Qv  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Qv - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Qv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Qv.mak" CFG="OUTPUT_LIBNAME - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Qv - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Qv - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Qv - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "Qv_EXPORTS"
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

!ELSEIF  "$(CFG)" == "Qv - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "Qv_EXPORTS"
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

# Name "Qv - Win32 Release"
# Name "Qv - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\QvLib.cxx

# End Source File
# Begin Source File

SOURCE=.\QvVisitor.cxx

# End Source File
# Begin Source File

SOURCE=.\QvVrmlFile.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+QvNode~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+point2D_const~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+point3D~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vector3D_const~-.cxx

# End Source File
# Begin Source File

SOURCE=.\vecutil.cxx

# End Source File
# Begin Source File

SOURCE=.\wrlbuild.cxx

# End Source File
# Begin Source File

SOURCE=.\wrltraverse.cxx

# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\QvAsciiText.h

# End Source File
# Begin Source File

SOURCE=.\QvBasic.h

# End Source File
# Begin Source File

SOURCE=.\QvChildList.h

# End Source File
# Begin Source File

SOURCE=.\QvCone.h

# End Source File
# Begin Source File

SOURCE=.\QvCoordinate3.h

# End Source File
# Begin Source File

SOURCE=.\QvCube.h

# End Source File
# Begin Source File

SOURCE=.\QvCylinder.h

# End Source File
# Begin Source File

SOURCE=.\QvDB.h

# End Source File
# Begin Source File

SOURCE=.\QvDebugError.h

# End Source File
# Begin Source File

SOURCE=.\QvDict.h

# End Source File
# Begin Source File

SOURCE=.\QvDirectionalLight.h

# End Source File
# Begin Source File

SOURCE=.\QvElement.h

# End Source File
# Begin Source File

SOURCE=.\QvExtensions.h

# End Source File
# Begin Source File

SOURCE=.\QvField.h

# End Source File
# Begin Source File

SOURCE=.\QvFieldData.h

# End Source File
# Begin Source File

SOURCE=.\QvFields.h

# End Source File
# Begin Source File

SOURCE=.\QvFontStyle.h

# End Source File
# Begin Source File

SOURCE=.\QvGroup.h

# End Source File
# Begin Source File

SOURCE=.\QvIndexedFaceSet.h

# End Source File
# Begin Source File

SOURCE=.\QvIndexedLineSet.h

# End Source File
# Begin Source File

SOURCE=.\QvInfo.h

# End Source File
# Begin Source File

SOURCE=.\QvInput.h

# End Source File
# Begin Source File

SOURCE=.\QvLOD.h

# End Source File
# Begin Source File

SOURCE=.\QvLists.h

# End Source File
# Begin Source File

SOURCE=.\QvMFColor.h

# End Source File
# Begin Source File

SOURCE=.\QvMFFace.h

# End Source File
# Begin Source File

SOURCE=.\QvMFFloat.h

# End Source File
# Begin Source File

SOURCE=.\QvMFLong.h

# End Source File
# Begin Source File

SOURCE=.\QvMFString.h

# End Source File
# Begin Source File

SOURCE=.\QvMFVec2f.h

# End Source File
# Begin Source File

SOURCE=.\QvMFVec3f.h

# End Source File
# Begin Source File

SOURCE=.\QvMaterial.h

# End Source File
# Begin Source File

SOURCE=.\QvMaterialBinding.h

# End Source File
# Begin Source File

SOURCE=.\QvMatrixTransform.h

# End Source File
# Begin Source File

SOURCE=.\QvNode.h

# End Source File
# Begin Source File

SOURCE=.\QvNodes.h

# End Source File
# Begin Source File

SOURCE=.\QvNormal.h

# End Source File
# Begin Source File

SOURCE=.\QvNormalBinding.h

# End Source File
# Begin Source File

SOURCE=.\QvOrthographicCamera.h

# End Source File
# Begin Source File

SOURCE=.\QvPList.h

# End Source File
# Begin Source File

SOURCE=.\QvPerspectiveCamera.h

# End Source File
# Begin Source File

SOURCE=.\QvPointLight.h

# End Source File
# Begin Source File

SOURCE=.\QvPointSet.h

# End Source File
# Begin Source File

SOURCE=.\QvReadError.h

# End Source File
# Begin Source File

SOURCE=.\QvRotation.h

# End Source File
# Begin Source File

SOURCE=.\QvSFBitMask.h

# End Source File
# Begin Source File

SOURCE=.\QvSFBool.h

# End Source File
# Begin Source File

SOURCE=.\QvSFColor.h

# End Source File
# Begin Source File

SOURCE=.\QvSFEnum.h

# End Source File
# Begin Source File

SOURCE=.\QvSFFloat.h

# End Source File
# Begin Source File

SOURCE=.\QvSFImage.h

# End Source File
# Begin Source File

SOURCE=.\QvSFLong.h

# End Source File
# Begin Source File

SOURCE=.\QvSFMatrix.h

# End Source File
# Begin Source File

SOURCE=.\QvSFRotation.h

# End Source File
# Begin Source File

SOURCE=.\QvSFString.h

# End Source File
# Begin Source File

SOURCE=.\QvSFVec2f.h

# End Source File
# Begin Source File

SOURCE=.\QvSFVec3f.h

# End Source File
# Begin Source File

SOURCE=.\QvScale.h

# End Source File
# Begin Source File

SOURCE=.\QvSeparator.h

# End Source File
# Begin Source File

SOURCE=.\QvShapeHints.h

# End Source File
# Begin Source File

SOURCE=.\QvSphere.h

# End Source File
# Begin Source File

SOURCE=.\QvSpotLight.h

# End Source File
# Begin Source File

SOURCE=.\QvState.h

# End Source File
# Begin Source File

SOURCE=.\QvString.h

# End Source File
# Begin Source File

SOURCE=.\QvSubField.h

# End Source File
# Begin Source File

SOURCE=.\QvSubNode.h

# End Source File
# Begin Source File

SOURCE=.\QvSwitch.h

# End Source File
# Begin Source File

SOURCE=.\QvTexture2.h

# End Source File
# Begin Source File

SOURCE=.\QvTexture2Transform.h

# End Source File
# Begin Source File

SOURCE=.\QvTextureCoordinate2.h

# End Source File
# Begin Source File

SOURCE=.\QvTransform.h

# End Source File
# Begin Source File

SOURCE=.\QvTransformSeparator.h

# End Source File
# Begin Source File

SOURCE=.\QvTranslation.h

# End Source File
# Begin Source File

SOURCE=.\QvUnknownNode.h

# End Source File
# Begin Source File

SOURCE=.\QvVisitor.h

# End Source File
# Begin Source File

SOURCE=.\QvVrmlFile.h

# End Source File
# Begin Source File

SOURCE=.\QvWWWAnchor.h

# End Source File
# Begin Source File

SOURCE=.\QvWWWInline.h

# End Source File
# Begin Source File

SOURCE=.\Qv_pi.h

# End Source File
# Begin Source File

SOURCE=.\color.h

# End Source File
# Begin Source File

SOURCE=.\mtl.h

# End Source File
# Begin Source File

SOURCE=.\vectors.h

# End Source File
# Begin Source File

SOURCE=.\vecutil.h

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

