#
# try to find Qt, but use the FindQt.cmake file from CMake
#

IF(NOT HAS_QT)
  INCLUDE(${CMAKE_ROOT}/Modules/FindQt.cmake)

  IF(QT_INCLUDE_PATH)
    IF(QT_QT_LIBRARY)
      INCLUDE_DIRECTORIES( ${QT_INCLUDE_PATH} )
      INCLUDE(${CMAKE_ROOT}/Modules/LinkQT.cmake)
      SET( HAS_QT "YES" )
    ENDIF(QT_QT_LIBRARY)
  ENDIF(QT_INCLUDE_PATH)

ENDIF(NOT HAS_QT)
