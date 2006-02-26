INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR}/mrc)

ADD_EXECUTABLE( mrc_test_vpgl main.cpp )
TARGET_LINK_LIBRARIES( mrc_test_vpgl vpgl vnl vcl )
