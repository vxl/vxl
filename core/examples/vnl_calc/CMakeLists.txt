ADD_EXECUTABLE( vnl_calc vnl_calc )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNetlib.cmake )
