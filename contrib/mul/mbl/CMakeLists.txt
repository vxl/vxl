# mul/mbl/CMakeLists.txt

SET(mbl_sources
 mbl_gamma.cxx                     mbl_gamma.h
 mbl_matrix_products.cxx           mbl_matrix_products.h
 mbl_matxvec.cxx                   mbl_matxvec.h
 mbl_mz_random.cxx                 mbl_mz_random.h
 mbl_k_means.cxx                   mbl_k_means.h
 mbl_data_collector_base.cxx       mbl_data_collector_base.h
 mbl_stats_1d.cxx                  mbl_stats_1d.h
 mbl_sum_1d.cxx                    mbl_sum_1d.h
 mbl_read_props.cxx                mbl_read_props.h
 mbl_lda.cxx                       mbl_lda.h
 mbl_screen_counter.cxx            mbl_screen_counter.h
 mbl_rbf_network.cxx               mbl_rbf_network.h
 mbl_read_int.cxx                  mbl_read_int.h
 mbl_read_double.cxx               mbl_read_double.h
 mbl_read_str.cxx                  mbl_read_str.h
 mbl_thin_plate_spline_2d.cxx      mbl_thin_plate_spline_2d.h
 mbl_histogram.cxx                 mbl_histogram.h
 mbl_clamped_plate_spline_2d.cxx   mbl_clamped_plate_spline_2d.h

 mbl_selected_data_wrapper.txx     mbl_selected_data_wrapper.h
 mbl_data_array_ptr_wrapper.txx    mbl_data_array_ptr_wrapper.h

 mbl_data_array_wrapper.txx        mbl_data_array_wrapper.h
 mbl_data_collector.txx            mbl_data_collector.h
 mbl_data_collector_list.txx       mbl_data_collector_list.h
 mbl_data_wrapper.txx              mbl_data_wrapper.h
 mbl_stochastic_data_collector.txx mbl_stochastic_data_collector.h
 mbl_data_wrapper_mixer.txx        mbl_data_wrapper_mixer.h

 mbl_chord.h
 mbl_cloneable_ptr.h
 mbl_print.h
 mbl_priority_bounded_queue.h
 mbl_index_sort.h
 mbl_lru_cache.h
 mbl_stl.h
)

AUX_SOURCE_DIRECTORY(Templates mbl_sources)

ADD_LIBRARY(mbl ${mbl_sources})
TARGET_LINK_LIBRARIES(mbl vnl_io vnl_algo vsl vgl_io)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
