# v3p/tiff/test/CMakeLists.txt


# Include the generated headers
include_directories(${TIFF_INCLUDE_DIR})

#message("TIFF_INCLUDE_DIR=${TIFF_INCLUDE_DIR}; TIFF_LIBRARIES=${TIFF_LIBRARIES} ")

add_executable( tiff_ascii_tag ascii_tag.c )
target_link_libraries( tiff_ascii_tag  ${TIFF_LIBRARIES} )

add_executable( tiff_long_tag long_tag.c check_tag.c )
target_link_libraries( tiff_long_tag  ${TIFF_LIBRARIES}  )

add_executable( tiff_short_tag short_tag.c check_tag.c )
target_link_libraries( tiff_short_tag  ${TIFF_LIBRARIES}  )

add_executable( tiff_strip_rw strip_rw.c
                strip.c
                test_arrays.c              test_arrays.h )
target_link_libraries( tiff_strip_rw  ${TIFF_LIBRARIES} )

add_executable( tiff_rewrite rewrite_tag.c )
target_link_libraries( tiff_rewrite  ${TIFF_LIBRARIES} )

add_executable( tiff_raw_decode raw_decode.c )
target_link_libraries( tiff_raw_decode  ${TIFF_LIBRARIES} )

add_executable( tiff_custom_dir custom_dir.c )
target_link_libraries( tiff_custom_dir  ${TIFF_LIBRARIES} )

add_test( NAME tiff_ascii_tag  COMMAND tiff_ascii_tag   )
add_test( NAME tiff_long_tag   COMMAND tiff_long_tag   )
add_test( NAME tiff_short_tag  COMMAND tiff_short_tag   )
add_test( NAME tiff_strip_rw   COMMAND tiff_strip_rw   )
add_test( NAME tiff_rewrite    COMMAND tiff_rewrite   )
add_test( NAME tiff_raw_decode COMMAND tiff_raw_decode WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"  )
add_test( NAME tiff_custom_dir COMMAND tiff_custom_dir   )

