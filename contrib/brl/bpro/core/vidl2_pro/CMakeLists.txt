# brl/bpro/core/vidl2_pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )

SET(vidl2_bpro_sources
    vidl2_io_istream.h vidl2_io_istream.cxx
    vidl2_io_ostream.h vidl2_io_ostream.cxx
    vidl2_open_istream_process.cxx vidl2_open_istream_process.h 
    vidl2_open_ostream_process.cxx vidl2_open_ostream_process.h 
)

AUX_SOURCE_DIRECTORY(Templates vidl2_bpro_sources)

ADD_LIBRARY(vidl2_bpro ${vidl2_bpro_sources})
TARGET_LINK_LIBRARIES(vidl2_bpro bprb brdb vidl2 vsl)


