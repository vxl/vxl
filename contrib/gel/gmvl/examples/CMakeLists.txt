INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )
INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/gel )

ADD_EXECUTABLE( gmvl_examples gmvl_test.cxx )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/gel/vtol/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
LINK_LIBRARIES(gmvl gbl)
LINK_DIRECTORIES(
  ${allvxl_BINARY_DIR}/gel/gmvl
  ${allvxl_BINARY_DIR}/gel/gbl
)
