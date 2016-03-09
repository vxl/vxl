# contrib/brl/bseg/boxm2/util/CMakeLists.txt
include_directories( ${GEL_INCLUDE_DIR} )
include_directories( ${GEL_INCLUDE_DIR}/mrc )

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )

set(boxm2_util_sources
    boxm2_cams_and_box_to_scene.h   boxm2_cams_and_box_to_scene.cxx
    boxm2_convert_bundle.h          boxm2_convert_bundle.cxx
    boxm2_convert_nvm.h             boxm2_convert_nvm.cxx
    boxm2_point_util.h              boxm2_point_util.cxx
    boxm2_detect_change_blobs.h     boxm2_detect_change_blobs.cxx
    boxm2_convert_nvm_txt.h         boxm2_convert_nvm_txt.cxx
   )

#no templates yet
#aux_source_directory(Templates boxm2_io_sources)

vxl_add_library(LIBRARY_NAME boxm2_util LIBRARY_SOURCES  ${boxm2_util_sources})
target_link_libraries(boxm2_util boxm2 bwm_video bil_algo expatpp ${VXL_LIB_PREFIX}vpgl baio ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vidl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl)

#install the .h .hxx and libs

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
