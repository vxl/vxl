# Harris corner detector
ADD_EXECUTABLE( run_osl_harris run_osl_harris )

ADD_EXECUTABLE( run_osl_canny run_osl_canny )
ADD_EXECUTABLE( topology_edges topology_edges )
ADD_EXECUTABLE( break_lines break_lines )

INCLUDE( ${allvxl_SOURCE_DIR}/oxl/osl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
