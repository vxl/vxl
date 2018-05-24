# Copyright 2006-2009 Brad King, Chuck Stewart
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file rgtl_license_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

set(rgtl_sources
  rgtl_compact_tree.h                 rgtl_compact_tree.hxx
  rgtl_compact_tree_data_fixed.h
  rgtl_compact_tree_index.h
  rgtl_compact_tree_link.h
  rgtl_compact_tree_node_data.h
  rgtl_compact_tree_policy.h
  rgtl_config.h
  rgtl_meta.h
  rgtl_object_array.h
  rgtl_object_array_points.h          rgtl_object_array_points.hxx
  rgtl_object_array_triangles_3d.h    rgtl_object_array_triangles_3d.cxx
  rgtl_object_closest_polygon_3d.h
  rgtl_object_once.h                  rgtl_object_once.cxx
  rgtl_octree_cell_bounds.h           rgtl_octree_cell_bounds.hxx
  rgtl_octree_cell_geometry.h         rgtl_octree_cell_geometry.hxx
  rgtl_octree_cell_location.h         rgtl_octree_cell_location.hxx
  rgtl_octree_data_fixed.h            rgtl_octree_data_fixed.hxx
  rgtl_octree_dual.h
  rgtl_octree_dual_element.h
  rgtl_octree_dual_index.h
  rgtl_octree_dual_location.h
  rgtl_octree_dual_visit.h
  rgtl_octree_objects.h               rgtl_octree_objects.hxx
  rgtl_octree_point_location.h        rgtl_octree_point_location.hxx
  rgtl_separating_axis.h
  rgtl_separating_axis_projection.h
  rgtl_static_assert.h
  rgtl_tagged_index.h
  rgtl_tagged_vector.h

  # Serialization
# rgtl_serialize.txt
  rgtl_serialize_access.h
  rgtl_serialize_base.h
  rgtl_serialize_direction.h
  rgtl_serialize_istream.h            rgtl_serialize_istream.cxx
  rgtl_serialize_ostream.h            rgtl_serialize_ostream.cxx
  rgtl_serialize_split.h
  rgtl_serialize_stl_vector.h
  rgtl_serialize_vnl_double_3.h
  rgtl_serialize_vnl_vector_fixed.h

  # Spherical Quad-Tree
  rgtl_sqt_base.h                     rgtl_sqt_base.cxx
  rgtl_sqt_cell_bounds.h              rgtl_sqt_cell_bounds.hxx
  rgtl_sqt_cell_geometry.h            rgtl_sqt_cell_geometry.hxx
  rgtl_sqt_cell_location.h            rgtl_sqt_cell_location.hxx
  rgtl_sqt_object_array.h
  rgtl_sqt_object_array_triangles_3d.cxx
  rgtl_sqt_object_array_triangles_3d.h
  rgtl_sqt_objects.h                  rgtl_sqt_objects.hxx
  rgtl_sqt_space.h                    rgtl_sqt_space.hxx

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

vxl_add_library(LIBRARY_NAME rgtl LIBRARY_SOURCES ${rgtl_sources})
target_link_libraries(rgtl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vcl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
