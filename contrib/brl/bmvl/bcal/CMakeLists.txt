# multi-view calibration library
# brl/bmvl/bcal/CMakeLists.txt

INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )

SET(bcal_sources

)
AUX_SOURCE_DIRECTORY(Templates bcal_sources)

ADD_LIBRARY(bcal ${bcal_sources})

TARGET_LINK_LIBRARIES(bcal vsol vnl vbl)

