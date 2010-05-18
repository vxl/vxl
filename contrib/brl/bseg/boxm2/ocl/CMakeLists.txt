# boxm open_cl acceleration library

INCLUDE( ${BRL_MODULE_PATH}/find_open_cl.cmake )
IF(OPENCL_FOUND)
  ADD_DEFINITIONS(-DHAS_OPENCL=1)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

  SET(boxm2_ocl_sources
      boxm_ray_trace_manager.h boxm_ray_trace_manager.txx
      boxm_ocl_utils.h  boxm_ocl_utils.cxx
      boxm_ocl_render_expected.h
     )

  SET(bomx2_ocl_source_files
      octree_library_functions.cl
  )
  AUX_SOURCE_DIRECTORY(Templates boxm2_ocl_sources)
  ADD_LIBRARY(boxm2_ocl ${boxm2_ocl_sources} ${bomx2_ocl_source_files})

  TARGET_LINK_LIBRARIES(boxm2_ocl boxm2 boxm2_util boxm2_sample boxm2_basic boct bprb bgeo vnl vnl_algo vgl vbl vbl_io vpgl vul ${OPENCL_LIBRARIES})

SUBDIRS(tests)

SUBDIRS(pro)

ENDIF(OPENCL_FOUND)
