INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )

ADD_EXECUTABLE( dense_matcher_full dense_matcher_full )
ADD_EXECUTABLE( make_3d_output make_3d_output )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/algo/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )

LINK_LIBRARIES( vsrl osl )

LINK_DIRECTORIES(
 ${allvxl_BINARY_DIR}/gel/vsrl
 ${allvxl_BINARY_DIR}/oxl/osl
)

