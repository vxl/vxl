# Packages used by this package
USES += oxl vxl

LIBRARY := mvl
VERSION := 1.0

IMMEDIATE_LIBS += -lvnl-algo -lvnl -lvgl-algo -lvgl -lvbl

# 0. stuff written in C++

SOURCES += mvl_five_point_camera_pencil.cxx
SOURCES += mvl_psi.cxx
SOURCES += mvl_six_point_design_matrix_row.cxx
SOURCES += mvl_three_view_six_point_structure.cxx
#SOURCES += $(wildcard mvl_mview_six_point_structure.cxx)
#SOURCES += $(wildcard mvl_image_point_conditioner.cxx)

# 1. Homogeneous objects

SOURCES += Homg.cxx
SOURCES += Homg1D.cxx
#SOURCES += HomgPoint1D.cxx

SOURCES += Homg2D.cxx
SOURCES += HomgLine2D.cxx
SOURCES += HomgLineSeg2D.cxx
SOURCES += HomgPoint2D.cxx
SOURCES += HomgOperator2D.cxx

SOURCES += Homg3D.cxx
SOURCES += HomgLine3D.cxx
SOURCES += HomgLineSeg3D.cxx
SOURCES += HomgPoint3D.cxx
SOURCES += HomgPlane3D.cxx
SOURCES += HomgOperator3D.cxx
SOURCES += HomgPrettyPrint.cxx

# 5. Containers

SOURCES += HomgInterestPoint.cxx
SOURCES += HomgInterestPointSet.cxx
SOURCES += LineSegSet.cxx
SOURCES += ProjStructure.cxx

# 2. Between-view relations

SOURCES += FMatrix.cxx
SOURCES += FManifoldProject.cxx
SOURCES += FMatrixAffine.cxx
SOURCES += FMatrixSkew.cxx
SOURCES += FMatrixPlanar.cxx

SOURCES += HMatrix1D.cxx
SOURCES += HMatrix2D.cxx
SOURCES += HMatrix3D.cxx

SOURCES += PMatrix.cxx
SOURCES += PMatrixDecompAa.cxx
SOURCES += PMatrixDec.cxx
SOURCES += PMatrixDecompCR.cxx
SOURCES += PMatrixAffine.cxx
SOURCES += PMatrixEuclidean.cxx

SOURCES += TriTensor.cxx
SOURCES += ProjectiveBasis2D.cxx

# 3. Representing matches between homogeneous objects

SOURCES += PairSetCorner.cxx
SOURCES += PairMatchSet.cxx
SOURCES += PairMatchSetLineSeg.cxx
SOURCES += PairMatchSetCorner.cxx
SOURCES += PairMatchSet2D3D.cxx
SOURCES += PairMatchMulti.cxx
SOURCES += PairMatchMultiIterator.cxx
SOURCES += MatchSet.cxx
SOURCES += TripleMatchSet.cxx
SOURCES += TripleMatchSetCorner.cxx
SOURCES += TripleMatchSetLineSeg.cxx
SOURCES += NViewMatches.cxx
SOURCES += mvl_multi_view_matches.cxx

# 4. Classes to perform image measurements on homogeneous points.

SOURCES += HomgMetric.cxx
SOURCES += ImageMetric.cxx
SOURCES += SimilarityMetric.cxx
SOURCES += AffineMetric.cxx
SOURCES += ClosestImagePointFinder.cxx

# 6. Computors

SOURCES += FMatrixCompute.cxx
SOURCES += FMatrixComputeLinear.cxx
SOURCES += FMatrixCompute7Point.cxx
SOURCES += FMatrixComputeLMedSq.cxx
SOURCES += FMatrixComputeMLESAC.cxx
SOURCES += FMatrixComputeNonLinear.cxx
SOURCES += FMatrixComputeRANSAC.cxx
SOURCES += FMatrixComputeRobust.cxx
SOURCES += FMPlanarComputeNonLinear.cxx
SOURCES += FMPlanarNonLinFun.cxx
SOURCES += FDesignMatrix.cxx

SOURCES += HMatrix1DCompute.cxx
SOURCES += HMatrix1DCompute3Point.cxx
SOURCES += HMatrix1DComputeDesign.cxx
SOURCES += HMatrix1DComputeOptimize1.cxx

SOURCES += HomgNorm2D.cxx
SOURCES += HMatrix2DCompute.cxx
SOURCES += HMatrix2DCompute4Line.cxx
SOURCES += HMatrix2DCompute4Point.cxx
SOURCES += HMatrix2DComputeLinear.cxx
SOURCES += HMatrix2DComputeMLESAC.cxx
SOURCES += HMatrix2DComputeRobust.cxx
SOURCES += HMatrix2DAffineCompute.cxx
SOURCES += HMatrix2DSimilarityCompute.cxx
SOURCES += HMatrix2DEuclideanCompute.cxx

SOURCES += PMatrixCompute.cxx
SOURCES += PMatrixComputeLinear.cxx

SOURCES += TriTensorCompute.cxx

# 7. Misc utility functions

SOURCES += ComputeGRIC.cxx
SOURCES += Probability.cxx
SOURCES += FileNameGeneratorBase.cxx
SOURCES += FileNameGenerator.cxx
SOURCES += RawPMatrixStore.cxx

MANPAGE_SOURCES += FMSkewComputeLinear.h
MANPAGE_SOURCES += FMatrixCompute8Point.h
MANPAGE_SOURCES += HomgMap2D.h
MANPAGE_SOURCES += HomgMatchPoint3D2D.h
MANPAGE_SOURCES += LineSeg.h
MANPAGE_SOURCES += ModifyHandle.h
MANPAGE_SOURCES += PMatrixCompute6Point.h
MANPAGE_SOURCES += PairMatchMultiStrength.h
MANPAGE_SOURCES += PairMatchSetMulti.h
MANPAGE_SOURCES += mvl_modify_handle.h

MANPAGE_SOURCES += PMatrix_sptr.h

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifeq "$(TJ_GCC30)" "1"
FLAGS_FMatrixComputeRobust.cxx += -O0 # otherwise gcc 3.0 needs too much virtual memory
endif

ifndef NOTEST
SUBDIRS = examples # tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

ifeq "HP-UX" "$(findstring HP-UX,$(OS))"
pic = $(PIC)
endif

ifeq ($(IU_COMPILER),KCC)
pic = $(PIC)
endif
