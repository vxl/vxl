# ./oxl/apps/vxl_vrml_viewer/CMakeLists.txt

INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindOpenGL.cmake)
INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindQv.cmake)

# Build here only if OpenGL is available:
IF(HAS_OPENGL)

ADD_EXECUTABLE( vxl_vrml_viewer vxl_vrml_viewer_sources )

SOURCE_FILES( vxl_vrml_viewer_sources main.cxx )

# Immediate dependencies:
INCLUDE( ${allvxl_SOURCE_DIR}/oxl/osl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/oxl/mvl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/oxl/vgui/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vgl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vpl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/oxl/vrml/CMakeListsLink.txt )

# need to list a couple of libs again on the link line
# this kludge will only be needed until cmake 1.4 link line features are used
# Cygwin requires libs listed with dependent libs to the right
LINK_LIBRARIES(png zlib tiff jpeg)

ENDIF(HAS_OPENGL)
