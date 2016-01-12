# v3p/tiff/test/CMakeLists.txt

# Include the generated headers
INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})

#MESSAGE("TIFF_INCLUDE_DIR=${TIFF_INCLUDE_DIR}; TIFF_LIBRARIES=${TIFF_LIBRARIES} ")

ADD_EXECUTABLE( tiff_ascii_tag ascii_tag.c )
TARGET_LINK_LIBRARIES( tiff_ascii_tag  ${TIFF_LIBRARIES} )

ADD_EXECUTABLE( tiff_long_tag long_tag.c check_tag.c )
TARGET_LINK_LIBRARIES( tiff_long_tag  ${TIFF_LIBRARIES}  )

ADD_EXECUTABLE( tiff_short_tag short_tag.c check_tag.c )
TARGET_LINK_LIBRARIES( tiff_short_tag  ${TIFF_LIBRARIES}  )

ADD_EXECUTABLE( tiff_strip_rw strip_rw.c
                strip.c
                test_arrays.c              test_arrays.h )
TARGET_LINK_LIBRARIES( tiff_strip_rw  ${TIFF_LIBRARIES} )

ADD_EXECUTABLE( tiff_rewrite rewrite_tag.c )
TARGET_LINK_LIBRARIES( tiff_rewrite  ${TIFF_LIBRARIES} )

ADD_EXECUTABLE( tiff_raw_decode raw_decode.c )
TARGET_LINK_LIBRARIES( tiff_raw_decode  ${TIFF_LIBRARIES} )

ADD_EXECUTABLE( tiff_custom_dir custom_dir.c )
TARGET_LINK_LIBRARIES( tiff_custom_dir  ${TIFF_LIBRARIES} )

ADD_TEST( NAME tiff_ascii_tag  COMMAND tiff_ascii_tag   )
ADD_TEST( NAME tiff_long_tag   COMMAND tiff_long_tag   )
ADD_TEST( NAME tiff_short_tag  COMMAND tiff_short_tag   )
ADD_TEST( NAME tiff_strip_rw   COMMAND tiff_strip_rw   )
ADD_TEST( NAME tiff_rewrite    COMMAND tiff_rewrite   )
ADD_TEST( NAME tiff_raw_decode COMMAND tiff_raw_decode  "${CMAKE_CURRENT_SOURCE_DIR}"  )
ADD_TEST( NAME tiff_custom_dir COMMAND tiff_custom_dir   )

