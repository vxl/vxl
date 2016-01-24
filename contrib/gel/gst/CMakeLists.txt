# gel/gst/CMakeLists.txt

doxygen_add_library(contrib/gel/gst
  DEPENDS core/vbl
  PACKAGE contrib/gel
  DESCRIPTION "Simplified 2D Topology representations"
  )

set(gst_sources
 gst_edge_2d.cxx              gst_edge_2d.h              gst_edge_2d_sptr.h
 gst_face_2d.cxx              gst_face_2d.h
 gst_polygon_2d.cxx           gst_polygon_2d.h           gst_polygon_2d_sptr.h
 gst_polygon_2d_operators.cxx gst_polygon_2d_operators.h
 gst_vertex_2d.cxx            gst_vertex_2d.h            gst_vertex_2d_sptr.h
)

aux_source_directory(Templates gst_sources)

add_library(gst ${gst_sources})
install_targets(/lib gst)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/gel/gst ${gst_sources})

target_link_libraries(gst vbl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
