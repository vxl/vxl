# gel/vsrl/CMakeLists.txt

SET(vsrl_sources
 vsrl_3d_output.cxx             vsrl_3d_output.h
 vsrl_dense_matcher.cxx         vsrl_dense_matcher.h
 vsrl_diffusion.cxx             vsrl_diffusion.h
 vsrl_dynamic_program.cxx       vsrl_dynamic_program.h
 vsrl_image_correlation.cxx     vsrl_image_correlation.h
 vsrl_intensity_token.cxx       vsrl_intensity_token.h
 vsrl_null_token.cxx            vsrl_null_token.h
 vsrl_parameters.cxx            vsrl_parameters.h
 vsrl_raster_dp_setup.cxx       vsrl_raster_dp_setup.h
 vsrl_results_dense_matcher.cxx vsrl_results_dense_matcher.h
 vsrl_saliency_diffusion.cxx    vsrl_saliency_diffusion.h
 vsrl_step_diffusion.cxx        vsrl_step_diffusion.h
 vsrl_stereo_dense_matcher.cxx  vsrl_stereo_dense_matcher.h
 vsrl_token.cxx                 vsrl_token.h
 vsrl_token_saliency.cxx        vsrl_token_saliency.h
 vsrl_window_accumulator.cxx    vsrl_window_accumulator.h
 vsrl_region_disparity.cxx      vsrl_region_disparity.h
)

AUX_SOURCE_DIRECTORY(Templates vsrl_sources)

# vsrl_manager depends on vgui, on tbl/vepl and on brl/bseg/sdet
INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )

IF (HAS_VGUI)
 SET(vsrl_sources ${vsrl_sources}
  vsrl_point_picker.cxx         vsrl_point_picker.h    vsrl_point_picker_sptr.h
  vsrl_menus.h vsrl_menus.cxx
 )
 IF( EXISTS ${vxl_SOURCE_DIR}/contrib/tbl/vepl/CMakeLists.txt )
  INCLUDE_DIRECTORIES(${TBL_INCLUDE_DIR})
  IF( BUILD_BRL )
   INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg)
   SET(vsrl_sources ${vsrl_sources}
    vsrl_manager.cxx              vsrl_manager.h
   )
  ENDIF( BUILD_BRL )
 ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/tbl/vepl/CMakeLists.txt )
ENDIF (HAS_VGUI)

ADD_LIBRARY(vsrl ${vsrl_sources})

IF (HAS_VGUI)
 IF( EXISTS ${vxl_SOURCE_DIR}/contrib/brl/bseg/sdet/CMakeLists.txt )
  TARGET_LINK_LIBRARIES(vsrl sdet)
 ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/brl/bseg/sdet/CMakeLists.txt )
 TARGET_LINK_LIBRARIES(vsrl vgui)
 IF( EXISTS ${vxl_SOURCE_DIR}/contrib/tbl/vepl/CMakeLists.txt )
  TARGET_LINK_LIBRARIES(vsrl vepl)
 ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/tbl/vepl/CMakeLists.txt )
ENDIF (HAS_VGUI)

TARGET_LINK_LIBRARIES(vsrl vtol vdgl vgl vil1 vnl mvl vgel rsdl)

INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${RPL_INCLUDE_DIR})

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)
