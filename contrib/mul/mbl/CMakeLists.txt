# mul/mbl/CMakeLists.txt

doxygen_add_library(contrib/mul/mbl
  DEPENDS core/vnl core/vgl core/vsl
  PACKAGE contrib/mul
  DESCRIPTION "Manchester's Basics Library"
  )

set( VXL_MBL_TEST_SAVE_MEASUREMENT_ROOT  CACHE PATH "Where to store historical measurements")
mark_as_advanced(VXL_MBL_TEST_SAVE_MEASUREMENT_ROOT)

# Set up mbl_config.h for VXL_MBL_TEST_SAVE_MEASUREMENT_ROOT, etc.
vxl_configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mbl_config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/mbl_config.h
  include/vxl/contrib/mul/mbl
)

set(mbl_sources
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
                                        mbl_parse_sequence.h
                                        mbl_parse_tuple.h
                                        mbl_parse_int_list.h
  mbl_parse_colon_pairs_list.cxx        mbl_parse_colon_pairs_list.h
  mbl_parse_string_list.cxx             mbl_parse_string_list.h
  mbl_parse_keyword_list.cxx            mbl_parse_keyword_list.h
  mbl_read_props.cxx                    mbl_read_props.h
  mbl_read_multi_props.cxx              mbl_read_multi_props.h
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
  mbl_wt_histogram.cxx                  mbl_wt_histogram.h
  mbl_clamped_plate_spline_2d.cxx       mbl_clamped_plate_spline_2d.h
  mbl_correspond_points.cxx             mbl_correspond_points.h

  mbl_ar_process.hxx                    mbl_ar_process.h

  mbl_selected_data_wrapper.hxx         mbl_selected_data_wrapper.h
  mbl_data_array_ptr_wrapper.hxx        mbl_data_array_ptr_wrapper.h

  mbl_data_array_wrapper.hxx            mbl_data_array_wrapper.h
  mbl_data_collector.hxx                mbl_data_collector.h
  mbl_data_collector_list.hxx           mbl_data_collector_list.h
  mbl_data_wrapper.hxx                  mbl_data_wrapper.h
  mbl_stochastic_data_collector.hxx     mbl_stochastic_data_collector.h
  mbl_data_wrapper_mixer.hxx            mbl_data_wrapper_mixer.h
  mbl_file_data_collector.hxx           mbl_file_data_collector.h
  mbl_file_data_wrapper.hxx             mbl_file_data_wrapper.h

  mbl_clusters.hxx                      mbl_clusters.h
  mbl_cluster_tree.hxx                  mbl_cluster_tree.h

  mbl_table.cxx                         mbl_table.h
  mbl_progress.cxx                      mbl_progress.h
  mbl_progress_text.cxx                 mbl_progress_text.h
  mbl_progress_time.cxx                 mbl_progress_time.h
  mbl_progress_null.cxx                 mbl_progress_null.h
  mbl_progress_composite.cxx            mbl_progress_composite.h
  mbl_progress_hierarchy.cxx            mbl_progress_hierarchy.h
                                        mbl_progress_callback.h
  mbl_exception.cxx                     mbl_exception.h
  mbl_rvm_regression_builder.cxx        mbl_rvm_regression_builder.h
  mbl_test.cxx                          mbl_test.h
  mbl_log.cxx                           mbl_log.h

  mbl_chord.h
  mbl_chord_3d.h
  mbl_cloneable_ptr.h
  mbl_cloneables_factory_def.h
  mbl_cloneables_factory.h
  mbl_combination.h
  mbl_draw_line.h
  mbl_print.h
  mbl_priority_bounded_queue.h
  mbl_index_sort.h
  mbl_lru_cache.h
  mbl_stl.h
  mbl_jarque_bera.h
  mbl_stl_pred.h
  mbl_vector_distance.h

  mbl_mod_gram_schmidt.cxx             mbl_mod_gram_schmidt.h
  mbl_linear_interpolator.cxx          mbl_linear_interpolator.h
  mbl_minimum_spanning_tree.cxx        mbl_minimum_spanning_tree.h
  mbl_stepwise_regression.cxx          mbl_stepwise_regression.h
  mbl_dyn_prog.cxx                     mbl_dyn_prog.h

  mbl_save_text_file.hxx               mbl_save_text_file.h
  mbl_load_text_file.hxx               mbl_load_text_file.h
  mbl_sample_stats_1d.cxx              mbl_sample_stats_1d.h

  mbl_mask.cxx                         mbl_mask.h

  mbl_eps_writer.cxx                   mbl_eps_writer.h
)

aux_source_directory(Templates mbl_sources)

if(CMAKE_COMPILER_IS_GNUCXX)
  set_source_files_properties(Templates/mbl_data_wrapper_mixer+vnl_vector+double--.cxx PROPERTIES COMPILE_FLAGS -O0)
endif()

vxl_add_library(LIBRARY_NAME mbl LIBRARY_SOURCES ${mbl_sources})
target_link_libraries(mbl ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

if(VXL_BUILD_MUL_TOOLS)
  add_subdirectory(tools)
  add_subdirectory(examples)
endif()

if (VXL_BUILD_OBJECT_LIBRARIES)
  add_library(mbl-obj OBJECT ${mbl_sources})
endif()
