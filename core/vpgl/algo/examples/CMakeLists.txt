# core/vpgl/algo/examples/CMakeLists.txt

ADD_EXECUTABLE(vpgl_synth_bundle_adjust vpgl_synth_bundle_adjust.cxx)
TARGET_LINK_LIBRARIES( vpgl_synth_bundle_adjust vpgl_algo vgl_algo vnl vgl vul )
