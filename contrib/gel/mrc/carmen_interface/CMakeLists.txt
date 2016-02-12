# gel/mrc/carmen_interface/CMakeLists.txt

if(HAS_CARMEN)

set(carmen_interface_sources
 carmen_interface.cpp carmen_interface.h
 main.cpp
)

add_executable(carmen_interface ${carmen_interface_sources})
target_link_libraries(carmen_interface ${VXL_LIB_PREFIX}vcl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

endif()
