include(CheckIncludeFiles)
include(CheckIncludeFileCXX)
include(CheckFunctionExists)
include(CheckSymbolExists)
include(CheckCXXSymbolExists)
#
# Perform the VXL specific test with status output
#
# Sets the TEST to 1 if the corresponding program could be compiled
# and linked
#

macro(PERFORM_CMAKE_TEST PLFM_TEST_FILE TEST)
  if( VXL_UPDATE_CONFIGURATION )
    unset( ${TEST} )
  endif()
  if(NOT DEFINED "${TEST}")
    # Perform test
    if(CMAKE_REQUIRED_LIBRARIES)
      set(TEST_ADD_LIBRARIES "-DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}")
    endif()
    message(STATUS "Performing Test ${TEST}")

    if(CMAKE_CXX_STANDARD)
      set(TRY_COMP_CXX_STANDARD
        -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD})
    endif()
    try_compile(${TEST}
                ${CMAKE_BINARY_DIR}
                ${PLFM_TEST_FILE}
                CMAKE_FLAGS
                  -DCOMPILE_DEFINITIONS:STRING=${CMAKE_REQUIRED_FLAGS} "${TEST_ADD_LIBRARIES}"
                  ${TRY_COMP_CXX_STANDARD}
                COMPILE_DEFINITIONS -D${TEST}
                OUTPUT_VARIABLE OUTPUT)
    if(${TEST})
      set(${TEST} 1 CACHE INTERNAL "VXL test ${FUNCTION}")
      message(STATUS "Performing Test ${TEST} - Success")
    else()
      message(STATUS "Performing Test ${TEST} - Failed")
      set(${TEST} 0 CACHE INTERNAL "Test ${FUNCTION}")
      file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
                 "Performing Test ${TEST} failed with the following output:\n"
                 "${OUTPUT}\n")
    endif()
  else()
    # Have result
    #foreach(tst ${TEST})
    #  message("Test ${TEST} resulted in ${${tst}}")
    #endforeach()
  endif()
endmacro()

#
# Perform a custom VXL try compile test with status output
#
# DIR is the directory containing the test project
#
# Sets the TEST to 1 if the corresponding program could be compiled
# and linked
#

macro(PERFORM_CMAKE_TEST_CUSTOM DIR TEST)
  if( VXL_UPDATE_CONFIGURATION )
    unset( ${TEST} )
  endif()
  if(NOT DEFINED "${TEST}")
    # Perform test
    set(MACRO_CHECK_FUNCTION_DEFINITIONS
        "-D${TEST} ${CMAKE_REQUIRED_FLAGS}")
    if(CMAKE_REQUIRED_LIBRARIES)
      set(TEST_ADD_LIBRARIES
          "-DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}")
    endif()
    message(STATUS "Performing Test ${TEST}")

    try_compile(${TEST}
                ${CMAKE_BINARY_DIR}/config/${DIR}
                ${vxl_config_SOURCE_DIR}/${DIR}
                ${TEST}
                CMAKE_FLAGS -DCOMPILE_DEFINITIONS:STRING=${MACRO_CHECK_FUNCTION_DEFINITIONS}
                -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
                -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
                "${TEST_ADD_LIBRARIES}"
                OUTPUT_VARIABLE OUTPUT)
    if(${TEST})
      set(${TEST} 1 CACHE INTERNAL "VXL test ${FUNCTION}")
      message(STATUS "Performing Test ${TEST} - Success")
    else()
      message(STATUS "Performing Test ${TEST} - Failed")
      set(${TEST} 0 CACHE INTERNAL "Test ${FUNCTION}")
      file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
                 "Performing Test ${TEST} failed with the following output:\n"
                 "${OUTPUT}\n")
    endif()
  endif()
  file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/config/${DIR})
endmacro()

#
# Perform the VXL specific try-run test with status output
#
# Sets TEST to 1 if the corresponding program compiles, links, run,
# and returns 0 (indicating success).
#

macro(PERFORM_CMAKE_TEST_RUN PLFM_TEST_FILE TEST)
  if( VXL_UPDATE_CONFIGURATION )
    unset( ${TEST} )
  endif()
  if(NOT DEFINED "${TEST}")
    # Perform test
    set(MACRO_CHECK_FUNCTION_DEFINITIONS
        "-D${TEST} ${CMAKE_REQUIRED_FLAGS}")
    if(CMAKE_REQUIRED_LIBRARIES)
      set(TEST_ADD_LIBRARIES
          "-DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}")
    endif()
    message(STATUS "Performing Test ${TEST}")

    try_run(${TEST} ${TEST}_COMPILED
            ${CMAKE_BINARY_DIR}
            ${PLFM_TEST_FILE}
            CMAKE_FLAGS -DCOMPILE_DEFINITIONS:STRING=${MACRO_CHECK_FUNCTION_DEFINITIONS}
            "${TEST_ADD_LIBRARIES}"
            OUTPUT_VARIABLE OUTPUT)
    if(${TEST}_COMPILED)
      if(${TEST})
        message(STATUS "Performing Test ${TEST} - Failed")
        set(${TEST} 0 CACHE INTERNAL "Test ${FUNCTION} (failed to run)")
        file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
                   "Performing Test ${TEST} failed with the following output:\n"
                   "${OUTPUT}\n")
      else()
        set(${TEST} 1 CACHE INTERNAL "VXL test ${FUNCTION} (successful run)")
        message(STATUS "Performing Test ${TEST} - Success")
        file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
                   "Performing Test ${TEST} succeeded with the following output:\n"
                   "${OUTPUT}\n")
      endif()
    else()
      message(STATUS "Performing Try-Run Test ${TEST} - Test Compilation Failed")
      set(${TEST} 0 CACHE INTERNAL "Test ${FUNCTION} (failed to compile)")
      file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
                 "Performing Try-Run Test ${TEST} failed to compile with the following output:\n"
                 "${OUTPUT}\n")
    endif()
  else()
    # Have result
    #foreach(tst ${TEST})
    #  message("Test ${TEST} resulted in ${${tst}}")
    #endforeach()
  endif()
endmacro()

#
# Check for include file and if not found, set variable to 0
#

macro(PERFORM_CHECK_HEADER PLFM_TEST_FILE VARIABLE)
  if( VXL_UPDATE_CONFIGURATION )
    unset(${VARIABLE})
  endif()
  CHECK_INCLUDE_FILE_CXX(${PLFM_TEST_FILE} ${VARIABLE})
  if("x${${VARIABLE}}" STREQUAL "x")
    set(${VARIABLE} 0)
  endif()
endmacro()


macro(PERFORM_CHECK_C_HEADER PLFM_TEST_FILE VARIABLE)
  if( VXL_UPDATE_CONFIGURATION )
    unset(${VARIABLE})
  endif()
  CHECK_INCLUDE_FILES(${PLFM_TEST_FILE} ${VARIABLE})
  if("x${${VARIABLE}}" STREQUAL "x")
    set(${VARIABLE} 0)
  endif()
endmacro()

#
# Check value of variable and if true, set to VALUE_TRUE, otherwise to
# VALUE_FALSE
#

macro(SET_BOOL VAR VALUE_TRUE VALUE_FALSE)
  set(SET_BOOL_VAR "${VAR}")
  if(${SET_BOOL_VAR})
    set(${VAR} ${VALUE_TRUE})
  else()
    set(${VAR} ${VALUE_FALSE})
  endif()
endmacro()

#
# Set the variable to inverse of the given value
#

macro(SET_INVERT VAR VALUE)
  set(SET_INVERT_VAR "${VALUE}")
  if(SET_INVERT_VAR)
    set(${VAR} "0")
  else()
    set(${VAR} "1")
  endif()
endmacro()

#
# Check if the type exists (should really go to CMake/Modules)
#

macro(CHECK_TYPE_EXISTS TYPE FILES VARIABLE)
  if( VXL_UPDATE_CONFIGURATION )
    unset( ${VARIABLE} )
  endif()
  if(NOT DEFINED "${VARIABLE}")
    set(CHECK_TYPE_EXISTS_CONTENT "/* */\n")
    set(MACRO_CHECK_TYPE_EXISTS_FLAGS ${CMAKE_REQUIRED_FLAGS})
    if(CMAKE_REQUIRED_LIBRARIES)
      set(CHECK_TYPE_EXISTS_LIBS
          "-DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}")
    endif()
    foreach(PLFM_TEST_FILE ${FILES})
      set(CHECK_TYPE_EXISTS_CONTENT
          "${CHECK_TYPE_EXISTS_CONTENT}#include <${PLFM_TEST_FILE}>\n")
    endforeach()
    set(CHECK_TYPE_EXISTS_CONTENT
        "${CHECK_TYPE_EXISTS_CONTENT}\nvoid cmakeRequireSymbol(${TYPE} dummy){(void)dummy;}\nint main()\n{return 0;\n}\n")

    file(WRITE ${CMAKE_BINARY_DIR}/CMakeTmp/CheckTypeExists.cxx
         "${CHECK_TYPE_EXISTS_CONTENT}")

    message(STATUS "Looking for ${TYPE}")
    try_compile(${VARIABLE}
                ${CMAKE_BINARY_DIR}
                ${CMAKE_BINARY_DIR}/CMakeTmp/CheckTypeExists.cxx
                CMAKE_FLAGS
                -DCOMPILE_DEFINITIONS:STRING=${MACRO_CHECK_TYPE_EXISTS_FLAGS}
                "${CHECK_TYPE_EXISTS_LIBS}"
                OUTPUT_VARIABLE OUTPUT)
    if(${VARIABLE})
      message(STATUS "Looking for ${TYPE} - found")
      set(${VARIABLE} 1 CACHE INTERNAL "Have symbol ${TYPE}")
      file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
           "Determining if the ${TYPE} "
           "exist passed with the following output:\n"
           "${OUTPUT}\nFile ${CMAKE_BINARY_DIR}/CMakeTmp/CheckTypeExists.cxx:\n"
           "${CHECK_TYPE_EXISTS_CONTENT}\n")
    else()
      message(STATUS "Looking for ${TYPE} - not found.")
      set(${VARIABLE} "" CACHE INTERNAL "Have symbol ${TYPE}")
      file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
           "Determining if the ${TYPE} "
           "exist failed with the following output:\n"
           "${OUTPUT}\nFile ${CMAKE_BINARY_DIR}/CMakeTmp/CheckTypeExists.c:\n"
           "${CHECK_TYPE_EXISTS_CONTENT}\n")
    endif()
  endif()
endmacro()

#
# Check if the type exists and if not make result 0
#

macro(CHECK_TYPE_EXISTS_ZERO SYMBOL FILES VARIABLE)
  if( VXL_UPDATE_CONFIGURATION )
    unset( ${VARIABLE} )
  endif()
  CHECK_TYPE_EXISTS("${SYMBOL}" "${FILES}" "${VARIABLE}")
  if(NOT ${VARIABLE})
    set(${VARIABLE} 0)
  endif()
endmacro()

#
# Check if the function exists and if not make result 0
#

macro(CHECK_FUNCTION_EXISTS_ZERO FUNCTION VARIABLE)
  if( VXL_UPDATE_CONFIGURATION )
    unset(${VARIABLE})
  endif()
  CHECK_FUNCTION_EXISTS("${FUNCTION}" "${VARIABLE}")
  if(NOT ${VARIABLE})
    set(${VARIABLE} 0)
  endif()
endmacro()

#
# Determine which C++ type matches the given size
#

macro( DETERMINE_TYPE VAR INTEGRAL_TYPE SIZE TYPE_LIST )
  if( VXL_UPDATE_CONFIGURATION )
    set( VXL_${VAR} "" )
  endif()
  # If we've tested this before, use the cached result and don't re-run
  if( NOT VXL_${VAR} )
    # We can't have IF commands on a macro parameter. For example,
    # if( INTEGRAL_TYPE ) doesn't seem to work. I think the
    # expansion is done at the wrong time. A macro is not a procedure
    # call. This is a workaround.
    set( MSG1 "Looking for ${SIZE}-bit int." )
    set( MSG0 "Looking for ${SIZE}-bit float." )
    set( MSG ${MSG${INTEGRAL_TYPE}} )

    set( VXL_${VAR} "void" )
    set( VXL_HAS_${VAR} 0 )
    foreach( TYPE ${TYPE_LIST} )
      # Write the config to a file instead of passing on the command
      # line to avoid issues with spaces. (In "long double", for
      # example)
      file(WRITE ${CMAKE_BINARY_DIR}/CMakeTmp/config.h "#define THE_TYPE ${TYPE}\n#define THE_SIZE ${SIZE}\n#define INTEGRAL_TYPE ${INTEGRAL_TYPE}")
      set( MACRO_DETERMINE_TYPE_FLAGS "-DVXL_HAS_TYPE_OF_SIZE" )
      message( STATUS "${MSG} [Checking ${TYPE}...]" )
      try_compile(COMPILE_RESULT
            ${CMAKE_BINARY_DIR}
            ${VXL_PLFM_TEST_FILE}
            CMAKE_FLAGS -DCOMPILE_DEFINITIONS:STRING=${MACRO_DETERMINE_TYPE_FLAGS}
                        -DINCLUDE_DIRECTORIES:STRING=${CMAKE_BINARY_DIR}/CMakeTmp
                        -DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}
            OUTPUT_VARIABLE OUTPUT )
      if( COMPILE_RESULT )
        set( VXL_${VAR} ${TYPE} )
        set( VXL_HAS_${VAR} 1 )
      else()
        file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
          "${MSG} Failed to compile with the following output:\n(FLAGS=${MACRO_DETERMINE_TYPE_FLAGS})\n${OUTPUT}\n")
      endif()
    endforeach()
    if( VXL_HAS_${VAR} )
      message( STATUS "${MSG} Found ${VXL_${VAR}}." )
    else()
      message( STATUS "${MSG} Not found." )
    endif()
    # Cache the value to prevent a second run of the test
    set( VXL_${VAR} ${VXL_${VAR}} CACHE INTERNAL "VXL test result" )
    set( VXL_HAS_${VAR} ${VXL_HAS_${VAR}} CACHE INTERNAL "VXL test result" )
  endif()
endmacro()


#
# Determine if a particular function is declared in the given header.
#

macro(PERFORM_C_CHECK_FUNCTION SYMBOL PLFM_TEST_FILE VARIABLE)
  if( VXL_UPDATE_CONFIGURATION )
    unset(${VARIABLE})
  endif()
  CHECK_CXX_SYMBOL_EXISTS(${SYMBOL} ${PLFM_TEST_FILE} ${VARIABLE})
  if(${VARIABLE})
    set(${VARIABLE} "1")
  else()
    set(${VARIABLE} "0")
  endif()
endmacro()
