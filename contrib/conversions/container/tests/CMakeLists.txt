INCLUDE( ${allvxl_SOURCE_DIR}/conversions/container/CMakeListsLink.txt )

LINK_LIBRARIES( testlib )

IF(HAS_COOL)
ELSE(HAS_COOL)
  ADD_DEFINITIONS(-DHAS_NO_COOL=1)
ENDIF(HAS_COOL)

ADD_EXECUTABLE( conversions_container_test_driver
  test_driver.cxx

  test_container.cxx
  test_COOL_container.cxx
)

ADD_TEST( conversions_test_container conversions_container_test_driver test_container )
ADD_TEST( conversions_test_COOL_container conversions_container_test_driver test_COOL_container )
