ADD_EXECUTABLE( test_container test_container.cxx )
ADD_TEST( conversions_test_container test_container )

IF(HAS_COOL)
  ADD_EXECUTABLE( test_COOL_container test_COOL_container.cxx )
  ADD_TEST( conversions_test_COOL_container test_COOL_container )
ENDIF(HAS_COOL)

INCLUDE( ${allvxl_SOURCE_DIR}/conversions/container/CMakeListsLink.txt )
