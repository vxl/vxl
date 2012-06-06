# This is mul/mfpf/tools/CMakeLists.txt

LINK_LIBRARIES( mfpf msm vil_algo )

ADD_EXECUTABLE( mfpf_build_finder mfpf_build_finder.cxx )
ADD_EXECUTABLE( mfpf_test_finder mfpf_test_finder.cxx )
