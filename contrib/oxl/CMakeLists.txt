#
# ./oxl/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/oxl)

SUBDIRS(mvl oxp vrml osl)


INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindOpenGL.cmake)


IF(OPENGL_LIBRARY)

  SUBDIRS(vgui)

ENDIF(OPENGL_LIBRARY)

