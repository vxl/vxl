# mul/vil2/algo/CMakeLists.txt

SET( vil2_algo_sources
  vil2_algo_sobel_3x3.cxx  vil2_algo_sobel_3x3.h
)


AUX_SOURCE_DIRECTORY(Templates vil2_algo_sources)

ADD_LIBRARY(vil2_algo ${vil2_algo_sources})

TARGET_LINK_LIBRARIES( vil2_algo vil2 vil vcl )


#IF( BUILD_EXAMPLES )
#  SUBDIRS( examples )
#ENDIF( BUILD_EXAMPLES )
#IF( BUILD_TESTING )
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING )
