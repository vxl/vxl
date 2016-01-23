# contrib/brl/bseg/boxm/CMakeLists.txt
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)

include_directories( ${EXPAT_INCLUDE_DIR} )
include_directories( ${GEL_INCLUDE_DIR} )
include_directories( ${GEL_INCLUDE_DIR}/mrc )
include_directories( ${BRL_INCLUDE_DIR}/b3p/expatpp )

set(boxm_sources
    boxm_export.h
    boxm_block.txx                         boxm_block.h
    boxm_scene_base.cxx                    boxm_scene_base.h
    boxm_scene.txx                         boxm_scene.h
    boxm_aux_scene.txx                     boxm_aux_scene.h
    boxm_scene_parser.cxx                  boxm_scene_parser.h
    boxm_apm_traits.cxx                    boxm_apm_traits.h
    boxm_aux_traits.h
    boxm_io_scene_base.h                   boxm_io_scene_base.cxx
)

aux_source_directory(Templates boxm_sources)

add_library(boxm  ${boxm_sources})

target_link_libraries(boxm boct bsta brdb vgl_io vgl_xio vpgl vpgl_xio vpgl_algo vsol expatpp vgl_algo vnl_algo vnl vgl vul vil vbl_io vbl vsl vpl vcl)

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
endif( BUILD_TESTING )

endif(EXPAT_FOUND)

#install the .h .txx and libs
install_targets(/lib boxm)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm/ ${boxm_sources})
