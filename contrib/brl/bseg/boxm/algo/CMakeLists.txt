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
    dummy.cxx
   )

ADD_LIBRARY(boxm_algo ${boxm_algo_sources})

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

TARGET_LINK_LIBRARIES(boxm_algo boxm boxm_util boxm_sample boxm_sample_algo boxm_basic vcl vgl bsta imesh vpgl bgeo vgl vgl_algo)

#raytracing algorithms
SUBDIRS(rt)

#splatting algorithms
SUBDIRS(sp)

#processes
SUBDIRS(pro)
