ADD_EXECUTABLE( example1 example1 )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vcsl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
