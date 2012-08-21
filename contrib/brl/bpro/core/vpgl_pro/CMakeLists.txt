# brl/bpro/core/vpgl_pro/CMakeLists.txt
# Brown vpgl processes

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )
IF(EXPAT_FOUND)
 IF(EXPATPP_FOUND)
  INCLUDE_DIRECTORIES (${EXPAT_INCLUDE_DIRS} )
  INCLUDE_DIRECTORIES (${EXPATPP_INCLUDE_DIRS} )
 ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)

SET( vpgl_pro_sources
  vpgl_processes.h
  vpgl_register.h      vpgl_register.cxx
)
AUX_SOURCE_DIRECTORY(Templates vpgl_pro_sources)
AUX_SOURCE_DIRECTORY(processes vpgl_pro_sources)

ADD_LIBRARY(vpgl_pro ${vpgl_pro_sources})
TARGET_LINK_LIBRARIES(vpgl_pro bprb vsph bpgl bpgl_algo vpgl_io vpgl vpgl_algo vnl vil vgl vbl_io vul bkml bbas_pro)

INSTALL_TARGETS(/lib vpgl_pro)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bpro/core/vpgl_pro ${vpgl_pro_sources})

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
