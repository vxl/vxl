# Microsoft Developer Studio Project File - Name="mvl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/oxl"  == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL == compiler defines
#  == override in output directory
# mvl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mvl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mvl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mvl.mak" CFG="mvl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mvl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mvl - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "mvl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mvl - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mvl - Win32 Release"

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
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/oxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "mvl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "mvl - Win32 Debug"

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
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/oxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "mvl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ELSEIF  "$(CFG)" == "mvl - Win32 MinSizeRel"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MinSizeRel"
# PROP BASE Intermediate_Dir "MinSizeRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinSizeRel"
# PROP Intermediate_Dir "MinSizeRel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "_MBCS" /D "_ATL_DLL"  /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FD /c
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/oxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "mvl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O1
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mvl - Win32 RelWithDebInfo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RelWithDebInfo"
# PROP BASE Intermediate_Dir "RelWithDebInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "RelWithDebInfo"
# PROP Intermediate_Dir "RelWithDebInfo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/oxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "mvl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /Zi /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "mvl - Win32 Release"
# Name "mvl - Win32 Debug"
# Name "mvl - Win32 MinSizeRel"
# Name "mvl - Win32 RelWithDebInfo"


# Begin Source File

SOURCE=.\"$(IUEROOT)/oxl/mvl/CMakeLists.txt"

!IF  "$(CFG)" == "mvl - Win32 Release"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/oxl/CMakeLists.txt"\
	"$(IUEROOT)/oxl/mvl/CMakeLists.txt"
# Begin Custom Build

"mvl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/oxl/mvl/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/oxl/mvl" -O"$(IUEROOT)/oxl/mvl" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "mvl - Win32 Debug"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/oxl/CMakeLists.txt"\
	"$(IUEROOT)/oxl/mvl/CMakeLists.txt"
# Begin Custom Build

"mvl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/oxl/mvl/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/oxl/mvl" -O"$(IUEROOT)/oxl/mvl" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "mvl - Win32 MinSizeRel"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/oxl/CMakeLists.txt"\
	"$(IUEROOT)/oxl/mvl/CMakeLists.txt"
# Begin Custom Build

"mvl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/oxl/mvl/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/oxl/mvl" -O"$(IUEROOT)/oxl/mvl" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "mvl - Win32 RelWithDebInfo"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/oxl/CMakeLists.txt"\
	"$(IUEROOT)/oxl/mvl/CMakeLists.txt"
# Begin Custom Build

"mvl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/oxl/mvl/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/oxl/mvl" -O"$(IUEROOT)/oxl/mvl" -B"$(IUEROOT)"

# End Custom Build

!ENDIF

# End Source File
# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AffineMetric.cxx

# End Source File
# Begin Source File

SOURCE=.\ClosestImagePointFinder.cxx

# End Source File
# Begin Source File

SOURCE=.\FDesignMatrix.cxx

# End Source File
# Begin Source File

SOURCE=.\FMPlanarComputeNonLinear.cxx

# End Source File
# Begin Source File

SOURCE=.\FMPlanarNonLinFun.cxx

# End Source File
# Begin Source File

SOURCE=.\FManifoldProject.cxx

# End Source File
# Begin Source File

SOURCE=.\FMatrix.cxx

# End Source File
# Begin Source File

SOURCE=.\FMatrixAffine.cxx

# End Source File
# Begin Source File

SOURCE=.\FMatrixCompute.cxx

# End Source File
# Begin Source File

SOURCE=.\FMatrixComputeLinear.cxx

# End Source File
# Begin Source File

SOURCE=.\FMatrixPlanar.cxx

# End Source File
# Begin Source File

SOURCE=.\FMatrixSkew.cxx

# End Source File
# Begin Source File

SOURCE=.\HMatrix1D.cxx

# End Source File
# Begin Source File

SOURCE=.\HMatrix1DCompute.cxx

# End Source File
# Begin Source File

SOURCE=.\HMatrix1DCompute3Point.cxx

# End Source File
# Begin Source File

SOURCE=.\HMatrix1DComputeDesign.cxx

# End Source File
# Begin Source File

SOURCE=.\HMatrix1DComputeOptimize1.cxx

# End Source File
# Begin Source File

SOURCE=.\HMatrix2D.cxx

# End Source File
# Begin Source File

SOURCE=.\HMatrix2DAffineCompute.cxx

# End Source File
# Begin Source File

SOURCE=.\HMatrix2DCompute.cxx

# End Source File
# Begin Source File

SOURCE=.\HMatrix2DCompute4Line.cxx

# End Source File
# Begin Source File

SOURCE=.\HMatrix2DCompute4Point.cxx

# End Source File
# Begin Source File

SOURCE=.\HMatrix2DComputeLinear.cxx

# End Source File
# Begin Source File

SOURCE=.\HMatrix2DEuclideanCompute.cxx

# End Source File
# Begin Source File

SOURCE=.\HMatrix2DSimilarityCompute.cxx

# End Source File
# Begin Source File

SOURCE=.\HMatrix3D.cxx

# End Source File
# Begin Source File

SOURCE=.\Homg.cxx

# End Source File
# Begin Source File

SOURCE=.\Homg1D.cxx

# End Source File
# Begin Source File

SOURCE=.\Homg2D.cxx

# End Source File
# Begin Source File

SOURCE=.\Homg3D.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgConic.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgInterestPoint.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgInterestPointSet.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgLine2D.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgLine3D.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgLineSeg2D.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgLineSeg3D.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgMetric.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgNorm2D.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgOperator1D.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgOperator2D.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgOperator3D.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgPlane3D.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgPoint1D.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgPoint2D.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgPoint3D.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgPrettyPrint.cxx

# End Source File
# Begin Source File

SOURCE=.\ImageMetric.cxx

# End Source File
# Begin Source File

SOURCE=.\LineSeg.cxx

# End Source File
# Begin Source File

SOURCE=.\LineSegSet.cxx

# End Source File
# Begin Source File

SOURCE=.\MatchSet.cxx

# End Source File
# Begin Source File

SOURCE=.\NViewMatches.cxx

# End Source File
# Begin Source File

SOURCE=.\PMatrix.cxx

# End Source File
# Begin Source File

SOURCE=.\PMatrixAffine.cxx

# End Source File
# Begin Source File

SOURCE=.\PMatrixCompute.cxx

# End Source File
# Begin Source File

SOURCE=.\PMatrixComputeLinear.cxx

# End Source File
# Begin Source File

SOURCE=.\PMatrixDec.cxx

# End Source File
# Begin Source File

SOURCE=.\PMatrixDecompAa.cxx

# End Source File
# Begin Source File

SOURCE=.\PMatrixDecompCR.cxx

# End Source File
# Begin Source File

SOURCE=.\PMatrixEuclidean.cxx

# End Source File
# Begin Source File

SOURCE=.\PairMatchMulti.cxx

# End Source File
# Begin Source File

SOURCE=.\PairMatchMultiIterator.cxx

# End Source File
# Begin Source File

SOURCE=.\PairMatchSet.cxx

# End Source File
# Begin Source File

SOURCE=.\PairMatchSet2D3D.cxx

# End Source File
# Begin Source File

SOURCE=.\PairMatchSetCorner.cxx

# End Source File
# Begin Source File

SOURCE=.\PairMatchSetLineSeg.cxx

# End Source File
# Begin Source File

SOURCE=.\PairSetCorner.cxx

# End Source File
# Begin Source File

SOURCE=.\ProjStructure.cxx

# End Source File
# Begin Source File

SOURCE=.\ProjectiveBasis2D.cxx

# End Source File
# Begin Source File

SOURCE=.\SimilarityMetric.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/ModifyHandle+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_array_2d+HomgPoint2D-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_array_2d+PairMatchSetLineSeg~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_array_2d+vnl_matrix+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_array_2d+vnl_vector+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_sparse_array_1d+FMatrix~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_sparse_array_1d+HMatrix2D~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_sparse_array_1d+HMatrix3D-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_sparse_array_1d+ImageMetric~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_sparse_array_1d+PairMatchSetCorner~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_sparse_array_1d+PairMatchSetLineSeg~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_sparse_array_2d+FMatrix~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_sparse_array_2d+HMatrix2D~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_sparse_array_2d+HMatrix3D-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_sparse_array_2d+PairMatchSetCorner~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_sparse_array_2d+PairMatchSetLineSeg~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_sparse_array_3d+TriTensor~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_sparse_array_3d+TripleMatchSetCorner~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_sparse_array_3d+TripleMatchSetLineSeg~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+ImageMetric~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+ImageMetric~~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+HomgLineSeg2D-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+HomgMatchPoint3D2D~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+HomgPlane3D~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+HomgPoint2D-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+HomgPoint3D~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+LineSeg~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+uint.FMatrix~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+uint.HMatrix2D~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+uint.HMatrix3D-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+uint.HomgInterestPointSet-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+uint.ImageMetric~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+uint.LineSegSet-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+uint.PMatrix-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+uint.PairMatchSetCorner~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+uint.PairMatchSetLineSeg~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+uint.TriTensor-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+uint.TriTensor~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+uint.TripleMatchSetCorner~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+uint.TripleMatchSetLineSeg~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+FMatrix~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+HMatrix1D-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+HMatrix2D-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+HomgInterestPoint-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+HomgInterestPointSet~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+HomgLine2D-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+HomgLine3D-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+HomgLineSeg2D-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+HomgLineSeg3D-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+HomgMetric-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+HomgPlane3D-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+HomgPoint1D-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+HomgPoint2D-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+HomgPoint3D-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+ImageMetric~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+NViewMatch-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+PMatrix-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+PMatrix~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+PairMatchMulti-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+TriTensor-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_vector+HomgPoint2D--.cxx

# End Source File
# Begin Source File

SOURCE=.\TriTensor.cxx

# End Source File
# Begin Source File

SOURCE=.\TripleMatchSet.cxx

# End Source File
# Begin Source File

SOURCE=.\TripleMatchSetCorner.cxx

# End Source File
# Begin Source File

SOURCE=.\TripleMatchSetLineSeg.cxx

# End Source File
# Begin Source File

SOURCE=.\mvl_five_point_camera_pencil.cxx

# End Source File
# Begin Source File

SOURCE=.\mvl_multi_view_matches.cxx

# End Source File
# Begin Source File

SOURCE=.\mvl_psi.cxx

# End Source File
# Begin Source File

SOURCE=.\mvl_six_point_design_matrix_row.cxx

# End Source File
# Begin Source File

SOURCE=.\mvl_three_view_six_point_structure.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

