# brl/bmvl/bcal/bcam/tests/CMakeLists.txt

LINK_LIBRARIES(bcal)
ADD_EXECUTABLE(bcam_test cam_test.cpp)
ADD_TEST(      bcam_test bcam_test)
