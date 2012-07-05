# Brown Elastic Curve Matching Library
# bbas/bcvr/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg)
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})

SET(bcvr_sources

bcvr_cvmatch.h
bcvr_cvmatch.cxx
bcvr_cvmatch_sptr.h

bcvr_clsd_cvmatch.h
bcvr_clsd_cvmatch.cxx
bcvr_clsd_cvmatch_sptr.h

bcvr_cv_cor.h
bcvr_cv_cor.cxx
bcvr_cv_cor_sptr.h

)

AUX_SOURCE_DIRECTORY(Templates bcvr_sources)

ADD_LIBRARY(bcvr  ${bcvr_sources})

TARGET_LINK_LIBRARIES(bcvr bsol vdgl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

