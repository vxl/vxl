# Copyright 2006-2009 Brad King, Chuck Stewart
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file rgtl_license_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

SET(rgtl_sources
  rgtl_compact_tree.hxx                 rgtl_compact_tree.txx
  rgtl_compact_tree_data_fixed.hxx
  rgtl_compact_tree_index.hxx
  rgtl_compact_tree_link.hxx
  rgtl_compact_tree_node_data.hxx
  rgtl_compact_tree_policy.hxx
  rgtl_config.hxx
  rgtl_meta.hxx
  rgtl_object_array.hxx
  rgtl_object_array_points.hxx          rgtl_object_array_points.txx
  rgtl_object_array_triangles_3d.hxx    rgtl_object_array_triangles_3d.cxx
  rgtl_object_closest_polygon_3d.hxx
  rgtl_object_once.hxx                  rgtl_object_once.cxx
  rgtl_octree_cell_bounds.hxx           rgtl_octree_cell_bounds.txx
  rgtl_octree_cell_location.hxx         rgtl_octree_cell_location.txx
  rgtl_octree_data_fixed.hxx            rgtl_octree_data_fixed.txx
  rgtl_octree_dual.hxx
  rgtl_octree_dual_element.hxx
  rgtl_octree_dual_index.hxx
  rgtl_octree_dual_location.hxx
  rgtl_octree_dual_visit.hxx
  rgtl_octree_objects.hxx               rgtl_octree_objects.txx
  rgtl_octree_point_location.hxx        rgtl_octree_point_location.txx
  rgtl_separating_axis.hxx
  rgtl_separating_axis_projection.hxx
  rgtl_static_assert.hxx
  rgtl_tagged_index.hxx
  rgtl_tagged_vector.hxx

  # Serialization
# rgtl_serialize.txt
  rgtl_serialize_access.hxx
  rgtl_serialize_base.hxx
  rgtl_serialize_direction.hxx
  rgtl_serialize_istream.hxx            rgtl_serialize_istream.cxx
  rgtl_serialize_ostream.hxx            rgtl_serialize_ostream.cxx
  rgtl_serialize_split.hxx
  rgtl_serialize_stl_vector.hxx
  rgtl_serialize_vnl_double_3.hxx
  rgtl_serialize_vnl_vector_fixed.hxx

  # Spherical Quad-Tree
  rgtl_sqt_base.hxx                     rgtl_sqt_base.cxx
  rgtl_sqt_cell_bounds.hxx              rgtl_sqt_cell_bounds.txx
  rgtl_sqt_cell_geometry.hxx            rgtl_sqt_cell_geometry.txx
  rgtl_sqt_cell_location.hxx            rgtl_sqt_cell_location.txx
  rgtl_sqt_object_array.hxx
  rgtl_sqt_object_array_triangles_3d.cxx
  rgtl_sqt_object_array_triangles_3d.hxx
  rgtl_sqt_objects.hxx                  rgtl_sqt_objects.txx
  rgtl_sqt_space.hxx                    rgtl_sqt_space.txx

  # Explicit instantiations
  Templates/rgtl_object_array_points+2-.cxx
  Templates/rgtl_object_array_points+3-.cxx
  Templates/rgtl_octree_cell_bounds+1-.cxx
  Templates/rgtl_octree_cell_bounds+2-.cxx
  Templates/rgtl_octree_cell_bounds+3-.cxx
  Templates/rgtl_octree_cell_geometry+2-.cxx
  Templates/rgtl_octree_cell_geometry+3-.cxx
  Templates/rgtl_octree_cell_location+1-.cxx
  Templates/rgtl_octree_cell_location+2-.cxx
  Templates/rgtl_octree_cell_location+3-.cxx
  Templates/rgtl_octree_objects+2-.cxx
  Templates/rgtl_octree_objects+3-.cxx
  Templates/rgtl_octree_point_location+2-.cxx
  Templates/rgtl_octree_point_location+3-.cxx
  Templates/rgtl_sqt_cell_bounds+2-.cxx
  Templates/rgtl_sqt_cell_bounds+3-.cxx
  Templates/rgtl_sqt_cell_geometry+2._-.cxx
  Templates/rgtl_sqt_cell_geometry+3._-.cxx
  Templates/rgtl_sqt_cell_location+2-.cxx
  Templates/rgtl_sqt_cell_location+3-.cxx
  Templates/rgtl_sqt_objects+2-.cxx
  Templates/rgtl_sqt_objects+3-.cxx
  Templates/rgtl_sqt_space+2._-.cxx
  Templates/rgtl_sqt_space+3._-.cxx
  )

ADD_LIBRARY(rgtl ${rgtl_sources})
TARGET_LINK_LIBRARIES(rgtl vnl_algo vnl vcl)

INSTALL_TARGETS(/lib rgtl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/rpl/rgtl ${rgtl_sources})

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
