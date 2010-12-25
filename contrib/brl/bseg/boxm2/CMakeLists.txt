# contrib/brl/bseg/boxm/CMakeLists.txt
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)

  INCLUDE_DIRECTORIES( ${EXPAT_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p/expatpp )

  INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
  IF(OPENCL_FOUND)
    ADD_DEFINITIONS(-DHAS_OPENCL=1)
    INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

    SET(boxm2_sources
        boxm2_scene.h                 boxm2_scene.cxx
        boxm2_block.h                 boxm2_block.cxx
        boxm2_block_metadata.h
        boxm2_data_base.h             boxm2_data_base.cxx
        boxm2_data.h                  boxm2_data.txx
        boxm2_data_traits.h
        boxm2_processor.h
        boxm2_process.h
        boxm2_opencl_processor.h      boxm2_opencl_processor.cxx
        boxm2_cpp_processor.h         boxm2_cpp_processor.cxx
        boxm2_opencl_process_base.h
        boxm2_cpp_process_base.h
        boxm2_opencl_render_process.h boxm2_opencl_render_process.cxx
        boxm2_cpp_render_process.h    boxm2_cpp_render_process.cxx
        boxm2_opencl_render_depth_process.h boxm2_opencl_render_depth_process.cxx
        boxm2_util.h                  boxm2_util.cxx
       )

    AUX_SOURCE_DIRECTORY(Templates boxm2_sources)

    ADD_LIBRARY(boxm2  ${boxm2_sources})
    TARGET_LINK_LIBRARIES(boxm2 boxm2_basic bocl brdb vbl_io expatpp vpgl vgl vgl_xio vbl vsl vcl)

    #basic
    subdirs(basic)

    #io
    subdirs(io)

    #tests
    SUBDIRS(tests)
    
    #exes
    subdirs(exe)
    
    #tableaus
    subdirs(view)

  ENDIF(OPENCL_FOUND)
ENDIF(EXPAT_FOUND)
