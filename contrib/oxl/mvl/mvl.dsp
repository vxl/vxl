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
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/oxl" == include path
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
!MESSAGE "mvl - Win32 Release" (based on "Win32 (x86) Static Library")
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
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/oxl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "mvl_EXPORTS"
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
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /GZ /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/oxl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "mvl_EXPORTS"
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

# Name "mvl - Win32 Release"
# Name "mvl - Win32 Debug"

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

SOURCE=.\HomgPoint2D.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgPoint3D.cxx

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

SOURCE=.\HomgOperator2D.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgOperator3D.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgPlane3D.cxx

# End Source File
# Begin Source File

SOURCE=.\HomgPrettyPrint.cxx

# End Source File
# Begin Source File

SOURCE=.\ImageMetric.cxx

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
# Begin Source File

SOURCE=.\ComputeGRIC.cxx

# End Source File
# Begin Source File

SOURCE=.\FMatrixCompute7Point.cxx

# End Source File
# Begin Source File

SOURCE=.\FMatrixComputeLMedSq.cxx

# End Source File
# Begin Source File

SOURCE=.\FMatrixComputeMLESAC.cxx

# End Source File
# Begin Source File

SOURCE=.\FMatrixComputeNonLinear.cxx

# End Source File
# Begin Source File

SOURCE=.\FMatrixComputeRANSAC.cxx

# End Source File
# Begin Source File

SOURCE=.\FMatrixComputeRobust.cxx

# End Source File
# Begin Source File

SOURCE=.\HMatrix2DComputeMLESAC.cxx

# End Source File
# Begin Source File

SOURCE=.\HMatrix2DComputeRobust.cxx

# End Source File
# Begin Source File

SOURCE=.\Probability.cxx

# End Source File
# Begin Source File

SOURCE=.\FileNameGenerator.cxx

# End Source File
# Begin Source File

SOURCE=.\FileNameGeneratorBase.cxx

# End Source File
# Begin Source File

SOURCE=.\RawPMatrixStore.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+PMatrix-.cxx

# End Source File
# Begin Source File

SOURCE=.\TriTensorCompute.cxx

# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AffineMetric.h

# End Source File
# Begin Source File

SOURCE=.\ClosestImagePointFinder.h

# End Source File
# Begin Source File

SOURCE=.\ComputeGRIC.h

# End Source File
# Begin Source File

SOURCE=.\FDesignMatrix.h

# End Source File
# Begin Source File

SOURCE=.\FMPlanarComputeNonLinear.h

# End Source File
# Begin Source File

SOURCE=.\FMPlanarNonLinFun.h

# End Source File
# Begin Source File

SOURCE=.\FManifoldProject.h

# End Source File
# Begin Source File

SOURCE=.\FMatrix.h

# End Source File
# Begin Source File

SOURCE=.\FMatrixAffine.h

# End Source File
# Begin Source File

SOURCE=.\FMatrixCompute.h

# End Source File
# Begin Source File

SOURCE=.\FMatrixCompute7Point.h

# End Source File
# Begin Source File

SOURCE=.\FMatrixComputeLMedSq.h

# End Source File
# Begin Source File

SOURCE=.\FMatrixComputeLinear.h

# End Source File
# Begin Source File

SOURCE=.\FMatrixComputeMLESAC.h

# End Source File
# Begin Source File

SOURCE=.\FMatrixComputeNonLinear.h

# End Source File
# Begin Source File

SOURCE=.\FMatrixComputeRANSAC.h

# End Source File
# Begin Source File

SOURCE=.\FMatrixComputeRobust.h

# End Source File
# Begin Source File

SOURCE=.\FMatrixPlanar.h

# End Source File
# Begin Source File

SOURCE=.\FMatrixSkew.h

# End Source File
# Begin Source File

SOURCE=.\FileNameGenerator.h

# End Source File
# Begin Source File

SOURCE=.\FileNameGeneratorBase.h

# End Source File
# Begin Source File

SOURCE=.\HMatrix1D.h

# End Source File
# Begin Source File

SOURCE=.\HMatrix1DCompute.h

# End Source File
# Begin Source File

SOURCE=.\HMatrix1DCompute3Point.h

# End Source File
# Begin Source File

SOURCE=.\HMatrix1DComputeDesign.h

# End Source File
# Begin Source File

SOURCE=.\HMatrix1DComputeOptimize1.h

# End Source File
# Begin Source File

SOURCE=.\HMatrix2D.h

# End Source File
# Begin Source File

SOURCE=.\HMatrix2DAffineCompute.h

# End Source File
# Begin Source File

SOURCE=.\HMatrix2DCompute.h

# End Source File
# Begin Source File

SOURCE=.\HMatrix2DCompute4Line.h

# End Source File
# Begin Source File

SOURCE=.\HMatrix2DCompute4Point.h

# End Source File
# Begin Source File

SOURCE=.\HMatrix2DComputeLinear.h

# End Source File
# Begin Source File

SOURCE=.\HMatrix2DComputeMLESAC.h

# End Source File
# Begin Source File

SOURCE=.\HMatrix2DComputeRobust.h

# End Source File
# Begin Source File

SOURCE=.\HMatrix2DEuclideanCompute.h

# End Source File
# Begin Source File

SOURCE=.\HMatrix2DSimilarityCompute.h

# End Source File
# Begin Source File

SOURCE=.\HMatrix3D.h

# End Source File
# Begin Source File

SOURCE=.\Homg.h

# End Source File
# Begin Source File

SOURCE=.\Homg1D.h

# End Source File
# Begin Source File

SOURCE=.\Homg2D.h

# End Source File
# Begin Source File

SOURCE=.\Homg3D.h

# End Source File
# Begin Source File

SOURCE=.\HomgPoint2D.h

# End Source File
# Begin Source File

SOURCE=.\HomgPoint3D.h

# End Source File
# Begin Source File

SOURCE=.\HomgInterestPoint.h

# End Source File
# Begin Source File

SOURCE=.\HomgInterestPointSet.h

# End Source File
# Begin Source File

SOURCE=.\HomgLine2D.h

# End Source File
# Begin Source File

SOURCE=.\HomgLine3D.h

# End Source File
# Begin Source File

SOURCE=.\HomgLineSeg2D.h

# End Source File
# Begin Source File

SOURCE=.\HomgLineSeg3D.h

# End Source File
# Begin Source File

SOURCE=.\HomgMetric.h

# End Source File
# Begin Source File

SOURCE=.\HomgNorm2D.h

# End Source File
# Begin Source File

SOURCE=.\HomgOperator2D.h

# End Source File
# Begin Source File

SOURCE=.\HomgOperator3D.h

# End Source File
# Begin Source File

SOURCE=.\HomgPlane3D.h

# End Source File
# Begin Source File

SOURCE=.\HomgPrettyPrint.h

# End Source File
# Begin Source File

SOURCE=.\ImageMetric.h

# End Source File
# Begin Source File

SOURCE=.\LineSegSet.h

# End Source File
# Begin Source File

SOURCE=.\MatchSet.h

# End Source File
# Begin Source File

SOURCE=.\NViewMatches.h

# End Source File
# Begin Source File

SOURCE=.\PMatrix.h

# End Source File
# Begin Source File

SOURCE=.\PMatrixAffine.h

# End Source File
# Begin Source File

SOURCE=.\PMatrixCompute.h

# End Source File
# Begin Source File

SOURCE=.\PMatrixComputeLinear.h

# End Source File
# Begin Source File

SOURCE=.\PMatrixDec.h

# End Source File
# Begin Source File

SOURCE=.\PMatrixDecompAa.h

# End Source File
# Begin Source File

SOURCE=.\PMatrixDecompCR.h

# End Source File
# Begin Source File

SOURCE=.\PMatrixEuclidean.h

# End Source File
# Begin Source File

SOURCE=.\PairMatchMulti.h

# End Source File
# Begin Source File

SOURCE=.\PairMatchMultiIterator.h

# End Source File
# Begin Source File

SOURCE=.\PairMatchSet.h

# End Source File
# Begin Source File

SOURCE=.\PairMatchSet2D3D.h

# End Source File
# Begin Source File

SOURCE=.\PairMatchSetCorner.h

# End Source File
# Begin Source File

SOURCE=.\PairMatchSetLineSeg.h

# End Source File
# Begin Source File

SOURCE=.\PairSetCorner.h

# End Source File
# Begin Source File

SOURCE=.\Probability.h

# End Source File
# Begin Source File

SOURCE=.\ProjStructure.h

# End Source File
# Begin Source File

SOURCE=.\ProjectiveBasis2D.h

# End Source File
# Begin Source File

SOURCE=.\RawPMatrixStore.h

# End Source File
# Begin Source File

SOURCE=.\SimilarityMetric.h

# End Source File
# Begin Source File

SOURCE=.\TriTensor.h

# End Source File
# Begin Source File

SOURCE=.\TripleMatchSet.h

# End Source File
# Begin Source File

SOURCE=.\TripleMatchSetCorner.h

# End Source File
# Begin Source File

SOURCE=.\TripleMatchSetLineSeg.h

# End Source File
# Begin Source File

SOURCE=.\mvl_five_point_camera_pencil.h

# End Source File
# Begin Source File

SOURCE=.\mvl_multi_view_matches.h

# End Source File
# Begin Source File

SOURCE=.\mvl_psi.h

# End Source File
# Begin Source File

SOURCE=.\mvl_six_point_design_matrix_row.h

# End Source File
# Begin Source File

SOURCE=.\mvl_three_view_six_point_structure.h

# End Source File
# Begin Source File

SOURCE=.\FMSkewComputeLinear.h

# End Source File
# Begin Source File

SOURCE=.\FMatrixCompute8Point.h

# End Source File
# Begin Source File

SOURCE=.\HomgMap2D.h

# End Source File
# Begin Source File

SOURCE=.\HomgMatchPoint3D2D.h

# End Source File
# Begin Source File

SOURCE=.\LineSeg.h

# End Source File
# Begin Source File

SOURCE=.\ModifyHandle.h

# End Source File
# Begin Source File

SOURCE=.\PMatrixCompute6Point.h

# End Source File
# Begin Source File

SOURCE=.\PairMatchMultiStrength.h

# End Source File
# Begin Source File

SOURCE=.\PairMatchSetMulti.h

# End Source File
# Begin Source File

SOURCE=.\TriTensorCompute.h

# End Source File
# Begin Source File

SOURCE=.\mvl_modify_handle.h

# End Source File
# Begin Source File

SOURCE=.\PMatrix_sptr.h

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

