#
# ./oxl/apps/vxl_vrml_viewer/CMakeLists.txt
#

IF (HAS_GLUT)
 ADD_EXECUTABLE( glmovie glmovie.cxx )
ENDIF (HAS_GLUT)

ADD_EXECUTABLE( vobgetframe vobgetframe.cxx )

INCLUDE( ${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindOpenGL.cmake )
INCLUDE( ${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindGLUT.cmake )
INCLUDE( ${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindMPEG.cmake )

INCLUDE( ${allvxl_SOURCE_DIR}/oxl/oxp/CMakeListsLink.txt )

INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vidl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vpl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )

IF(WIN32)
 LINK_LIBRARIES( vfw32 )
ENDIF(WIN32)
