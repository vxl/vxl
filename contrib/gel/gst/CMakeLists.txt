#
# ./gel/gst/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

ADD_LIBRARY(gst gst_sources)

SOURCE_FILES(gst_sources
gst_edge_2d     gst_polygon_2d_operators
gst_face_2d     gst_vertex_2d
gst_polygon_2d
)

