# basic radiometery library

  # Set up brad_where_brl_lib_dir.h to have a macro set to $CMAKE_LIBRARY_OUTPUT_DIRECTORY
  vxl_configure_file(${CMAKE_CURRENT_SOURCE_DIR}/brad_where_brl_lib_dir.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/brad_where_brl_lib_dir.h include/vxl/bbas/brad
  )

  # Copy the Eulerspiral lookup-table file to the BRL library folder
  if(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    vxl_configure_file_copyonly( ${CMAKE_CURRENT_SOURCE_DIR}/brad_sat_img_calibration_table.txt
      ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/brad_sat_img_calibration_table.txt include/vxl/bbas/brad
    )
    vxl_configure_file_copyonly( ${CMAKE_CURRENT_SOURCE_DIR}/brad_sat_img_sun_irradiance_table.txt
      ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/brad_sat_img_sun_irradiance_table.txt include/vxl/bbas/brad
    )
  endif()

  add_definitions(-DBRAD_WHERE_BRL_LIB_DIR_H_EXISTS)
  include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

  include_directories( ${BRL_INCLUDE_DIR}/bbas )
  include_directories(${GEL_INCLUDE_DIR})

  set(brad_sources
      brad_illum_util.h               brad_illum_util.cxx
      brad_illum_cost_function.h
      brad_sun_pos.h                  brad_sun_pos.cxx
      brad_sun_dir_index.h            brad_sun_dir_index.cxx
      brad_hist_prob_feature_vector.h brad_hist_prob_feature_vector.cxx
      brad_grad_hist_feature_vector.h brad_grad_hist_feature_vector.cxx
      brad_grad_int_feature_vector.h  brad_grad_int_feature_vector.cxx
      brad_eigenspace.h               brad_eigenspace.hxx
      brad_eigenspace_base.h          brad_eigenspace_base.cxx
      brad_eigenspace_sptr.h
      brad_phongs_model_est.h         brad_phongs_model_est.cxx
      brad_synoptic_function_1d.h     brad_synoptic_function_1d.cxx
      brad_image_metadata.h           brad_image_metadata.cxx
      brad_atmospheric_parameters.h   brad_atmospheric_parameters.cxx
      brad_image_atmospherics_est.h   brad_image_atmospherics_est.cxx
      brad_estimate_shadows.h         brad_estimate_shadows.cxx
      brad_appearance_neighborhood_index.h  brad_appearance_neighborhood_index.cxx
      brad_belief_prop_utils.h        brad_belief_prop_utils.cxx
      brad_sam_template_match.h       brad_sam_template_match.cxx
      brad_spectral_angle_mapper.h    brad_spectral_angle_mapper.cxx
      brad_multispectral_functions.h  brad_multispectral_functions.cxx
      brad_worldview3_functions.h     brad_worldview3_functions.cxx
)
  aux_source_directory(Templates brad_sources)
  vxl_add_library(LIBRARY_NAME brad LIBRARY_SOURCES ${brad_sources} )

  target_link_libraries(brad bsta bsta_algo ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul )

  add_subdirectory(io)

  add_subdirectory(app)

#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif()