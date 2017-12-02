# contrib/brl/bseg/boxm/CMakeLists.txt
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)

include_directories( ${EXPAT_INCLUDE_DIR} )
include_directories( ${GEL_INCLUDE_DIR} )
include_directories( ${GEL_INCLUDE_DIR}/mrc )
include_directories( ${BRL_INCLUDE_DIR}/b3p/expatpp )

set(boxm_sources
    boxm_export.h
    boxm_block.hxx                         boxm_block.h
    boxm_scene_base.cxx                    boxm_scene_base.h
    boxm_scene.hxx                         boxm_scene.h
    boxm_aux_scene.hxx                     boxm_aux_scene.h
    boxm_scene_parser.cxx                  boxm_scene_parser.h
    boxm_apm_traits.cxx                    boxm_apm_traits.h
    boxm_aux_traits.h
    boxm_io_scene_base.h                   boxm_io_scene_base.cxx
)

aux_source_directory(Templates boxm_sources)

vxl_add_library(LIBRARY_NAME boxm LIBRARY_SOURCES  ${boxm_sources})

target_link_libraries(boxm boct bsta brdb ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_xio ${VXL_LIB_PREFIX}vpgl_algo vsol expatpp ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vcl)

#processes
add_subdirectory(pro)

#util
add_subdirectory(util)

#samples
add_subdirectory(sample)

#basic algorithms involving scenes
add_subdirectory(algo)

#basic structures
add_subdirectory(basic)


#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

endif()

#install the .h .hxx and libs
