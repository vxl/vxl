# ./gel/gst/CMakeLists.txt

ADD_LIBRARY(gst gst_sources)

SOURCE_FILES(gst_sources
  gst_edge_2d.cxx
  gst_face_2d.cxx
  gst_polygon_2d.cxx
  gst_polygon_2d_operators.cxx
  gst_vertex_2d.cxx
)

AUX_SOURCE_DIRECTORY(Templates gst_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/gel/gst/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)
