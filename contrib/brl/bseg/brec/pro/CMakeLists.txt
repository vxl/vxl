# This is contrib/brl/bseg/brec/pro/CMakeLists.txt
# Brown Recognition Process Library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/dbil/algo )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvxm )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/brec )

SET(brec_pro_sources
    brec_register.h                       brec_register.cxx
    brec_processes.h
)

AUX_SOURCE_DIRECTORY(Templates brec_pro_sources)

AUX_SOURCE_DIRECTORY(processes brec_pro_sources)

ADD_LIBRARY(brec_pro ${brec_pro_sources})

TARGET_LINK_LIBRARIES(brec_pro brec bvxm bvxm_io bbgm_pro bprb bil_algo sdet vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vsol vil vil_io vil_pro vnl vnl_io bgeo vgl vgl_algo brip brdb)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

