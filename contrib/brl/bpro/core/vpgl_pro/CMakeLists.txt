# brl/bpro/core/vpgl_pro/CMakeLists.txt
# Brown vpgl processes

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )

SET( vpgl_pro_sources
  vpgl_processes.h
  vpgl_register.h      vpgl_register.cxx
)

AUX_SOURCE_DIRECTORY(Templates vpgl_pro_sources)

AUX_SOURCE_DIRECTORY(processes vpgl_pro_sources)

ADD_LIBRARY(vpgl_pro ${vpgl_pro_sources})

TARGET_LINK_LIBRARIES(vpgl_pro bprb bgeo vpgl_io vpgl vpgl_algo vnl vil vgl vbl_io vul)
