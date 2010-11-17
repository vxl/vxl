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
        boxm2_scene.h                   boxm2_scene.cxx
        boxm2_block.h                   boxm2_block.cxx
	boxm2_data.h boxm2_data_traits.h       boxm2_data.txx		
        boxm2_scene_parser.h            boxm2_scene_parser.cxx
        boxm2_asio_mgr.h                boxm2_asio_mgr.cxx
        boxm2_sio_mgr.h                 boxm2_sio_mgr.cxx
        boxm2_block_id.h                boxm2_block_id.cxx
        boxm2_cache.h                   boxm2_cache.cxx
        boxm2_dumb_cache.h              boxm2_dumb_cache.cxx

        boxm2_disk_io_mgr.h             boxm2_disk_io_mgr.cxx

        boxm2_array_1d.h                boxm2_array_1d.txx
        boxm2_array_2d.h                boxm2_array_2d.txx
        boxm2_array_3d.h                boxm2_array_3d.txx
       )

    AUX_SOURCE_DIRECTORY(Templates boxm2_sources)

    ADD_LIBRARY(boxm2  ${boxm2_sources})
    TARGET_LINK_LIBRARIES(boxm2 expatpp vpgl vgl vgl_xio vbl vsl vcl baio) 

    #tests
    SUBDIRS(tests)

  ENDIF(OPENCL_FOUND)
ENDIF(EXPAT_FOUND)
