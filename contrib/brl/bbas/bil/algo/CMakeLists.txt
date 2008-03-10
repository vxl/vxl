# This is contrib/brl/bbas/bil/algo/CMakeLists.txt
# The Development Brown Image Algorithms Library

# Include for brip
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )

SET(CMAKE_CXX_FLAGS -Wall)

SET( bil_algo_sources
   bil_scale_image.txx                bil_scale_image.h
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
)

AUX_SOURCE_DIRECTORY(Templates bil_algo_sources)

ADD_LIBRARY( bil_algo ${bil_algo_sources} )

TARGET_LINK_LIBRARIES( bil_algo vil_algo)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
