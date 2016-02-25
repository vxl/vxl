# mul/pdf1d/tests/CMakeLists.txt

add_executable(pdf1d_test_all test_pdf1d.cxx )
target_link_libraries( pdf1d_test_all pdf1d mbl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}testlib )
add_test( NAME pdf1d_test_pdf1d COMMAND $<TARGET_FILE:pdf1d_test_all>)

add_executable( pdf1d_test_include test_include.cxx )
target_link_libraries( pdf1d_test_include pdf1d )
