# 3D reconstruction from multiple views library
# brl/bmvl/brct/CMakeLists.txt

INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)

SET(brct_sources
    brct_algos.h    brct_algos.cpp
    kalman_filter.h kalman_filter.cpp
    brct_structure_estimator.h brct_structure_estimator.cxx
    brct_epi_reconstructor.h brct_epi_reconstructor.cxx
)

AUX_SOURCE_DIRECTORY(Templates brct_sources)

ADD_LIBRARY(brct ${brct_sources})

TARGET_LINK_LIBRARIES(brct vdgl vtol vsol vil1 vnl vbl mvl bdgl bugl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
