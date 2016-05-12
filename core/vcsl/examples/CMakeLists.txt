# vxl/vcsl/examples/CMakeLists.txt

link_libraries( ${VXL_LIB_PREFIX}vcsl ${VXL_LIB_PREFIX}vnl )

add_executable(vcsl_example1   example1.cxx)
add_executable(vcsl_tutor vcsl_tutor.cxx)

if(CMAKE_COMPILER_IS_GNUCXX)
  set_source_files_properties(example1.cxx PROPERTIES COMPILE_FLAGS -O1)
endif()
