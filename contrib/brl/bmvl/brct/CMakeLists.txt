# segmentation detection library
# brl/bmvl/brct/CMakeLists.txt

INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
SET(brct_sources

)
AUX_SOURCE_DIRECTORY(Templates brct_sources)

ADD_LIBRARY(brct ${brct_sources})

TARGET_LINK_LIBRARIES(brct vdgl vtol vsol vil vnl vbl)

