link_libraries( rrel ${VXL_LIB_PREFIX}vnl )

add_executable( homography2d_fit   homography2d_fit.cxx )
add_executable( homography2d_gen   homography2d_gen.cxx )
add_executable( line_fit           line_fit.cxx )
add_executable( line_gen           line_gen.cxx )
