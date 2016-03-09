# boxm opencl opengl display
include_directories(${GLEW_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${GEL_INCLUDE_DIR}/mrc )
set(boxm2_view_pro_sources
    boxm2_view_register.h      boxm2_view_register.cxx
    boxm2_view_processes.h
   )
aux_source_directory(processes boxm2_view_pro_sources)
vxl_add_library(LIBRARY_NAME boxm2_view_pro LIBRARY_SOURCES ${boxm2_view_pro_sources} )

target_link_libraries(boxm2_view_pro boxm2_view ${VXL_LIB_PREFIX}vgui)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

