# ./tbl/vipl/tests/CMakeLists.txt

ADD_DEFINITIONS(-DINSTANTIATE_TEMPLATES)

ADD_EXECUTABLE( vipl_test_all
  test_driver.cxx        test_driver.h

  vipl_test_threshold.cxx
  vipl_test_erode_disk.cxx
  vipl_test_gaussian_convolution.cxx
  vipl_test_histogram.cxx   # uses vnl_vector as container; remove if BUILD_CORE_NUMERICS is OFF
)
TARGET_LINK_LIBRARIES( vipl_test_all vipl vil testlib )

ADD_TEST(vipl_test_threshold            ${EXECUTABLE_OUTPUT_PATH}/vipl_test_all vipl_test_threshold)
ADD_TEST(vipl_test_erode_disk           ${EXECUTABLE_OUTPUT_PATH}/vipl_test_all vipl_test_erode_disk)
ADD_TEST(vipl_test_gaussian_convolution ${EXECUTABLE_OUTPUT_PATH}/vipl_test_all vipl_test_gaussian_convolution)

IF (BUILD_CORE_NUMERICS)
  TARGET_LINK_LIBRARIES( vipl_test_all vnl )
  ADD_TEST(vipl_test_histogram            ${EXECUTABLE_OUTPUT_PATH}/vipl_test_all vipl_test_histogram)
ENDIF (BUILD_CORE_NUMERICS)

ADD_EXECUTABLE( vipl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vipl_test_include vipl )
ADD_EXECUTABLE( vipl_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vipl_test_template_include vipl )
