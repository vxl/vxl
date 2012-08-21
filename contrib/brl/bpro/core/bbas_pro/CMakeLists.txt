# brl/bpro/core/bbas_pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc)

SET(bbas_pro_sources
    bbas_processes.h
    bbas_register.h             bbas_register.cxx
    bbas_1d_array_float.h       bbas_1d_array_float.cxx
    bbas_1d_array_int.h         bbas_1d_array_int.cxx
    bbas_1d_array_unsigned.h    bbas_1d_array_unsigned.cxx
    bbas_1d_array_string.h      bbas_1d_array_string.cxx
   )

AUX_SOURCE_DIRECTORY(Templates bbas_pro_sources)
AUX_SOURCE_DIRECTORY(processes bbas_pro_sources)

ADD_LIBRARY(bbas_pro ${bbas_pro_sources})
TARGET_LINK_LIBRARIES(bbas_pro bprb brdb brad brad_io bsta_io bsta vpgl vpgl_algo vil_io vil vnl_io vnl_algo vnl vgl vbl_io vbl vsl imesh depth_map)

INSTALL_TARGETS(/lib bbas_pro)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bpro/core/bbas_pro ${bbas_pro_sources})

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
