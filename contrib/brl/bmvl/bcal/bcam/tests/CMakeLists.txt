# brl/bmvl/bcal/bcam/tests/CMakeLists.txt

LINK_LIBRARIES(bcal)
ADD_EXECUTABLE(bcam_test cam_test.cxx)
ADD_TEST(      bcam_test bcam_test ${BRL_INCLUDE_DIR}/bmvl/bcal/bcam/tests)
