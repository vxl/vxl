#
# ./oxl/vgui/impl/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

IF(WIN32)
  SUBDIRS(mfc)
ELSE(WIN32)

  INCLUDE(${CMAKE_SOURCE_DIR}/CMake/Modules/FindGTK.cmake)

  IF(GTK_INCLUDE_PATH)

    SUBDIRS(gtk)

  ENDIF(GTK_INCLUDE_PATH)


ENDIF(WIN32)

