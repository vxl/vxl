# core/vpgl/algo/examples/CMakeLists.txt

add_executable(vpgl_synth_bundle_adjust vpgl_synth_bundle_adjust.cxx)
target_link_libraries( vpgl_synth_bundle_adjust ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul )
