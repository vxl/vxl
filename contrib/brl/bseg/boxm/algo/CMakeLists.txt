# brl/bseg/boxm/algo/CMakeLists.txt

SET(boxm_algo_sources
    boxm_split_sample.h
    boxm_merge_mog.h           boxm_merge_mog.cxx
    boxm_vrml_util.h           boxm_vrml_util.cxx
  )

#AUX_SOURCE_DIRECTORY(Templates boxm_opt_sources)

ADD_LIBRARY(boxm_algo ${boxm_algo_sources})

TARGET_LINK_LIBRARIES(boxm_algo boxm bvrml)

#processes
SUBDIRS(pro)

