# core/vpgl/algo/examples/CMakeLists.txt

add_executable(vpgl_synth_bundle_adjust vpgl_synth_bundle_adjust.cxx)
target_link_libraries( vpgl_synth_bundle_adjust vpgl_algo vgl_algo vnl vgl vul )
