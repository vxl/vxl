# Verify whether the png library in use is actually v3p/png:
INCLUDE(${MODULE_PATH}/FindPNG.cmake)
IF(PNG_PNG_INCLUDE_DIR MATCHES "v3p.png")
  SET(PNG_IS_V3P "YES")
ENDIF(PNG_PNG_INCLUDE_DIR MATCHES "v3p.png")

# Only build and run this test if png is indeed v3p/png:
IF(BUILD_TESTING AND PNG_IS_V3P)
  INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})
  LINK_LIBRARIES(${PNG_LIBRARIES})
  ADD_DEFINITIONS(${PNG_DEFINITIONS})

  ADD_EXECUTABLE(pngtest pngtest.c)
  ADD_TEST(pngtest pngtest ${CMAKE_CURRENT_SOURCE_DIR}/pngtest.png)
ENDIF(BUILD_TESTING AND PNG_IS_V3P)
