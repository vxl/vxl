# This is contrib/prip/tests/CMakeLists.txt

LINK_LIBRARIES(vbl vcl vil_algo vdtop vil )

ADD_EXECUTABLE( vdtop_precompute vdtop_precompute.cxx)
ADD_EXECUTABLE( vdtop_kernel_test vdtop_kernel_test.cxx)
ADD_EXECUTABLE( vdtop_well_composed_kernel_test vdtop_well_composed_kernel_test.cxx)

LINK_LIBRARIES(vbl vcl vil_algo vmap vdtop vil )

ADD_EXECUTABLE( vdtop_veinerization_test vdtop_veinerization_test.cxx)
