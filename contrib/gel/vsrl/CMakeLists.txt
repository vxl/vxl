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
 vsrl_point_picker.cxx          vsrl_point_picker.h
 vsrl_point_picker_sptr.h
 vsrl_menus.h
)

AUX_SOURCE_DIRECTORY(Templates vsrl_sources)

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg)
INCLUDE_DIRECTORIES(${TBL_INCLUDE_DIR})

# vsrl_manager depends on vgui and hence on OpenGL
INCLUDE(${MODULE_PATH}/NewCMake/FindOpenGL.cmake)

IF (OPENGL_FOUND)
SET(vsrl_sources ${vsrl_sources}
 vsrl_manager.cxx               vsrl_manager.h
)
ENDIF (OPENGL_FOUND)

ADD_LIBRARY(vsrl ${vsrl_sources})

TARGET_LINK_LIBRARIES(vsrl sdet vtol vdgl vepl vgl vil vnl)
IF (OPENGL_FOUND)
 TARGET_LINK_LIBRARIES(vsrl vgui)
ENDIF (OPENGL_FOUND)

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)
