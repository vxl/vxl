# This is contrib/prip/vdtop/tests/CMakeLists.txt

link_libraries(vdtop vmap ${VXL_LIB_PREFIX}vil)

#add_executable(vdtop_precompute vdtop_precompute.cxx)
add_executable(vdtop_kernel_test               vdtop_kernel_test.cxx)
add_executable(vdtop_well_composed_kernel_test vdtop_well_composed_kernel_test.cxx)
add_executable(vdtop_veinerization_test        vdtop_veinerization_test.cxx)

#add_test( NAME vdtop_precompute COMMAND $<TARGET_FILE:vdtop_precompute)>
add_test( NAME vdtop_kernel_test COMMAND $<TARGET_FILE:vdtop_kernel_test>               ${CMAKE_CURRENT_SOURCE_DIR}/lena.org.pgm)
add_test( NAME vdtop_well_composed_kernel_test COMMAND $<TARGET_FILE:vdtop_well_composed_kernel_test> ${CMAKE_CURRENT_SOURCE_DIR}/lena.org.pgm)
# temporarily disabling this test: it causes segfaults
#add_test( NAME vdtop_veinerization_test COMMAND $<TARGET_FILE:vdtop_veinerization_test>        ${CMAKE_CURRENT_SOURCE_DIR}/lena.org.pgm)

add_executable(vdtop_test_include test_include.cxx)
add_executable(vdtop_test_template_include test_template_include.cxx)
