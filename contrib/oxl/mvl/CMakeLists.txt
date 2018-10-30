# oxl/mvl/CMakeLists.txt

doxygen_add_library(contrib/oxl/mvl
  DEPENDS core/vnl core/vbl core/vgl core/vul core/vil1
  PACKAGE contrib/oxl
  DESCRIPTION "Multi-view epipolar geometry"
  )

set(mvl_sources
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
                                           Homg2D.h
                                           Homg3D.h
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
                                           MatchSet.h
    NViewMatches.cxx                       NViewMatches.h
    PMatrix.cxx                            PMatrix.h      PMatrix_sptr.h
                                           PMatrixAffine.h
                                           PMatrixEuclidean.h
                                           PMatrixCompute.h
    PMatrixComputeLinear.cxx               PMatrixComputeLinear.h PMatrixCompute6Point.h
    PMatrixDec.cxx                         PMatrixDec.h
    PMatrixDecompAa.cxx                    PMatrixDecompAa.h
    PMatrixDecompCR.cxx                    PMatrixDecompCR.h
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
                                           FileNameGenerator.h
    RawPMatrixStore.cxx                    RawPMatrixStore.h
    mvl_linear_backproject.cxx             mvl_linear_backproject.h

    HomgMap2D.h
    HomgMatchPoint3D2D.h
    LineSeg.h
    ModifyHandle.h
    PairMatchMultiStrength.h
    PairMatchSetMulti.h
    mvl_modify_handle.h
)
aux_source_directory(Templates mvl_sources)

if(CMAKE_COMPILER_IS_GNUCXX)
  set_source_files_properties(FMatrixComputeRobust.cxx PROPERTIES COMPILE_FLAGS -O0)
endif()

vxl_add_library(LIBRARY_NAME mvl LIBRARY_SOURCES ${mvl_sources})
target_link_libraries(mvl ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vul)

if(VXL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
