# oxl/xcv/CMakeLists.txt

ADD_EXECUTABLE( xcv xcv_sources )

# This is needed for xcv_geomery.cxx
IF(CMAKE_COMPILER_IS_GNUCXX)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-50")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

SOURCE_FILES( xcv_sources
  xcv_image_tableau
  xcv_file
  xcv_display
  xcv_processing
  xcv_geometry
  xcv_segmentation
  xcv_multiview
  xcv_twoview_manager
  xcv_threeview_manager
  xcv_tjunction

  xcv

  xcv_multiview_create_multiview_menu
  xcv_picker_tableau
  xcv_axes_tableau
  xcv_axes_limits
)

INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )

AUX_SOURCE_DIRECTORY( Templates xcv_sources )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vcgl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/oxl/osl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/oxl/mvl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/oxl/vgui/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vgl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/algo/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vpl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
