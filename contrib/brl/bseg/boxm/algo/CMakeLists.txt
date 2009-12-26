# brl/bseg/boxm/algo/CMakeLists.txt

SET(boxm_algo_sources
    boxm_split_sample.h
    boxm_merge_mog.h           boxm_merge_mog.cxx
    boxm_vrml_util.h
   )

ADD_LIBRARY(boxm_algo ${boxm_algo_sources})

TARGET_LINK_LIBRARIES(boxm_algo boxm bvrml vnl vil)

#processes
SUBDIRS(pro)
