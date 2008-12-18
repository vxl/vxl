# brl/bpro/core/vidl2_pro/CMakeLists.txt


INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )

INCLUDE( ${MODULE_PATH}/NewCMake/FindDirectShow.cmake )


IF(DIRECTSHOW_FOUND)
  INCLUDE_DIRECTORIES(${DIRECTSHOW_INCLUDE_DIRS})
ENDIF(DIRECTSHOW_FOUND)

SET(vidl2_pro_sources
    vidl2_io_istream.h             vidl2_io_istream.cxx
    vidl2_io_ostream.h             vidl2_io_ostream.cxx
    vidl2_open_istream_process.h   vidl2_open_istream_process.cxx
    vidl2_open_ostream_process.h   vidl2_open_ostream_process.cxx
    vidl2_pro_utils.h              vidl2_pro_utils.cxx
    vidl2_get_frame_istream_process.h   vidl2_get_frame_istream_process.cxx
    vidl2_close_istream_process.h   vidl2_close_istream_process.cxx
)

IF(DIRECTSHOW_FOUND)
  SET(vidl2_pro_sources ${vidl2_pro_sources}
      vidl2_open_dshow_istream_process.h         vidl2_open_dshow_istream_process.cxx
  )
ENDIF(DIRECTSHOW_FOUND)

AUX_SOURCE_DIRECTORY(Templates vidl2_pro_sources)

ADD_LIBRARY(vidl2_pro ${vidl2_pro_sources})
TARGET_LINK_LIBRARIES(vidl2_pro bprb brdb vidl2 vsl)
