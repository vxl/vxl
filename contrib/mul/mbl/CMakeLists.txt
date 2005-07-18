# mul/mbl/CMakeLists.txt

SET( VXL_MBL_TEST_SAVE_MEASUREMENT_ROOT  CACHE PATH "Where to store historical measurements")
MARK_AS_ADVANCED(VXL_MBL_TEST_SAVE_MEASUREMENT_ROOT)

# Set up mbl_config.h for VXL_MBL_TEST_SAVE_MEASUREMENT_ROOT, etc.
CONFIGURE_FILE(
  ${vxl_SOURCE_DIR}/contrib/mul/mbl/mbl_config.h.in
  ${vxl_BINARY_DIR}/contrib/mul/mbl/mbl_config.h ESCAPE_QUOTES
)
INCLUDE_DIRECTORIES( ${vxl_BINARY_DIR}/contrib/mul )


SET(mbl_sources
  mbl_add_all_loaders.cxx               mbl_add_all_loaders.h
  mbl_gamma.cxx                         mbl_gamma.h
  mbl_matrix_products.cxx               mbl_matrix_products.h
  mbl_matxvec.cxx                       mbl_matxvec.h
  mbl_random_n_from_m.cxx               mbl_random_n_from_m.h
  mbl_select_n_from_m.cxx               mbl_select_n_from_m.h
  mbl_k_means.cxx                       mbl_k_means.h
  mbl_data_collector_base.cxx           mbl_data_collector_base.h
  mbl_stats_1d.cxx                      mbl_stats_1d.h
  mbl_stats_nd.cxx                      mbl_stats_nd.h
  mbl_sum_1d.cxx                        mbl_sum_1d.h
  mbl_parse_block.cxx                   mbl_parse_block.h
  mbl_read_props.cxx                    mbl_read_props.h
  mbl_lda.cxx                           mbl_lda.h
  mbl_screen_counter.cxx                mbl_screen_counter.h
  mbl_rbf_network.cxx                   mbl_rbf_network.h
  mbl_read_int.cxx                      mbl_read_int.h
  mbl_read_double.cxx                   mbl_read_double.h
  mbl_read_str.cxx                      mbl_read_str.h
  mbl_read_yes.cxx                      mbl_read_yes.h
  mbl_thin_plate_spline_2d.cxx          mbl_thin_plate_spline_2d.h
  mbl_thin_plate_spline_3d.cxx          mbl_thin_plate_spline_3d.h
  mbl_thin_plate_spline_weights_3d.cxx  mbl_thin_plate_spline_weights_3d.h
  mbl_histogram.cxx                     mbl_histogram.h
  mbl_clamped_plate_spline_2d.cxx       mbl_clamped_plate_spline_2d.h
  mbl_correspond_points.cxx             mbl_correspond_points.h

  mbl_ar_process.txx                    mbl_ar_process.h

  mbl_selected_data_wrapper.txx         mbl_selected_data_wrapper.h
  mbl_data_array_ptr_wrapper.txx        mbl_data_array_ptr_wrapper.h

  mbl_data_array_wrapper.txx            mbl_data_array_wrapper.h
  mbl_data_collector.txx                mbl_data_collector.h
  mbl_data_collector_list.txx           mbl_data_collector_list.h
  mbl_data_wrapper.txx                  mbl_data_wrapper.h
  mbl_stochastic_data_collector.txx     mbl_stochastic_data_collector.h
  mbl_data_wrapper_mixer.txx            mbl_data_wrapper_mixer.h
  mbl_file_data_collector.txx           mbl_file_data_collector.h
  mbl_file_data_wrapper.txx             mbl_file_data_wrapper.h

  mbl_table.cxx                         mbl_table.h
  mbl_progress.cxx                      mbl_progress.h
  mbl_progress_text.cxx                 mbl_progress_text.h
  mbl_progress_null.cxx                 mbl_progress_null.h
  mbl_progress_composite.cxx            mbl_progress_composite.h
  mbl_exception.h                       mbl_exception.cxx
  mbl_rvm_regression_builder.cxx        mbl_rvm_regression_builder.h
  mbl_test.h							mbl_test.cxx
  
  mbl_chord.h
  mbl_chord_3d.h
  mbl_cloneable_ptr.h
  mbl_cloneables_factory.h
  mbl_combination.h
  mbl_print.h
  mbl_priority_bounded_queue.h
  mbl_index_sort.h
  mbl_lru_cache.h
  mbl_stl.h
  mbl_jarque_bera.h
  mbl_stl_pred.h
  
  mbl_config.h.in
)
AUX_SOURCE_DIRECTORY(Templates mbl_sources)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(Templates/mbl_data_wrapper_mixer+vnl_vector+double--.cxx PROPERTIES COMPILE_FLAGS -O0)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY(mbl ${mbl_sources})
TARGET_LINK_LIBRARIES(mbl vnl_io vnl_algo vnl vgl_io vsl)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)

#SUBDIRS(tools)
