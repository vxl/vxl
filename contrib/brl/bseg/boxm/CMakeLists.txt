# contrib/brl/bseg/boxm/CMakeLists.txt

INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

SET(boxm_sources
    boxm_block.txx       boxm_block.h           
    boxm_scene.txx       boxm_scene.h
    
)

AUX_SOURCE_DIRECTORY(Templates boxm_sources)

ADD_LIBRARY(boxm  ${boxm_sources})

TARGET_LINK_LIBRARIES(boxm boct vcl vgl vgl_io vgl_xio vbl vsl vpgl bgeo)

#tests
SUBDIRS(tests)

