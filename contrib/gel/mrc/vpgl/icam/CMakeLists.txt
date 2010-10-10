# vpgl/icam/CMakeLists.txt
# image-based camera registration library
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl)

SET(icam_sources
    icam_cost_func.cxx         icam_cost_func.h  
    icam_minimizer.cxx         icam_minimizer.h 
    icam_utils.h               icam_utils.cxx
    icam_depth_transform.h     icam_depth_transform.cxx
    icam_depth_trans_pyramid.h icam_depth_trans_pyramid.cxx
    icam_sample.h              icam_sample.cxx
    icam_view_metadata.h       icam_view_metadata.cxx
    icam_view_sphere.h         icam_view_sphere.cxx
    icam_transform_2d.h        icam_transform_2d.cxx
    icam_image.h
)


AUX_SOURCE_DIRECTORY(Templates icam_sources)

ADD_LIBRARY(icam  ${icam_sources})

TARGET_LINK_LIBRARIES(icam vsph vcl vnl vil vbl vgl_io vgl_algo vil_algo vsl vul vpgl vpgl_algo vsl vbl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
