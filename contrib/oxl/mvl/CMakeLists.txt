# ./oxl/mvl/CMakeLists.txt

ADD_LIBRARY(mvl mvl_sources)

SOURCE_FILES( mvl_sources
  AffineMetric.cxx
  ClosestImagePointFinder.cxx
  FDesignMatrix.cxx
  FMPlanarComputeNonLinear.cxx
  FMPlanarNonLinFun.cxx
  FManifoldProject.cxx
  FMatrix.cxx
  FMatrixAffine.cxx
  FMatrixCompute.cxx
  FMatrixComputeLinear.cxx
  FMatrixPlanar.cxx
  FMatrixSkew.cxx
  HMatrix1D.cxx
  HMatrix1DCompute.cxx
  HMatrix1DCompute3Point.cxx
  HMatrix1DComputeDesign.cxx
  HMatrix1DComputeOptimize1.cxx
  HMatrix2D.cxx
  HMatrix2DAffineCompute.cxx
  HMatrix2DCompute.cxx
  HMatrix2DCompute4Line.cxx
  HMatrix2DCompute4Point.cxx
  HMatrix2DComputeLinear.cxx
  HMatrix2DEuclideanCompute.cxx
  HMatrix2DSimilarityCompute.cxx
  HMatrix3D.cxx
  Homg.cxx
  Homg1D.cxx
  Homg2D.cxx
  Homg3D.cxx
  HomgInterestPoint.cxx
  HomgInterestPointSet.cxx
  HomgLine2D.cxx
  HomgLine3D.cxx
  HomgLineSeg2D.cxx
  HomgLineSeg3D.cxx
  HomgMetric.cxx
  HomgNorm2D.cxx
  HomgOperator2D.cxx
  HomgOperator3D.cxx
  HomgPlane3D.cxx
  HomgPoint1D.cxx
  HomgPoint2D.cxx
  HomgPoint3D.cxx
  HomgPrettyPrint.cxx
  ImageMetric.cxx
  LineSegSet.cxx
  MatchSet.cxx
  NViewMatches.cxx
  PMatrix.cxx
  PMatrixAffine.cxx
  PMatrixCompute.cxx
  PMatrixComputeLinear.cxx
  PMatrixDec.cxx
  PMatrixDecompAa.cxx
  PMatrixDecompCR.cxx
  PMatrixEuclidean.cxx
  PairMatchMulti.cxx
  PairMatchMultiIterator.cxx
  PairMatchSet.cxx
  PairMatchSet2D3D.cxx
  PairMatchSetCorner.cxx
  PairMatchSetLineSeg.cxx
  PairSetCorner.cxx
  ProjStructure.cxx
  ProjectiveBasis2D.cxx
  SimilarityMetric.cxx
  TriTensor.cxx
  TriTensorCompute.cxx
  TripleMatchSet.cxx
  TripleMatchSetCorner.cxx
  TripleMatchSetLineSeg.cxx
  mvl_five_point_camera_pencil.cxx
  mvl_multi_view_matches.cxx
  #    gives an error under Win32 (?)
  mvl_psi.cxx
  mvl_six_point_design_matrix_row.cxx
  mvl_three_view_six_point_structure.cxx
  ComputeGRIC.cxx
  FMatrixCompute7Point.cxx
  FMatrixComputeLMedSq.cxx
  FMatrixComputeMLESAC.cxx
  FMatrixComputeNonLinear.cxx
  FMatrixComputeRANSAC.cxx
  FMatrixComputeRobust.cxx
  HMatrix2DComputeMLESAC.cxx
  HMatrix2DComputeRobust.cxx
  Probability.cxx
  FileNameGeneratorBase.cxx
  FileNameGenerator.cxx
  RawPMatrixStore.cxx
)

AUX_SOURCE_DIRECTORY(Templates mvl_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/oxl/mvl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

IF( BUILD_TESTING )
#  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
