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
    endif(OPENCL_FOUND)

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
    endif(OPENCL_FOUND)

    aux_source_directory(Templates boxm2_volm_sources)

    add_library(boxm2_volm ${boxm2_volm_sources})
    target_link_libraries(boxm2_volm boxm2 boxm2_io brip baio vpgl volm vnl vidl vil_algo vil vgl_algo vgl_xio vgl vbl vul vsl vcl depth_map)
    if(OPENCL_FOUND)
    target_link_libraries(boxm2_volm bocl)
    endif(OPENCL_FOUND)

    #install the .h .txx and libs
    install_targets(/lib boxm2_volm)
    INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2_volm/ ${boxm2_volm_sources})

    add_subdirectory(io)

    if( BUILD_TESTING )
      add_subdirectory(tests)
    endif( BUILD_TESTING )

    add_subdirectory(exe)

    #pro
    add_subdirectory(pro)

    #desc matcher
    add_subdirectory(desc)

    #conf matcher
    add_subdirectory(conf)
  endif(EXPATPP_FOUND)
endif(EXPAT_FOUND)
