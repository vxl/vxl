# This is contrib/brl/bbas/bkml/tests/CMakeLists.txt

ADD_EXECUTABLE(bkml_test_all
                test_driver.cxx
                test_bkml.cxx
              )

TARGET_LINK_LIBRARIES(bkml_test_all bkml vgl testlib )
ADD_TEST(bkml_test_bkml ${EXECUTABLE_OUTPUT_PATH}/bkml_test_all test_bkml)
 
ADD_EXECUTABLE(bkml_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bkml_test_include bkml )
