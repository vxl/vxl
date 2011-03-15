# bbas/bgrl2/algo/CMakeLists.txt

SET( bgrl2_algo_sources
  bgrl2_algs.cxx              bgrl2_algs.h
)

ADD_LIBRARY( bgrl2_algo  ${bgrl2_algo_sources})

TARGET_LINK_LIBRARIES( bgrl2_algo bgrl2 )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
