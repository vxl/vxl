if(HAS_GEOTIFF)
include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
if(OPENCL_FOUND)

  add_definitions(-DHAS_OPENCL=1)
  include_directories( ${OPENCL_INCLUDE_PATH} )
  include_directories( ${GEL_INCLUDE_DIR}/mrc )
  include_directories( ${MUL_INCLUDE_DIR} )

  #find expatpp library
  include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  if(EXPAT_FOUND)
    include_directories( ${EXPAT_INCLUDE_DIR} )
    include_directories( ${BRL_INCLUDE_DIR}/b3p/expatpp )

    # executable to convert a bvxm scene to a boxm2_scene
    add_executable( bvxm_to_boxm2_exe bvxm_to_boxm2.cxx  bvxm_to_boxm2.h)
    target_link_libraries( bvxm_to_boxm2_exe boxm2 bvxm bvxm_io bvxm_grid_io bvxm_algo boxm2_io boct ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vcl)

  endif()


endif()
else()
  message(STATUS "Skipping contrib/brl/bseg/bvxm_bridge: requires geotiff")
endif()
