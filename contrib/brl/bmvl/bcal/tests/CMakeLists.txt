# brl/bmvl/bcal/bcam/tests/CMakeLists.txt

LINK_LIBRARIES(bcal)
ADD_EXECUTABLE(bcal_test bcal_test.cxx)
ADD_TEST(      bcal_test bcal_test ${BRL_INCLUDE_DIR}/bmvl/bcal/tests)
