# ./oxl/CMakeLists.txt

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/oxl)

SUBDIRS(mvl oxp vrml osl)

# using OpenGL:
SUBDIRS(vgui xcv VPLAYER apps)

IF(BUILD_TESTING)
  SUBDIRS(oxltest)
ENDIF(BUILD_TESTING)
