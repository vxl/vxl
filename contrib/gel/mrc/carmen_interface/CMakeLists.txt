# gel/mrc/carmen_interface/CMakeLists.txt

IF (HAS_CARMEN)
ADD_EXECUTABLE(carmen_interface carmen_interface_sources)
SOURCE_FILES(carmen_interface_sources
 carmen_interface.cpp carmen_interface.h
 main.cpp
)
INCLUDE(${allvxl_SOURCE_DIR}/Carmen/CMakeListsLink.txt)
ENDIF (HAS_CARMEN)

INCLUDE(${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt)
