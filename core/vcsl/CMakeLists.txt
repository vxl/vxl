# ./vxl/vcsl/CMakeLists.txt

ADD_LIBRARY( vcsl vcsl_sources )

SOURCE_FILES( vcsl_sources
  vcsl_dimension.cxx
  vcsl_length.cxx
  vcsl_angle.cxx
  vcsl_unit.cxx
  vcsl_length_unit.cxx
  vcsl_meter.cxx
  vcsl_angle_unit.cxx
  vcsl_radian.cxx
  vcsl_degree.cxx
  vcsl_axis.cxx
  vcsl_coordinate_system.cxx
  vcsl_cartesian.cxx
  vcsl_spatial.cxx
  vcsl_graph.cxx
  vcsl_cartesian_2d.cxx
  vcsl_cartesian_3d.cxx
  vcsl_polar.cxx
  vcsl_cylindrical.cxx
  vcsl_spherical.cxx
  vcsl_spheroid.cxx
  vcsl_geographic.cxx
  vcsl_utm.cxx
  vcsl_geocentric.cxx
  vcsl_geodetic.cxx
  vcsl_lambertian.cxx
  vcsl_spatial_transformation.cxx
  vcsl_composition.cxx
  vcsl_scale.cxx
  vcsl_translation.cxx
  vcsl_rotation.cxx
  vcsl_displacement.cxx
  vcsl_cylindrical_to_cartesian_3d.cxx
  vcsl_perspective.cxx
  vcsl_matrix.cxx
)

AUX_SOURCE_DIRECTORY( Templates vcsl_sources )

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/vxl/vcsl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
