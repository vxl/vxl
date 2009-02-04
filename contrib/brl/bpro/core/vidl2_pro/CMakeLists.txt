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
    vidl2_pro_utils.h              vidl2_pro_utils.cxx
    vidl2_register.h               vidl2_register.cxx
    vidl2_processes.h
)


SET (HAS_DIRECTSHOW 0 )
IF(DIRECTSHOW_FOUND)
  SET (HAS_DIRECTSHOW 1 )
ENDIF(DIRECTSHOW_FOUND)

AUX_SOURCE_DIRECTORY(Templates vidl2_pro_sources)
AUX_SOURCE_DIRECTORY(processes vidl2_pro_sources)

ADD_LIBRARY(vidl2_pro ${vidl2_pro_sources})
TARGET_LINK_LIBRARIES(vidl2_pro bprb brdb vidl2 vsl)
