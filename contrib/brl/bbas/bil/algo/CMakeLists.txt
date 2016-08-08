# This is contrib/brl/bbas/bil/algo/CMakeLists.txt
# The Development Brown Image Algorithms Library

# Include for brip
include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${GEL_INCLUDE_DIR} )

#set(CMAKE_CXX_FLAGS -Wall)

set( bil_algo_sources
     bil_harr_wavelet_transform.hxx     bil_harr_wavelet_transform.h
     bil_wshed2d.cxx                    bil_wshed2d.h
     bil_edt.cxx                        bil_edt.h
     bil_cedt.cxx                       bil_cedt.h
     bil_finite_differences.hxx         bil_finite_differences.h
     bil_finite_second_differences.hxx  bil_finite_second_differences.h
     bil_roi_mask.h
     bil_perform_tensor_decomposition.hxx bil_perform_tensor_decomposition.h
     bil_detect_ridges.cxx              bil_detect_ridges.h bil_detect_ridges.hxx
     bil_equalize.h
     bil_edge_indicator.h
     bil_color_conversions.cxx          bil_color_conversions.h
     bil_scale_image.hxx                bil_scale_image.h
     bil_compass_edge_detector.cxx      bil_compass_edge_detector.h
     bil_nms.cxx                        bil_nms.h
     bil_blob_finder.cxx                bil_blob_finder.h
     bil_trace_4con_boundary.cxx        bil_trace_4con_boundary.h
     bil_trace_8con_boundary.cxx        bil_trace_8con_boundary.h
     bil_debayer_image.cxx              bil_debayer_image.h
     bil_image_bounds_from_histogram.cxx bil_image_bounds_from_histogram.h
)

aux_source_directory(Templates bil_algo_sources)

vxl_add_library(LIBRARY_NAME bil_algo LIBRARY_SOURCES ${bil_algo_sources} )

target_link_libraries( bil_algo ${VXL_LIB_PREFIX}bsta ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl vsol)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
