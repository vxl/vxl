# bbas/bpgl/algo/examples/CMakeLists.txt

add_executable(bpgl_synth_bundle_adjust bpgl_synth_bundle_adjust.cxx)
target_link_libraries( bpgl_synth_bundle_adjust bpgl bpgl_algo vpgl_algo vgl_algo vnl vgl vul )
