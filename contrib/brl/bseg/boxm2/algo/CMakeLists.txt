# brl/bseg/boxm2/algo/CMakeLists.txt
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
SET(boxm2_algo_sources
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

ADD_LIBRARY(boxm2_algo ${boxm2_algo_sources})

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

TARGET_LINK_LIBRARIES(boxm2_algo boxm2 boxm2_util boxm2_sample boxm2_sample_algo boxm2_basic vcl vgl bsta imesh vpgl bgeo vgl vgl_algo)

#raytracing algorithms
SUBDIRS(rt)

#splatting algorithms
SUBDIRS(sp)

#processes
SUBDIRS(pro)
