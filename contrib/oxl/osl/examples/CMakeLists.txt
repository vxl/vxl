# Harris corner detector
ADD_EXECUTABLE( run_osl_harris run_osl_harris.cxx )

ADD_EXECUTABLE( run_osl_canny run_osl_canny.cxx )
ADD_EXECUTABLE( topology_edges topology_edges.cxx )
ADD_EXECUTABLE( break_lines break_lines.cxx )
ADD_EXECUTABLE( fit_lines fit_lines.cxx )

INCLUDE( ${allvxl_SOURCE_DIR}/oxl/osl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vgl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
