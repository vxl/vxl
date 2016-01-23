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

add_library(icam  ${icam_sources})
target_link_libraries(icam vsph vpgl_io vpgl_algo vpgl vgl_algo vil_algo vnl_algo vgl_io vgl vnl vil vul vsl vbl_io vbl vcl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

