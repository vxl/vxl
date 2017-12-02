# bbas/bpgl/algo/examples/CMakeLists.txt

add_executable(bpgl_synth_bundle_adjust bpgl_synth_bundle_adjust.cxx)
target_link_libraries( bpgl_synth_bundle_adjust bpgl bpgl_algo ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul )
