# This is contrib/brl/bseg/vcon/pro/CMakeLists.txt
# vcon Process Library
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
include_directories( ${MUL_INCLUDE_DIR} )

set(vcon_pro_sources
    vcon_processes.h
    vcon_register.h  vcon_register.cxx
)

aux_source_directory(processes vcon_pro_sources)


vxl_add_library(LIBRARY_NAME vcon_pro LIBRARY_SOURCES ${vcon_pro_sources})
target_link_libraries( vcon_pro brip  ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil bvgl bvgl_algo vpgl_pro bbas_pro volm)
# Need vpgl_pro only for the instantiation of brdb_value_t<vbl_smart_ptr<vpgl_camera<double> > >
