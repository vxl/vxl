# This is contrib/prip/vdtop/tests/CMakeLists.txt

LINK_LIBRARIES(vdtop vmap vil)

#ADD_EXECUTABLE(vdtop_precompute vdtop_precompute.cxx)
ADD_EXECUTABLE(vdtop_kernel_test               vdtop_kernel_test.cxx)
ADD_EXECUTABLE(vdtop_well_composed_kernel_test vdtop_well_composed_kernel_test.cxx)
ADD_EXECUTABLE(vdtop_veinerization_test        vdtop_veinerization_test.cxx)

#ADD_TEST( NAME vdtop_precompute COMMAND $<TARGET_FILE:vdtop_precompute)>
ADD_TEST( NAME vdtop_kernel_test COMMAND $<TARGET_FILE:vdtop_kernel_test>               ${CMAKE_CURRENT_SOURCE_DIR}/lena.org.pgm)
ADD_TEST( NAME vdtop_well_composed_kernel_test COMMAND $<TARGET_FILE:vdtop_well_composed_kernel_test> ${CMAKE_CURRENT_SOURCE_DIR}/lena.org.pgm)
# temporarily disabling this test: it causes segfaults
#ADD_TEST( NAME vdtop_veinerization_test COMMAND $<TARGET_FILE:vdtop_veinerization_test>        ${CMAKE_CURRENT_SOURCE_DIR}/lena.org.pgm)

ADD_EXECUTABLE(vdtop_test_include test_include.cxx)
ADD_EXECUTABLE(vdtop_test_template_include test_template_include.cxx)
