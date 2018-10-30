# vxl/vbl/CMakeLists.txt

doxygen_add_library(core/vbl
  DEPENDS core/vsl
  PACKAGE core
  DESCRIPTION "Basic Template Library"
  )

set( vbl_sources
  vbl_fwd.h

  vbl_bool_ostream.cxx        vbl_bool_ostream.h
  vbl_smart_ptr.hxx           vbl_smart_ptr.h
                              vbl_shared_pointer.h
                              vbl_scoped_ptr.h
                              vbl_checked_delete.h
  vbl_sort.hxx                vbl_sort.h           vbl_sort.cxx
  vbl_qsort.cxx               vbl_qsort.h

  vbl_array_1d.hxx            vbl_array_1d.h
  vbl_array_2d.hxx            vbl_array_2d.h
  vbl_array_3d.hxx            vbl_array_3d.h
  vbl_bit_array_2d.cxx        vbl_bit_array_2d.h
  vbl_bit_array_3d.cxx        vbl_bit_array_3d.h
  vbl_sparse_array_base.hxx   vbl_sparse_array_base.h
  vbl_sparse_array_1d.hxx     vbl_sparse_array_1d.h
  vbl_sparse_array_2d.hxx     vbl_sparse_array_2d.h
  vbl_sparse_array_3d.hxx     vbl_sparse_array_3d.h
  vbl_big_sparse_array_3d.hxx vbl_big_sparse_array_3d.h
  vbl_batch_multimap.h
  vbl_batch_compact_multimap.h

                              vbl_ref_count.h
  vbl_bounding_box.hxx        vbl_bounding_box.h

  vbl_triple.hxx              vbl_triple.h
  vbl_quadruple.hxx           vbl_quadruple.h
  vbl_local_minima.hxx        vbl_local_minima.h
  vbl_attributes.hxx          vbl_attributes.h
  vbl_disjoint_sets.cxx       vbl_disjoint_sets.h
  vbl_graph_partition.cxx     vbl_graph_partition.h
  vbl_edge.h                  vbl_edge.cxx
)

aux_source_directory(Templates vbl_sources)

vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vbl LIBRARY_SOURCES ${vbl_sources})
target_link_libraries( ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vcl )

if( VXL_BUILD_EXAMPLES )
  add_subdirectory(examples)
endif()

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

if (VXL_BUILD_OBJECT_LIBRARIES)
  add_library(vbl-obj OBJECT ${vbl_sources})
endif()
