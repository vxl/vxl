# vxl/vcsl/CMakeLists.txt

doxygen_add_library(core/vcsl
  DEPENDS core/vnl core/vbl
  PACKAGE core-L2
  DESCRIPTION "Spatial Coordinate Systems and Manipulation"
  )

  set( vcsl_sources
    vcsl_dimension.h vcsl_dimension_sptr.h
    vcsl_length.cxx vcsl_length.h vcsl_length_sptr.h

    vcsl_angle.cxx vcsl_angle.h vcsl_angle_sptr.h

    vcsl_unit.h vcsl_unit_sptr.h
    vcsl_length_unit.cxx vcsl_length_unit.h vcsl_length_unit_sptr.h
    vcsl_meter.cxx vcsl_meter.h vcsl_meter_sptr.h
    vcsl_millimeter.cxx vcsl_millimeter.h vcsl_millimeter_sptr.h
    vcsl_micron.cxx vcsl_micron.h vcsl_micron_sptr.h

    vcsl_angle_unit.cxx vcsl_angle_unit.h vcsl_angle_unit_sptr.h
    vcsl_radian.cxx vcsl_radian.h vcsl_radian_sptr.h
    vcsl_degree.cxx vcsl_degree.h vcsl_degree_sptr.h

    vcsl_axis.cxx vcsl_axis.h vcsl_axis_sptr.h
    vcsl_coordinate_system.cxx vcsl_coordinate_system.h vcsl_coordinate_system_sptr.h

    vcsl_cartesian.h vcsl_cartesian_sptr.h

    vcsl_spatial.cxx vcsl_spatial.h vcsl_spatial_sptr.h
    vcsl_graph.cxx vcsl_graph.h vcsl_graph_sptr.h

    vcsl_cartesian_2d.cxx vcsl_cartesian_2d.h vcsl_cartesian_2d_sptr.h
    vcsl_cartesian_3d.cxx vcsl_cartesian_3d.h vcsl_cartesian_3d_sptr.h

    vcsl_polar.cxx vcsl_polar.h vcsl_polar_sptr.h
    vcsl_cylindrical.cxx vcsl_cylindrical.h vcsl_cylindrical_sptr.h
    vcsl_spherical.cxx vcsl_spherical.h vcsl_spherical_sptr.h

    vcsl_spheroid.cxx vcsl_spheroid.h vcsl_spheroid_sptr.h
    vcsl_geographic.cxx vcsl_geographic.h vcsl_geographic_sptr.h
    vcsl_utm.cxx vcsl_utm.h vcsl_utm_sptr.h
    vcsl_geocentric.cxx vcsl_geocentric.h vcsl_geocentric_sptr.h
    vcsl_geodetic.h vcsl_geodetic_sptr.h
    vcsl_lambertian.cxx vcsl_lambertian.h vcsl_lambertian_sptr.h

    vcsl_spatial_transformation.cxx vcsl_spatial_transformation.h vcsl_spatial_transformation_sptr.h

    vcsl_composition.cxx vcsl_composition.h vcsl_composition_sptr.h
    vcsl_scale.cxx vcsl_scale.h vcsl_scale_sptr.h
    vcsl_translation.cxx vcsl_translation.h vcsl_translation_sptr.h
    vcsl_rotation.cxx vcsl_rotation.h vcsl_rotation_sptr.h
    vcsl_displacement.cxx vcsl_displacement.h vcsl_displacement_sptr.h

    vcsl_cylindrical_to_cartesian_3d.cxx vcsl_cylindrical_to_cartesian_3d.h vcsl_cylindrical_to_cartesian_3d_sptr.h

    vcsl_perspective.cxx vcsl_perspective.h vcsl_perspective_sptr.h

    vcsl_matrix.cxx vcsl_matrix.h vcsl_matrix_sptr.h
    vcsl_matrix_param.h vcsl_matrix_param_sptr.h
  )

  aux_source_directory( Templates vcsl_sources )

  vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vcsl LIBRARY_SOURCES ${vcsl_sources} )
  target_link_libraries( ${VXL_LIB_PREFIX}vcsl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl )

  if( VXL_BUILD_EXAMPLES )
    add_subdirectory(examples)
  endif()

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()
