# ./oxl/apps/vxl_vrml_viewer/CMakeLists.txt

INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindGLUT.cmake)
INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindOpenGL.cmake)
INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindMPEG.cmake)

IF(HAS_OPENGL)
 IF (HAS_GLUT)
  ADD_EXECUTABLE( glmovie glmovie.cxx make_index.cxx )
 ENDIF (HAS_GLUT)
 ADD_EXECUTABLE( vobgetframe vobgetframe.cxx )
ENDIF(HAS_OPENGL)

IF(WIN32)
 LINK_LIBRARIES( vfw32 )
ENDIF(WIN32)

# Immediate dependencies:
INCLUDE( ${allvxl_SOURCE_DIR}/oxl/oxp/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vidl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vpl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
