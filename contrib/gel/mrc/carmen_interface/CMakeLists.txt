# gel/mrc/carmen_interface/CMakeLists.txt

IF (HAS_CARMEN)

SET(carmen_interface_sources
 carmen_interface.cpp carmen_interface.h
 main.cpp
)

ADD_EXECUTABLE(carmen_interface ${carmen_interface_sources})
TARGET_LINK_LIBRARIES(carmen_interface vcl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF (HAS_CARMEN)
