# mul/vil3d/algo/CMakeLists.txt

SET(vil3d_algo_sources
  vil3d_gauss_reduce.h
  vil3d_algo_dummy.cxx
  vil3d_threshold.h vil3d_threshold.txx
  vil3d_grad_1x3.h vil3d_grad_1x3.cxx vil3d_grad_1x3.txx
)

AUX_SOURCE_DIRECTORY(Templates vil3d_algo_sources)

ADD_LIBRARY(vil3d_algo ${vil3d_algo_sources})
TARGET_LINK_LIBRARIES( vil3d_algo vil3d )

#IF( BUILD_TESTING)
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING)
