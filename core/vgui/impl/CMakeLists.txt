#
# ./oxl/vgui/impl/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

IF(WIN32)
  SUBDIRS(mfc)
ELSE(WIN32)

  INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindGTK.cmake)
  INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindGLUT.cmake)

  IF(HAS_GTK)
    SUBDIRS(gtk)
  ENDIF(HAS_GTK)

  IF(HAS_GLUT)
    SUBDIRS(glut)
  ENDIF(HAS_GLUT)

  IF(HAS_QT)
    SUBDIRS(qt)
  ENDIF(HAS_QT)

ENDIF(WIN32)
