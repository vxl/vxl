# brl/bseg/boxm/algo/CMakeLists.txt
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
SET(boxm_algo_sources
    boxm_compute_scene_statistics.h
    boxm_crop_scene.h
    boxm_fill_in_mesh.h
    boxm_fill_internal_cells.h
    boxm_init_scene.h
    boxm_refine.h
    boxm_save_block_raw.h
    boxm_save_scene_raw.h
    boxm_upload_mesh.h
    boxm_merge_mog.h                        boxm_merge_mog.cxx
    boxm_split_sample.h
    boxm_compute_scene_difference.h
   )

ADD_LIBRARY(boxm_algo ${boxm_algo_sources})

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

TARGET_LINK_LIBRARIES(boxm_algo boxm boxm_util boxm_sample boxm_sample_algo boxm_basic bsta imesh vpgl bgeo vgl_algo vgl vnl vbl vcl)

#raytracing algorithms
SUBDIRS(rt)

#splatting algorithms
SUBDIRS(sp)

#processes
SUBDIRS(pro)
