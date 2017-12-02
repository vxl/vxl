# This is contrib/brl/bseg/boxm2/volm/CMakeLists
# volm depends on bkml and expat
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  if(EXPATPP_FOUND)
    #depends on OPENCL being found...
    include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
    if(OPENCL_FOUND)
    add_definitions(-DHAS_OPENCL=1)
    include_directories( ${OPENCL_INCLUDE_PATH} )
    endif()

    # contrib/brl/bseg/boxm2/volm/CMakeLists.txt
    include_directories( ${GEL_INCLUDE_DIR} )
    include_directories( ${GEL_INCLUDE_DIR}/mrc )

    include_directories( ${BRL_INCLUDE_DIR}/bbas )
    include_directories( ${BRL_INCLUDE_DIR}/bseg )

    set(boxm2_volm_sources
        boxm2_volm_locations.h boxm2_volm_locations.cxx
        boxm2_volm_locations_sptr.h
        boxm2_volm_wr3db_index.h        boxm2_volm_wr3db_index.cxx
        boxm2_volm_wr3db_index_sptr.h
        boxm2_volm_matcher_p0.h         boxm2_volm_matcher_p0.cxx
       )

    if(OPENCL_FOUND)
    set(boxm2_volm_sources ${boxm2_volm_sources}
        boxm2_volm_matcher_p1.h         boxm2_volm_matcher_p1.cxx
       )
    endif()

    aux_source_directory(Templates boxm2_volm_sources)

    vxl_add_library(LIBRARY_NAME boxm2_volm LIBRARY_SOURCES ${boxm2_volm_sources})
    target_link_libraries(boxm2_volm boxm2 boxm2_io brip baio ${VXL_LIB_PREFIX}vpgl volm ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl depth_map)
    if(OPENCL_FOUND)
    target_link_libraries(boxm2_volm bocl)
    endif()

    #install the .h .hxx and libs

    add_subdirectory(io)

    if( BUILD_TESTING )
      add_subdirectory(tests)
    endif()

    add_subdirectory(exe)

    #pro
    add_subdirectory(pro)

    #desc matcher
    add_subdirectory(desc)

    #conf matcher
    add_subdirectory(conf)
  endif()
endif()
