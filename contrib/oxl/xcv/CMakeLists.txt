# oxl/xcv/CMakeLists.txt

INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindOpenGL.cmake)

# Build here only if OpenGL (vgui) is available:
IF (HAS_OPENGL)

INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_PATH})

ADD_EXECUTABLE(xcv xcv_sources)

# This is needed for xcv_geometry.cxx
IF (CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-50")
ENDIF (CMAKE_COMPILER_IS_GNUCXX)

SOURCE_FILES(xcv_sources
 xcv_image_tableau.cxx      xcv_image_tableau.h  xcv_image_tableau_sptr.h
 xcv_file.cxx               xcv_file.h
 xcv_display.cxx            xcv_display.h
 xcv_processing.cxx         xcv_processing.h
 xcv_geometry.cxx           xcv_geometry.h
 xcv_segmentation.cxx       xcv_segmentation.h
 xcv_multiview.cxx          xcv_multiview.h
                            xcv_mview_manager.h
 xcv_twoview_manager.cxx    xcv_twoview_manager.h
 xcv_threeview_manager.cxx  xcv_threeview_manager.h
 xcv_tjunction.cxx          xcv_tjunction.h

 xcv.cxx

 xcv_multiview_create_multiview_menu.cxx
 xcv_picker_tableau.cxx     xcv_picker_tableau.h xcv_picker_tableau_sptr.h
 xcv_axes_tableau.cxx       xcv_axes_tableau.h   xcv_axes_tableau_sptr.h
 xcv_axes_limits.cxx        xcv_axes_limits.h
)

AUX_SOURCE_DIRECTORY(Templates xcv_sources)

INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindGLUT.cmake)

# Immediate dependencies
INCLUDE(${allvxl_SOURCE_DIR}/oxl/osl/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/oxl/mvl/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/oxl/vgui/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/vxl/vgl/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/vxl/vnl/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/vxl/vnl/algo/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/vxl/vpl/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt)

ENDIF (HAS_OPENGL)
