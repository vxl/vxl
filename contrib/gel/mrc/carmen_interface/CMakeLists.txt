# gel/mrc/carmen_interface/CMakeLists.txt

IF (HAS_CARMEN)

SOURCE_FILES(carmen_interface_sources
 carmen_interface.cpp carmen_interface.h
 main.cpp
)

ADD_EXECUTABLE(carmen_interface carmen_interface_sources)
TARGET_LINK_LIBRARIES(carmen_interface vul vbl vcl)

ENDIF (HAS_CARMEN)
