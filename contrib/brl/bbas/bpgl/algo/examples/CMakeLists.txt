# bbas/bpgl/algo/examples/CMakeLists.txt

ADD_EXECUTABLE(bpgl_synth_bundle_adjust bpgl_synth_bundle_adjust.cxx)
TARGET_LINK_LIBRARIES( bpgl_synth_bundle_adjust bpgl bpgl_algo vpgl_algo vgl_algo vnl vgl vul )
