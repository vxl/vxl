# This is contrib/prip/vdtop/tests/CMakeLists.txt

LINK_LIBRARIES(vdtop)

#ADD_EXECUTABLE( vdtop_precompute vdtop_precompute.cxx)
ADD_EXECUTABLE( vdtop_kernel_test vdtop_kernel_test.cxx)
ADD_EXECUTABLE( vdtop_well_composed_kernel_test vdtop_well_composed_kernel_test.cxx)

ADD_EXECUTABLE( vdtop_veinerization_test vdtop_veinerization_test.cxx)

ADD_EXECUTABLE( vdtop_test_include test_include.cxx )
