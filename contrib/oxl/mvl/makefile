# Packages used by this package 
USES += oxl vxl

LIBRARY := mvl
VERSION := 1.0

IMMEDIATE_LIBS += -lvnl-algo -lvnl -lvbl -lvcl

SUBDIRS = examples

# Utilities 
# FIXME these ought to go to GeneralUtility later 
SOURCES += ModifyHandle.cxx
SOURCES += pair_float_int.cxx

# 1. Homogeneous objects  

SOURCES += Homg.cxx
SOURCES += Homg1D.cxx
SOURCES += HomgPoint1D.cxx
SOURCES += HomgOperator1D.cxx

SOURCES += Homg2D.cxx
SOURCES += HomgLine2D.cxx
SOURCES += HomgLineSeg2D.cxx
SOURCES += HomgPoint2D.cxx
SOURCES += HomgOperator2D.cxx
SOURCES += HomgConic.cxx

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
SOURCES += LineSeg.cxx
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
#SOURCES += PMatrixAffine.cxx
#SOURCES += PMatrixEuclidean.cxx

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
#SOURCES += wank.cxx
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
SOURCES += HMatrix2DAffineCompute.cxx
SOURCES += HMatrix2DSimilarityCompute.cxx
SOURCES += HMatrix2DEuclideanCompute.cxx

SOURCES += PMatrixCompute.cxx
SOURCES += PMatrixComputeLinear.cxx

################################
include ${IUEROOT}/config/top.mk
################################

ifeq ($(OS),hpux10.20)
pic = $(PIC)
endif

ifeq ($(IU_COMPILER),KCC)
pic := $(PIC)
endif
