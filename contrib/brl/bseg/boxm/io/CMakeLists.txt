# brl/bseg/boxm/io/CMakeLists.txt

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

IF(EXPAT_FOUND)
INCLUDE_DIRECTORIES (${EXPAT_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES (${BRL_INCLUDE_DIR}/b3p/expatpp )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

SET(boxm_io_sources
   boxm_io_scene_base.cxx   boxm_io_scene_base.h
)

AUX_SOURCE_DIRECTORY(Templates boxm_io_sources)

ADD_LIBRARY(boxm_io ${boxm_io_sources})

TARGET_LINK_LIBRARIES(boxm_io boxm ${EXPAT_LIBRARIES} expatpp)
IF ( APPLE )
  TARGET_LINK_LIBRARIES(boxm_io expat)
ENDIF ( APPLE )
ENDIF(EXPAT_FOUND)

