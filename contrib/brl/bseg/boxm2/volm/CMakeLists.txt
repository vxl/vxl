# This is contrib/brl/bseg/boxm2/volm/CMakeLists
# volm depends on bkml and expat
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  IF(EXPATPP_FOUND)
    #depends on OPENCL being found...
    INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
    IF(OPENCL_FOUND)
    ADD_DEFINITIONS(-DHAS_OPENCL=1)
    INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
    ENDIF(OPENCL_FOUND)

    # contrib/brl/bseg/boxm2/volm/CMakeLists.txt
    INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )

    SET(boxm2_volm_sources
        boxm2_volm_locations.h boxm2_volm_locations.cxx
        boxm2_volm_locations_sptr.h
        boxm2_volm_wr3db_index.h        boxm2_volm_wr3db_index.cxx
        boxm2_volm_wr3db_index_sptr.h
        boxm2_volm_matcher_p0.h         boxm2_volm_matcher_p0.cxx
       )

    IF(OPENCL_FOUND)
    SET(boxm2_volm_sources ${boxm2_volm_sources}
        boxm2_volm_matcher_p1.h         boxm2_volm_matcher_p1.cxx
       )
    ENDIF(OPENCL_FOUND)

    AUX_SOURCE_DIRECTORY(Templates boxm2_volm_sources)

    ADD_LIBRARY(boxm2_volm ${boxm2_volm_sources})
    TARGET_LINK_LIBRARIES(boxm2_volm boxm2 boxm2_io brip baio vpgl volm vnl vidl vil_algo vil vgl_algo vgl_xio vgl vbl vul vsl vcl depth_map)
    IF(OPENCL_FOUND)
    TARGET_LINK_LIBRARIES(boxm2_volm bocl)
    ENDIF(OPENCL_FOUND)

    #install the .h .txx and libs
    INSTALL_TARGETS(/lib boxm2_volm)
    INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2_volm/ ${boxm2_volm_sources})

    add_subdirectory(io)

    IF( BUILD_TESTING )
      add_subdirectory(tests)
    ENDIF( BUILD_TESTING )

    add_subdirectory(exe)

    #pro
    add_subdirectory(pro)

    #desc matcher
    add_subdirectory(desc)

    #conf matcher
    add_subdirectory(conf)
  ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)
