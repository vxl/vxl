# mul/mbl/tests/CMakeLists.txt

LINK_LIBRARIES(mbl vbl vul vpl testlib )

ADD_EXECUTABLE( test_mbl test_mbl.cxx )
ADD_TEST(   mbl_test_mbl test_mbl     )

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(test_mbl.cxx PROPERTIES COMPILE_FLAGS -O0)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_EXECUTABLE( mbl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( mbl_test_include mbl )
