# oxl/mvl/CMakeLists.txt

SET(mvl_sources
    AffineMetric.cxx                       AffineMetric.h
    ClosestImagePointFinder.cxx            ClosestImagePointFinder.h
    FDesignMatrix.cxx                      FDesignMatrix.h
    FMPlanarComputeNonLinear.cxx           FMPlanarComputeNonLinear.h
    FMPlanarNonLinFun.cxx                  FMPlanarNonLinFun.h
    FManifoldProject.cxx                   FManifoldProject.h
    FMatrix.cxx                            FMatrix.h
    FMatrixAffine.cxx                      FMatrixAffine.h
    FMatrixCompute.cxx                     FMatrixCompute.h
    FMatrixComputeLinear.cxx               FMatrixComputeLinear.h FMatrixCompute8Point.h
    FMatrixPlanar.cxx                      FMatrixPlanar.h
    FMatrixSkew.cxx                        FMatrixSkew.h
    HMatrix1D.cxx                          HMatrix1D.h
    HMatrix1DCompute.cxx                   HMatrix1DCompute.h
    HMatrix1DCompute3Point.cxx             HMatrix1DCompute3Point.h
    HMatrix1DComputeDesign.cxx             HMatrix1DComputeDesign.h
    HMatrix1DComputeOptimize1.cxx          HMatrix1DComputeOptimize1.h
    HMatrix2D.cxx                          HMatrix2D.h
    HMatrix2DAffineCompute.cxx             HMatrix2DAffineCompute.h
    HMatrix2DCompute.cxx                   HMatrix2DCompute.h
    HMatrix2DCompute4Line.cxx              HMatrix2DCompute4Line.h
    HMatrix2DCompute4Point.cxx             HMatrix2DCompute4Point.h
    HMatrix2DComputeLinear.cxx             HMatrix2DComputeLinear.h
    HMatrix2DEuclideanCompute.cxx          HMatrix2DEuclideanCompute.h
    HMatrix2DSimilarityCompute.cxx         HMatrix2DSimilarityCompute.h
    HMatrix3D.cxx                          HMatrix3D.h
    Homg.cxx                               Homg.h
    Homg2D.cxx                             Homg2D.h
    Homg3D.cxx                             Homg3D.h
    HomgPoint2D.cxx                        HomgPoint2D.h
    HomgPoint3D.cxx                        HomgPoint3D.h
    HomgInterestPoint.cxx                  HomgInterestPoint.h
    HomgInterestPointSet.cxx               HomgInterestPointSet.h
    HomgLine2D.cxx                         HomgLine2D.h
    HomgLine3D.cxx                         HomgLine3D.h
    HomgLineSeg2D.cxx                      HomgLineSeg2D.h
    HomgLineSeg3D.cxx                      HomgLineSeg3D.h
    HomgMetric.cxx                         HomgMetric.h
    HomgNorm2D.cxx                         HomgNorm2D.h
    HomgOperator2D.cxx                     HomgOperator2D.h
    HomgOperator3D.cxx                     HomgOperator3D.h
    HomgPlane3D.cxx                        HomgPlane3D.h
    HomgPrettyPrint.cxx                    HomgPrettyPrint.h
    ImageMetric.cxx                        ImageMetric.h
    LineSegSet.cxx                         LineSegSet.h
    MatchSet.cxx                           MatchSet.h
    NViewMatches.cxx                       NViewMatches.h
    PMatrix.cxx                            PMatrix.h PMatrix_sptr.h
    PMatrixAffine.cxx                      PMatrixAffine.h
    PMatrixCompute.cxx                     PMatrixCompute.h
    PMatrixComputeLinear.cxx               PMatrixComputeLinear.h PMatrixCompute6Point.h
    PMatrixDec.cxx                         PMatrixDec.h
    PMatrixDecompAa.cxx                    PMatrixDecompAa.h
    PMatrixDecompCR.cxx                    PMatrixDecompCR.h
    PMatrixEuclidean.cxx                   PMatrixEuclidean.h
    PairMatchMulti.cxx                     PairMatchMulti.h
    PairMatchMultiIterator.cxx             PairMatchMultiIterator.h
    PairMatchSet.cxx                       PairMatchSet.h
    PairMatchSet2D3D.cxx                   PairMatchSet2D3D.h
    PairMatchSetCorner.cxx                 PairMatchSetCorner.h
    PairMatchSetLineSeg.cxx                PairMatchSetLineSeg.h
    PairSetCorner.cxx                      PairSetCorner.h
    ProjStructure.cxx                      ProjStructure.h
    ProjectiveBasis2D.cxx                  ProjectiveBasis2D.h
    SimilarityMetric.cxx                   SimilarityMetric.h
    TriTensor.cxx                          TriTensor.h
    TriTensorCompute.cxx                   TriTensorCompute.h
    TripleMatchSet.cxx                     TripleMatchSet.h
    TripleMatchSetCorner.cxx               TripleMatchSetCorner.h
    TripleMatchSetLineSeg.cxx              TripleMatchSetLineSeg.h
    mvl_five_point_camera_pencil.cxx       mvl_five_point_camera_pencil.h
    mvl_multi_view_matches.cxx             mvl_multi_view_matches.h
    #    gives an error under Win32 (?)
    mvl_psi.cxx                            mvl_psi.h
    mvl_six_point_design_matrix_row.cxx    mvl_six_point_design_matrix_row.h
    mvl_three_view_six_point_structure.cxx mvl_three_view_six_point_structure.h
    ComputeGRIC.cxx                        ComputeGRIC.h
    FMatrixCompute7Point.cxx               FMatrixCompute7Point.h
    FMatrixComputeLMedSq.cxx               FMatrixComputeLMedSq.h
    FMatrixComputeMLESAC.cxx               FMatrixComputeMLESAC.h
    FMatrixComputeNonLinear.cxx            FMatrixComputeNonLinear.h
    FMatrixComputeRANSAC.cxx               FMatrixComputeRANSAC.h
    FMatrixComputeRobust.cxx               FMatrixComputeRobust.h
    HMatrix2DComputeMLESAC.cxx             HMatrix2DComputeMLESAC.h
    HMatrix2DComputeRobust.cxx             HMatrix2DComputeRobust.h
    Probability.cxx                        Probability.h
    FileNameGeneratorBase.cxx              FileNameGeneratorBase.h
    FileNameGenerator.cxx                  FileNameGenerator.h
    RawPMatrixStore.cxx                    RawPMatrixStore.h

    HomgMap2D.h
    HomgMatchPoint3D2D.h
    LineSeg.h
    ModifyHandle.h
    PairMatchMultiStrength.h
    PairMatchSetMulti.h
    mvl_modify_handle.h
)
AUX_SOURCE_DIRECTORY(Templates mvl_sources)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(FMatrixComputeRobust.cxx PROPERTIES COMPILE_FLAGS -O0)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY(mvl ${mvl_sources})
TARGET_LINK_LIBRARIES(mvl vil1 vnl_algo vnl vbl vgl vgl_algo vul)

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)
