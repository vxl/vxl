# brl/bseg/boxm/algo/CMakeLists.txt
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
SET(boxm_algo_sources
    boxm_split_sample.h
    boxm_merge_mog.h           boxm_merge_mog.cxx
    boxm_vrml_util.h
    boxm_crop_scene.h
    boxm_scene_to_bvxm_grid.h
   )

ADD_LIBRARY(boxm_algo ${boxm_algo_sources})

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

TARGET_LINK_LIBRARIES(boxm_algo boxm bvrml vnl vil)


#processes
SUBDIRS(pro)
