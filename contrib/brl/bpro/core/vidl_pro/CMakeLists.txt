# brl/bpro/core/vidl_pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )

INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindDirectShow.cmake )

SET (HAS_DIRECTSHOW 0)
IF(DIRECTSHOW_FOUND)
  SET (HAS_DIRECTSHOW 1)
  INCLUDE_DIRECTORIES(${DIRECTSHOW_INCLUDE_DIRS})
ENDIF(DIRECTSHOW_FOUND)

SET(vidl_pro_sources
    vidl_io_istream.h             vidl_io_istream.cxx
    vidl_io_ostream.h             vidl_io_ostream.cxx
    vidl_pro_utils.h              vidl_pro_utils.cxx
    vidl_register.h               vidl_register.cxx
    vidl_processes.h
   )
AUX_SOURCE_DIRECTORY(Templates vidl_pro_sources)
AUX_SOURCE_DIRECTORY(processes vidl_pro_sources)

ADD_LIBRARY(vidl_pro ${vidl_pro_sources})
TARGET_LINK_LIBRARIES(vidl_pro bprb brdb vidl vil_io vil vsl vul)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
