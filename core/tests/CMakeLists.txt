# This is core/tests/CMakeLists.txt

# header to tell test_build_info values of vxl_BUILD_DIR, BUILD_NAME,
# and SITE from CMake
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/test_build_info.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/test_build_info.h
                @ONLY IMMEDIATE)

# needed for generated test_build_info.h
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

# these defines are set (possibly) so the core_test_config test can
# report whether CMake has set these CMake variables
IF( UNIX )
  ADD_DEFINITIONS( -DCMAKE_DEFINES_VARIABLE_UNIX )
ENDIF( UNIX )
IF( CYGWIN )
  ADD_DEFINITIONS( -DCMAKE_DEFINES_VARIABLE_CYGWIN )
ENDIF( CYGWIN )
IF( MINGW )
  ADD_DEFINITIONS( -DCMAKE_DEFINES_VARIABLE_MINGW )
ENDIF( MINGW )
IF( WIN32 )
  ADD_DEFINITIONS( -DCMAKE_DEFINES_VARIABLE_WIN32 )
ENDIF( WIN32 )
IF( APPLE )
  ADD_DEFINITIONS( -DCMAKE_DEFINES_VARIABLE_APPLE )
ENDIF( APPLE )

ADD_EXECUTABLE( core_test_all
  test_driver.cxx
  test_config.cxx
  test_build_info.cxx test_build_info.h.in
)
TARGET_LINK_LIBRARIES( core_test_all testlib vcl )

ADD_TEST( core_test_config core_test_all test_config )
ADD_TEST( core_test_build_info core_test_all test_build_info )

# a simple Dart notes file, in case test_build_info fails
IF( COMMAND WRITE_FILE )
  WRITE_FILE(
    "${vxl_BINARY_DIR}/Testing/HTML/TestingResults/Sites/${SITE}/${BUILDNAME}/BuildNameNotes.xml"
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<Site BuildName=\"${BUILDNAME}\" Name=\"${SITE}\">\n"
    "<BuildNameNotes>\n"
    "<Note>\n"
    "<DateTime>unset</DateTime>\n"
    "<Text>\n"
    "The full test_build_info did not run.\n"
    "Here is some basic information:\n"
    "vxl_SOURCE_DIR = ${vxl_SOURCE_DIR}\n"
    "vxl_BINARY_DIR = ${vxl_BINARY_DIR}\n"
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
ENDIF( COMMAND WRITE_FILE )

ADD_EXECUTABLE( vxl_echo vxl_echo.cxx )
TARGET_LINK_LIBRARIES( vxl_echo vcl )

ADD_TEST( cmake_config vxl_echo
          MAKECOMMAND: ${MAKECOMMAND} nl
          CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS} nl
          CMAKE_C_FLAGS: ${CMAKE_C_FLAGS} nl
          BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS} nl
          CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS} nl
          CMAKE_MODULE_LINKER_FLAGS: ${CMAKE_MODULE_LINKER_FLAGS} nl
          CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS} nl
          VXL_USE_CMAKE_CONFIGURE: ${VXL_USE_CMAKE_CONFIGURE} nl
          )
