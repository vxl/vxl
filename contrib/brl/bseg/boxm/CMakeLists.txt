# contrib/brl/bseg/boxm/CMakeLists.txt
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)

INCLUDE_DIRECTORIES( ${EXPAT_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p/expatpp )

SET(boxm_sources
    boxm_block.txx                         boxm_block.h
    boxm_scene_base.cxx                    boxm_scene_base.h
    boxm_scene.txx                         boxm_scene.h
    boxm_aux_scene.txx                     boxm_aux_scene.h
    boxm_scene_parser.cxx                  boxm_scene_parser.h
    boxm_apm_traits.cxx                    boxm_apm_traits.h
    boxm_aux_traits.h
    boxm_io_scene_base.h                   boxm_io_scene_base.cxx
)

AUX_SOURCE_DIRECTORY(Templates boxm_sources)

ADD_LIBRARY(boxm  ${boxm_sources})

TARGET_LINK_LIBRARIES(boxm boct bsta brdb vgl_io vgl_xio vpgl vpgl_xio vpgl_algo vsol expatpp vgl_algo vnl_algo vnl vgl vul vil vbl_io vbl vsl vpl vcl)

#processes
SUBDIRS(pro)

#util
SUBDIRS(util)

#samples
SUBDIRS(sample)

#basic algorithms involving scenes
SUBDIRS(algo)

#basic structures
SUBDIRS(basic)


#tests
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF(EXPAT_FOUND)

#install the .h .txx and libs
INSTALL_TARGETS(/lib boxm)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm/ ${boxm_sources})
