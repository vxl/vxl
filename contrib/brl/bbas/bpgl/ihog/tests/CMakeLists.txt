# vpgl/ihog/tests/CMakeLists.txt

ADD_EXECUTABLE( ihog_test_all
  test_driver.cxx
  test_minimizer.cxx
)

TARGET_LINK_LIBRARIES( ihog_test_all ihog testlib vul vnl vgl vil)
ADD_TEST( ihog_test_minimizer         ihog_test_all test_minimizer )

ADD_EXECUTABLE( ihog_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( ihog_test_include ihog)
