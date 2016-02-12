# This is mul/mfpf/tools/CMakeLists.txt

link_libraries( mfpf msm ${VXL_LIB_PREFIX}vil_algo )

add_executable( mfpf_build_finder mfpf_build_finder.cxx )
add_executable( mfpf_test_finder mfpf_test_finder.cxx )
