# oxl/mvl/examples/CMakeLists.txt

link_libraries( mvl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul )

add_executable(mvl_multi_view_matches_example mvl_multi_view_matches_example.cxx)
add_executable(compute_FMatrix_example        compute_FMatrix_example.cxx)
add_executable(PMatrixDecompCR_example        PMatrixDecompCR_example.cxx)
