# ./oxl/CMakeLists.txt

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/oxl)

OPTION(BUILD_VGUI "Should the VGUI user interface be built?" NO)

SUBDIRS(mvl oxp vrml osl)

IF(BUILD_VGUI)
  # using OpenGL:
  SUBDIRS(vgui xcv VPLAYER apps)
ENDIF(BUILD_VGUI)

IF(BUILD_TESTING)
  SUBDIRS(oxltest)
ENDIF(BUILD_TESTING)
