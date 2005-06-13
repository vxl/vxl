# ./brl/bbas/bsta/CMakeLists.txt
# A library for basic statistics classes and methods

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)

SET(bsta_sources
 bsta_gauss.cxx            bsta_gauss.h
 bsta_histogram.txx        bsta_histogram.h
 bsta_joint_histogram.txx  bsta_joint_histogram.h
 bsta_k_medoid.cxx         bsta_k_medoid.h
)
AUX_SOURCE_DIRECTORY(Templates bsta_sources)

ADD_LIBRARY(bsta ${bsta_sources})

TARGET_LINK_LIBRARIES(bsta vnl vbl vcl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

