# Brown image processing library
# bseg/brip/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )

SET(brip_sources
   brip_histogram.h      brip_histogram.txx
   brip_mutual_info.h    brip_mutual_info.txx
   brip_float_ops.cxx    brip_float_ops.h
                         brip_vil_ops.h

)
AUX_SOURCE_DIRECTORY(Templates brip_sources)

ADD_LIBRARY(brip ${brip_sources})

TARGET_LINK_LIBRARIES(brip vil1 vil vnl_algo vnl vbl vul)


IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
