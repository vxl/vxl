#
# try to find Qt, but use the FindQt.cmake file from CMake
#

IF(NOT HAS_QT)


  INCLUDE(${CMAKE_ROOT}/Modules/FindQt.cmake)

  IF(QT_INCLUDE_PATH)
    IF(QT_QT_LIBRARY)
      IF (WIN32)
        SET(QT_LIBRARIES imm32.lib ws2_32.lib)
        #Ensure that qt.lib is last
      ENDIF (WIN32)
      SET( QT_LIBRARIES ${QT_LIBRARIES} ${QT_QT_LIBRARY} )
      SET( HAS_QT "YES" )
      ADD_DEFINITIONS( -DHAS_QT )
    ENDIF(QT_QT_LIBRARY)
  ENDIF(QT_INCLUDE_PATH)

ENDIF(NOT HAS_QT)
