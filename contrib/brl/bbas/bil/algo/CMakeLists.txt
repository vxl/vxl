# This is contrib/brl/bbas/bil/algo/CMakeLists.txt
# The Development Brown Image Algorithms Library

# Include for brip
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )

#SET(CMAKE_CXX_FLAGS -Wall)

SET( bil_algo_sources
     bil_harr_wavelet_transform.txx     bil_harr_wavelet_transform.h
     bil_wshed2d.cxx                    bil_wshed2d.h
     bil_edt.cxx                        bil_edt.h
     bil_cedt.cxx                       bil_cedt.h
     bil_finite_differences.txx         bil_finite_differences.h
     bil_finite_second_differences.txx  bil_finite_second_differences.h
     bil_roi_mask.h
     bil_perform_tensor_decomposition.txx bil_perform_tensor_decomposition.h
     bil_detect_ridges.cxx              bil_detect_ridges.h bil_detect_ridges.txx
     bil_equalize.h
     bil_edge_indicator.h
     bil_color_conversions.cxx          bil_color_conversions.h
     bil_scale_image.txx                bil_scale_image.h
     bil_compass_edge_detector.cxx      bil_compass_edge_detector.h
     bil_nms.cxx                        bil_nms.h
     bil_blob_finder.cxx                bil_blob_finder.h
     bil_trace_4con_boundary.cxx        bil_trace_4con_boundary.h
     bil_trace_8con_boundary.cxx        bil_trace_8con_boundary.h
	 bil_debayer_image.cxx				bil_debayer_image.h
   )

AUX_SOURCE_DIRECTORY(Templates bil_algo_sources)

ADD_LIBRARY( bil_algo ${bil_algo_sources} )

TARGET_LINK_LIBRARIES( bil_algo vil_algo vil vnl_algo vnl vgl_algo vgl vbl vsol)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

INSTALL_TARGETS(/lib bil_algo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bil/algo ${bil_algo_sources})
