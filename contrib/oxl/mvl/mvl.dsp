# Microsoft Developer Studio Project File - Name="mvl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

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
!MESSAGE "mvl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mvl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mvl - Win32 ReleaseWithDBInfo" (based on "Win32 (x86) Static Library")
!MESSAGE 

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/vxl/config.win32-vc60" /I "$(IUEROOT)/vxl" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Release\mvl.lib"

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/vxl/config.win32-vc60" /I "$(IUEROOT)\vcl\config.win32-VC60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)/vxl" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Debug\mvl.lib"

!ELSEIF  "$(CFG)" == "mvl - Win32 ReleaseWithDBInfo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mvl___Win32_ReleaseWithDBInfo"
# PROP BASE Intermediate_Dir "mvl___Win32_ReleaseWithDBInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseWithDBInfo"
# PROP Intermediate_Dir "ReleaseWithDBInfo"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/vxl/config.win32-vc60" /I "$(IUEROOT)/vxl" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/vxl/config.win32-vc60" /I "$(IUEROOT)/vxl" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Release\mvl.lib"
# ADD LIB32 /nologo /out:"..\Release\mvl.lib"

!ENDIF 

# Begin Target

# Name "mvl - Win32 Release"
# Name "mvl - Win32 Debug"
# Name "mvl - Win32 ReleaseWithDBInfo"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
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

SOURCE=.\FMPlanarComputeNonLinear.cxx
# End Source File
# Begin Source File

SOURCE=.\FMPlanarNonLinFun.cxx
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

SOURCE=".\Templates\ModifyHandle+double-.cxx"
# End Source File
# Begin Source File

SOURCE=.\ModifyHandle.cxx
# End Source File
# Begin Source File

SOURCE=.\NViewMatches.cxx
# End Source File
# Begin Source File

SOURCE=.\pair_float_int.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\pair_float_int_things.cxx
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

SOURCE=.\PMatrix.cxx
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

SOURCE=.\ProjectiveBasis2D.cxx
# End Source File
# Begin Source File

SOURCE=.\ProjStructure.cxx
# End Source File
# Begin Source File

SOURCE=.\SimilarityMetric.cxx
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

SOURCE=.\TriTensor.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_array_2d+HomgPoint2D-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_array_2d+PairMatchSetLineSeg~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_array_2d+vnl_matrix+double--.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_array_2d+vnl_vector+double--.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array+FMatrix~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array+HMatrix2D~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array+HMatrix3D-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array+ImageMetric~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array+PairMatchSetCorner~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array+PairMatchSetLineSeg~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array_2d+FMatrix~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array_2d+HMatrix2D~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array_2d+HMatrix3D-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array_2d+PairMatchSetCorner~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array_2d+PairMatchSetLineSeg~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array_3d+TripleMatchSetCorner~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array_3d+TripleMatchSetLineSeg~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_sparse_array_3d+TriTensor~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_algo+ImageMetric~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_algo+ImageMetric~~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_list+HomgLineSeg2D-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_list+HomgMatchPoint3D2D~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_list+HomgPlane3D~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_list+HomgPoint2D-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_list+HomgPoint3D~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_list+LineSeg~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_map+uint.FMatrix~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_map+uint.HMatrix2D~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_map+uint.HMatrix3D-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_map+uint.HomgInterestPointSet-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_map+uint.ImageMetric~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_map+uint.LineSegSet-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_map+uint.PairMatchSetCorner~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_map+uint.PairMatchSetLineSeg~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_map+uint.PMatrix-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_map+uint.TripleMatchSetCorner~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_map+uint.TripleMatchSetLineSeg~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_map+uint.TriTensor-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_map+uint.TriTensor~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+FMatrix~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+HMatrix1D-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+HMatrix2D-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+HomgInterestPoint-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+HomgInterestPointSet~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+HomgLine2D-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+HomgLine3D-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+HomgLineSeg2D-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+HomgLineSeg3D-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+HomgMetric-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+HomgPlane3D-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+HomgPoint1D-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+HomgPoint2D-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+HomgPoint3D-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+ImageMetric~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+NViewMatch-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+PairMatchMulti-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+PMatrix-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+PMatrix~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+TriTensor-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+vcl_vector+HomgPoint2D--.cxx"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AffineMetric.h
# End Source File
# Begin Source File

SOURCE=.\ClosestImagePointFinder.h
# End Source File
# Begin Source File

SOURCE=.\FDesignMatrix.h
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

SOURCE=.\FMatrixCompute8Point.h
# End Source File
# Begin Source File

SOURCE=.\FMatrixComputeLinear.h
# End Source File
# Begin Source File

SOURCE=.\FMatrixPlanar.h
# End Source File
# Begin Source File

SOURCE=.\FMatrixSkew.h
# End Source File
# Begin Source File

SOURCE=.\FMPlanarComputeNonLinear.h
# End Source File
# Begin Source File

SOURCE=.\FMPlanarNonLinFun.h
# End Source File
# Begin Source File

SOURCE=.\FMSkewComputeLinear.h
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

SOURCE=.\HomgConic.h
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

SOURCE=.\HomgMap2D.h
# End Source File
# Begin Source File

SOURCE=.\HomgMatchPoint3D2D.h
# End Source File
# Begin Source File

SOURCE=.\HomgMetric.h
# End Source File
# Begin Source File

SOURCE=.\HomgNorm2D.h
# End Source File
# Begin Source File

SOURCE=.\HomgOperator1D.h
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

SOURCE=.\HomgPoint1D.h
# End Source File
# Begin Source File

SOURCE=.\HomgPoint2D.h
# End Source File
# Begin Source File

SOURCE=.\HomgPoint3D.h
# End Source File
# Begin Source File

SOURCE=.\HomgPrettyPrint.h
# End Source File
# Begin Source File

SOURCE=.\ImageMetric.h
# End Source File
# Begin Source File

SOURCE=.\LineSeg.h
# End Source File
# Begin Source File

SOURCE=.\LineSegSet.h
# End Source File
# Begin Source File

SOURCE=.\MatchSet.h
# End Source File
# Begin Source File

SOURCE=.\ModifyHandle.h
# End Source File
# Begin Source File

SOURCE=.\NViewMatches.h
# End Source File
# Begin Source File

SOURCE=.\pair_float_int.h
# End Source File
# Begin Source File

SOURCE=.\PairMatchMulti.h
# End Source File
# Begin Source File

SOURCE=.\PairMatchMultiIterator.h
# End Source File
# Begin Source File

SOURCE=.\PairMatchMultiStrength.h
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

SOURCE=.\PairMatchSetMulti.h
# End Source File
# Begin Source File

SOURCE=.\PairSetCorner.h
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

SOURCE=.\PMatrixCompute6Point.h
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

SOURCE=.\ProjectiveBasis2D.h
# End Source File
# Begin Source File

SOURCE=.\ProjStructure.h
# End Source File
# Begin Source File

SOURCE=.\SimilarityMetric.h
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

SOURCE=.\TriTensor.h
# End Source File
# Begin Source File

SOURCE=.\TriTensorCompute.h
# End Source File
# End Group
# End Target
# End Project
