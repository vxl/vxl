# mul/mil/tests/CMakeLists.txt

LINK_LIBRARIES( mil_algo mil vpl testlib )

ADD_EXECUTABLE( test_mil test_mil.cxx )
ADD_TEST(       test_mil test_mil     )

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(test_mil.cxx PROPERTIES COMPILE_FLAGS -O0)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_EXECUTABLE( mil_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( mil_test_include mil_algo mil )
