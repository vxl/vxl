INCLUDE( ${allvxl_SOURCE_DIR}/conversions/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/CMakeListsLink.txt )

ADD_EXECUTABLE( test_container test_container.cxx )
ADD_TEST( conversions_test_container test_container )

IF(HAS_COOL)
  LINK_DIRECTORIES( COOL Numerics )
  LINK_LIBRARIES( TJCOOL iue-math )
  ADD_EXECUTABLE( test_COOL_container test_COOL_container.cxx )
  ADD_TEST( conversions_test_COOL_container test_COOL_container )
ENDIF(HAS_COOL)
