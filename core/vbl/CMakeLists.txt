# vxl/vbl/CMakeLists.txt

doxygen_add_library(core/vbl
  DEPENDS core/vsl
  PACKAGE core
  DESCRIPTION "Basic Template Library"
  )

set( vbl_sources
  vbl_fwd.h

  vbl_bool_ostream.cxx        vbl_bool_ostream.h
  vbl_smart_ptr.txx           vbl_smart_ptr.h
                              vbl_shared_pointer.h
                              vbl_scoped_ptr.h
                              vbl_checked_delete.h
  vbl_sort.txx                vbl_sort.h           vbl_sort.cxx
  vbl_qsort.cxx               vbl_qsort.h

  vbl_array_1d.txx            vbl_array_1d.h
  vbl_array_2d.txx            vbl_array_2d.h
  vbl_array_3d.txx            vbl_array_3d.h
  vbl_bit_array_2d.cxx        vbl_bit_array_2d.h
  vbl_bit_array_3d.cxx        vbl_bit_array_3d.h
  vbl_sparse_array_base.txx   vbl_sparse_array_base.h
  vbl_sparse_array_1d.txx     vbl_sparse_array_1d.h
  vbl_sparse_array_2d.txx     vbl_sparse_array_2d.h
  vbl_sparse_array_3d.txx     vbl_sparse_array_3d.h
  vbl_big_sparse_array_3d.txx vbl_big_sparse_array_3d.h
  vbl_batch_multimap.h
  vbl_batch_compact_multimap.h

                              vbl_ref_count.h
  vbl_bounding_box.txx        vbl_bounding_box.h

  vbl_triple.txx              vbl_triple.h
  vbl_quadruple.txx           vbl_quadruple.h
  vbl_local_minima.txx        vbl_local_minima.h
  vbl_attributes.txx          vbl_attributes.h
  vbl_disjoint_sets.cxx       vbl_disjoint_sets.h
  vbl_graph_partition.cxx     vbl_graph_partition.h
  vbl_edge.h                  vbl_edge.cxx
)

aux_source_directory(Templates vbl_sources)

add_library(vbl ${vbl_sources})
target_link_libraries( vbl vcl )

install_targets(/lib vbl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vbl ${vbl_sources})

if( BUILD_EXAMPLES )
  add_subdirectory(examples)
endif( BUILD_EXAMPLES )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )
