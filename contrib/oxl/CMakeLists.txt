#
# ./oxl/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/oxl)

SUBDIRS(mvl oxp vrml osl)
IF(BUILD_TESTS)
  SUBDIRS(oxltest)
ENDIF(BUILD_TESTS)

INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindOpenGL.cmake)

IF(HAS_OPENGL)
  SUBDIRS(vgui xcv VPLAYER apps)
ENDIF(HAS_OPENGL)

