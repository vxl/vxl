# mul/vil3d/algo/CMakeLists.txt

SET(vil3d_algo_sources
  vil3d_gauss_reduce.h
  vil3d_algo_dummy.cxx
)

AUX_SOURCE_DIRECTORY(Templates vil3d_algo_sources)

ADD_LIBRARY(vil3d_algo ${vil3d_algo_sources})
TARGET_LINK_LIBRARIES( vil3d_algo vil3d )

#IF( BUILD_TESTING)
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING)
