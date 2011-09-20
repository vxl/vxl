# contrib/brl/bseg/boxm2/util/CMakeLists.txt
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )

SET(boxm2_util_sources
    boxm2_convert_bundle.h          boxm2_convert_bundle.cxx
    boxm2_convert_nvm.h             boxm2_convert_nvm.cxx
    boxm2_cams_and_box_to_scene.h   boxm2_cams_and_box_to_scene.cxx
    boxm2_point_util.h              boxm2_point_util.cxx
   )
   
#no templates yet
#AUX_SOURCE_DIRECTORY(Templates boxm2_util_sources)

ADD_LIBRARY(boxm2_util  ${boxm2_util_sources})
TARGET_LINK_LIBRARIES(boxm2_util boxm2 bwm expatpp vpgl baio vpgl vidl vgl_xio vgl vbl vul vsl vcl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
