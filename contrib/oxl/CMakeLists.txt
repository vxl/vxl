#
# ./oxl/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/oxl)

SUBDIRS(mvl oxp vrml osl)


FIND_INCLUDE( HAS_OPENGL GL/gl.h /usr/local/include )

IF(HAS_OPENGL)
  SUBDIRS(vgui)
END(HAS_OPENGL)
