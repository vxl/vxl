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

vxl_add_library(LIBRARY_NAME gst LIBRARY_SOURCES ${gst_sources})
target_link_libraries(gst ${VXL_LIB_PREFIX}vbl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
