# This is core/tests/CMakeLists.txt

# The tests here are general vxl configuration tests, and should not
# depend on any libraries other than vcl and testlib.

# header to tell test_build_info values of vxl_BUILD_DIR, BUILD_NAME,
# and SITE from CMake
vxl_configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/test_build_info.h.in
                    ${CMAKE_CURRENT_BINARY_DIR}/test_build_info.h
                    core/tests )

# needed for generated test_build_info.h
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

# these defines are set (possibly) so the core_test_config test can
# report whether CMake has set these CMake variables
if( UNIX )
  add_definitions( -DCMAKE_DEFINES_VARIABLE_UNIX )
endif()
if( CYGWIN )
  add_definitions( -DCMAKE_DEFINES_VARIABLE_CYGWIN )
endif()
if( MINGW )
  add_definitions( -DCMAKE_DEFINES_VARIABLE_MINGW )
endif()
if( WIN32 )
  add_definitions( -DCMAKE_DEFINES_VARIABLE_WIN32 )
endif()
if( APPLE )
  add_definitions( -DCMAKE_DEFINES_VARIABLE_APPLE )
endif()

add_executable( core_test_all
  test_driver.cxx
  test_config.cxx
  test_build_info.cxx
)
target_link_libraries( core_test_all ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vcl )

add_executable( vxl_test_include test_include.cxx )

add_test( NAME core_test_config COMMAND $<TARGET_FILE:core_test_all> test_config )
add_test( NAME core_test_build_info COMMAND $<TARGET_FILE:core_test_all> test_build_info )

# a simple Dart notes file, in case test_build_info fails
if( COMMAND WRITE_FILE )
  write_file(
    "${CMAKE_BINARY_DIR}/Testing/HTML/TestingResults/Sites/${SITE}/${BUILDNAME}/BuildNameNotes.xml"
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<Site BuildName=\"${BUILDNAME}\" Name=\"${SITE}\">\n"
    "<BuildNameNotes>\n"
    "<Note>\n"
    "<DateTime>unset</DateTime>\n"
    "<Text>\n"
    "The full test_build_info did not run.\n"
    "Here is some basic information:\n"
    "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}\n"
    "CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}\n"
    "CMAKE_C_COMPILER = ${CMAKE_C_COMPILER}\n"
    "CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}\n"
    "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}\n"
    "CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}\n"
    "CMAKE_SYSTEM = ${CMAKE_SYSTEM}\n"
    "CMAKE_MAKE_PROGRAM = ${CMAKE_MAKE_PROGRAM}\n"
    "</Text>\n"
    "</Note>\n"
    "</BuildNameNotes>\n"
    "</Site>\n"
  )
endif()

add_executable( vxl_echo vxl_echo.cxx )
target_link_libraries( vxl_echo ${VXL_LIB_PREFIX}vcl )


# Deal with unavailablility of file(TO_CMAKE_PATH ...)
set( MYMAKECOMMAND ${MAKECOMMAND})

if(CMAKE_MAJOR_VERSION GREATER 2)
 file(TO_CMAKE_PATH ${MAKECOMMAND} MYMAKECOMMAND)
endif()

if(CMAKE_MAJOR_VERSION EQUAL 2)
  if(CMAKE_MINOR_VERSION  GREATER 3)
     file(TO_CMAKE_PATH ${MAKECOMMAND} MYMAKECOMMAND)
  endif()
endif()


add_test( NAME cmake_config COMMAND $<TARGET_FILE:vxl_echo> nl
          MAKECOMMAND: ${MYMAKECOMMAND} nl
          CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS} nl
          CMAKE_C_FLAGS: ${CMAKE_C_FLAGS} nl
          BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS} nl
          CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS} nl
          CMAKE_MODULE_LINKER_FLAGS: ${CMAKE_MODULE_LINKER_FLAGS} nl
          CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS} nl
          VXL_USE_CMAKE_CONFIGURE: ${VXL_USE_CMAKE_CONFIGURE} nl
          )
