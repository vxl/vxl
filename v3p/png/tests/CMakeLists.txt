include(${VXL_CMAKE_DIR}/FindPNG.cmake)

include_directories(${PNG_INCLUDE_DIR})
if(UNIX)
  link_libraries(m)
endif()
add_definitions(${PNG_DEFINITIONS} -DPNG_FREESTANDING_TESTS)

add_executable(pngtest pngtest.c)
target_link_libraries(pngtest ${PNG_LIBRARIES})

add_executable(pngvalid pngvalid.c)
target_link_libraries(pngvalid ${PNG_LIBRARIES})

add_test( NAME pngtest COMMAND $<TARGET_FILE:pngtest> ${CMAKE_CURRENT_SOURCE_DIR}/pngtest.png)
add_test( NAME pngvalid COMMAND $<TARGET_FILE:pngvalid> )
