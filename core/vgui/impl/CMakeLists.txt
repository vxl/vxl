# ./oxl/vgui/impl/CMakeLists.txt

INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindGLUT.cmake)

IF(HAS_GLUT)
  SUBDIRS(glut)
ENDIF(HAS_GLUT)

IF(WIN32)
  SUBDIRS(mfc)
ELSE(WIN32)

  INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindGTK.cmake)

  IF(HAS_X11)
    SUBDIRS(X)
  ENDIF(HAS_X11)

  IF(HAS_GLX)
    SUBDIRS(glX)
  ENDIF(HAS_GLX)

  IF(HAS_GTK)
    SUBDIRS(gtk)
  ENDIF(HAS_GTK)

  IF(HAS_QT)
    SUBDIRS(qt)
  ENDIF(HAS_QT)

  IF(HAS_SDL)
    SUBDIRS(SDL)
  ENDIF(HAS_SDL)

  IF(HAS_MOTIF)
    SUBDIRS(Xm)
  ENDIF(HAS_MOTIF)

  IF(HAS_FL)
    SUBDIRS(Fl)
  ENDIF(HAS_FL)

ENDIF(WIN32)
