# vxl/vcsl/examples/CMakeLists.txt

LINK_LIBRARIES( vcsl vnl )

ADD_EXECUTABLE(vcsl_example1   example1.cxx)
ADD_EXECUTABLE(vcsl_tutor vcsl_tutor.cxx)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(example1.cxx PROPERTIES COMPILE_FLAGS -O1)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)
