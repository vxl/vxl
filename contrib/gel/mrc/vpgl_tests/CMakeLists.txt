ADD_EXECUTABLE( mrc_test_vpgl main.cpp )
TARGET_LINK_LIBRARIES( mrc_test_vpgl vpgl vcsl vnl vbl vcl )
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/gel/mrc)
