# gel/mrc/carmen_interface/CMakeLists.txt

IF (HAS_CARMEN)

SET(carmen_interface_sources
 carmen_interface.cpp carmen_interface.h
 main.cpp
)

ADD_EXECUTABLE(carmen_interface ${carmen_interface_sources})
TARGET_LINK_LIBRARIES(carmen_interface vcl)

ENDIF (HAS_CARMEN)
