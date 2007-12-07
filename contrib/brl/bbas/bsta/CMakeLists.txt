# ./brl/bbas/bsta/CMakeLists.txt
# A library for basic statistics classes and methods

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})

SET(bsta_sources
 bsta_gauss.cxx            bsta_gauss.h
 bsta_histogram.txx        bsta_histogram.h
 bsta_joint_histogram.txx  bsta_joint_histogram.h
 bsta_k_medoid.cxx         bsta_k_medoid.h
 bsta_otsu_threshold.txx   bsta_otsu_threshold.h
 bsta_int_histogram_1d.cxx  bsta_int_histogram_1d.h
 bsta_int_histogram_2d.cxx  bsta_int_histogram_2d.h
)
AUX_SOURCE_DIRECTORY(Templates bsta_sources)

ADD_LIBRARY(bsta ${bsta_sources})
INSTALL_TARGETS(/lib bsta)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/bsta ${bsta_sources})

TARGET_LINK_LIBRARIES(bsta vnl vbl vcl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

