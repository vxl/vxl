ADD_EXECUTABLE( mvl_multi_view_matches_example
  mvl_multi_view_matches_example.cxx
)

IF(HAS_SIX_POINT_EXAMPLE)
ADD_EXECUTABLE( six_point_structure six_point_structure.cxx )
ENDIF(HAS_SIX_POINT_EXAMPLE)

INCLUDE( ${allvxl_SOURCE_DIR}/oxl/mvl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/algo/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vgl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
