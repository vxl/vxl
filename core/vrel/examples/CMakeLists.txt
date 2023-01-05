link_libraries( vrel ${VXL_LIB_PREFIX}vnl )

add_executable( r_homography2d_fit   robust_homography2d_fit.cxx )
add_executable( r_homography2d_gen   robust_homography2d_gen.cxx )
add_executable( r_robust_line_fit           robust_line_fit.cxx )
add_executable( r_robust_line_gen           robust_line_gen.cxx )
