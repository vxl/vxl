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

add_library(boxm2_util  ${boxm2_util_sources})
target_link_libraries(boxm2_util boxm2 bwm_video bil_algo expatpp vpgl baio vpgl vnl vidl vil_algo vil vgl_algo vgl_xio vgl vbl vul vsl vcl)

#install the .h .txx and libs
install_targets(/lib boxm2_util)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2_util/ ${boxm2_util_sources})

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )
