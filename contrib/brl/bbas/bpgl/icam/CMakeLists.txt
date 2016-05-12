# vpgl/icam/CMakeLists.txt
# image-based camera registration library
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bbas/bpgl )

set(icam_sources
    icam_cost_func.cxx         icam_cost_func.h
    icam_minimizer.cxx         icam_minimizer.h
    icam_minimizer_params.h
    icam_utils.h               icam_utils.cxx
    icam_depth_transform.h     icam_depth_transform.cxx
    icam_depth_trans_pyramid.h icam_depth_trans_pyramid.cxx
    icam_sample.h              icam_sample.cxx
    icam_view_metadata.h       icam_view_metadata.cxx
    icam_view_sphere.h         icam_view_sphere.cxx         icam_view_sphere_sptr.h
    icam_transform_2d.h        icam_transform_2d.cxx
    icam_image.h
    icam_cylinder_map.h        icam_cylinder_map.cxx   icam_cylinder_map_sptr.h
    icam_spherical_map.h       icam_spherical_map.cxx  icam_spherical_map_sptr.h
   )
aux_source_directory(Templates icam_sources)

vxl_add_library(LIBRARY_NAME icam LIBRARY_SOURCES  ${icam_sources})
target_link_libraries(icam vsph ${VXL_LIB_PREFIX}vpgl_io ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vcl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

