# mul/mil/tests/CMakeLists.txt

LINK_LIBRARIES( mil_algo mil testlib )

ADD_EXECUTABLE( test_mil test_mil.cxx )
ADD_TEST(       test_mil test_mil     )
