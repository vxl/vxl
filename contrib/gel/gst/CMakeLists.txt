# gel/gst/CMakeLists.txt

SET(gst_sources
 gst_edge_2d.cxx              gst_edge_2d.h              gst_edge_2d_sptr.h
 gst_face_2d.cxx              gst_face_2d.h
 gst_polygon_2d.cxx           gst_polygon_2d.h           gst_polygon_2d_sptr.h
 gst_polygon_2d_operators.cxx gst_polygon_2d_operators.h
 gst_vertex_2d.cxx            gst_vertex_2d.h            gst_vertex_2d_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates gst_sources)

ADD_LIBRARY(gst ${gst_sources})
INSTALL_TARGETS(/lib gst)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/gel/gst ${gst_sources})

TARGET_LINK_LIBRARIES(gst vbl)
