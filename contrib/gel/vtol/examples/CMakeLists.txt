INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )

ADD_EXECUTABLE( example2d example2d )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/gel/vtol/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/gel/vsol/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vgl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
