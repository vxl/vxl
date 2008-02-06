# brl/bpro/bipp/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )

SET(bipp_sources
   bipp_load_image_process.cxx     bipp_load_image_process.h
)

AUX_SOURCE_DIRECTORY(Templates bipp_sources)
ADD_LIBRARY(bipp ${bipp_sources})

TARGET_LINK_LIBRARIES(bipp vil)

SUBDIRS(io)
