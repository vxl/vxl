ADD_LIBRARY( vcsl vcsl_sources )

SOURCE_FILES( vcsl_sources
  vcsl_dimension
  vcsl_length
  vcsl_angle
  vcsl_unit
  vcsl_length_unit
  vcsl_meter
  vcsl_angle_unit
  vcsl_radian
  vcsl_degree
  vcsl_axis
  vcsl_coordinate_system
  vcsl_cartesian
  vcsl_spatial
  vcsl_graph
  vcsl_cartesian_2d
  vcsl_cartesian_3d
  vcsl_polar
  vcsl_cylindrical
  vcsl_spherical
  vcsl_spheroid
  vcsl_geographic
  vcsl_utm
  vcsl_geocentric
  vcsl_geodetic
  vcsl_lambertian
  vcsl_spatial_transformation
  vcsl_composition
  vcsl_scale
  vcsl_translation
  vcsl_rotation
  vcsl_displacement
  vcsl_cylindrical_to_cartesian_3d
  vcsl_perspective
  vcsl_matrix
)

AUX_SOURCE_DIRECTORY( Templates vcsl_sources )

SUBDIRS( tests examples )
