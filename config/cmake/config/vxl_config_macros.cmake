INCLUDE (${CMAKE_ROOT}/Modules/CheckIncludeFiles.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)

#
# Perform the VXL specific test with status output
#
# Sets the TEST to 1 if the corresponding program could be compiled
# and linked
#

MACRO(PERFORM_CMAKE_TEST FILE TEST)
  IF( VXL_UPDATE_CONFIGURATION )
    UNSET( ${TEST} )
  ENDIF( VXL_UPDATE_CONFIGURATION )
  IF(NOT DEFINED "${TEST}")
    # Perform test
    IF(CMAKE_REQUIRED_LIBRARIES)
      SET(TEST_ADD_LIBRARIES
          "-DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}")
    ENDIF(CMAKE_REQUIRED_LIBRARIES)
    MESSAGE(STATUS "Performing Test ${TEST}")

    if(CMAKE_CXX_STANDARD)
      set(TRY_COMP_CXX_STANDARD
        -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD})
    endif()
    TRY_COMPILE(${TEST}
                ${CMAKE_BINARY_DIR}
                ${vxl_config_SOURCE_DIR}/${FILE}
                CMAKE_FLAGS
                  -DCOMPILE_DEFINITIONS:STRING=${CMAKE_REQUIRED_FLAGS} "${TEST_ADD_LIBRARIES}"
                  ${TRY_COMP_CXX_STANDARD}
                COMPILE_DEFINITIONS -D${TEST}
                OUTPUT_VARIABLE OUTPUT)
    IF(${TEST})
      SET(${TEST} 1 CACHE INTERNAL "VXL test ${FUNCTION}")
      MESSAGE(STATUS "Performing Test ${TEST} - Success")
    ELSE(${TEST})
      MESSAGE(STATUS "Performing Test ${TEST} - Failed")
      SET(${TEST} 0 CACHE INTERNAL "Test ${FUNCTION}")
      FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
                 "Performing Test ${TEST} failed with the following output:\n"
                 "${OUTPUT}\n")
    ENDIF(${TEST})
  ELSE()
    # Have result
    #FOREACH(tst ${TEST})
    #  MESSAGE("Test ${TEST} resulted in ${${tst}}")
    #ENDFOREACH(tst ${TEST})
  ENDIF()
ENDMACRO(PERFORM_CMAKE_TEST FILE TEST)

#
# Perform a custom VXL try compile test with status output
#
# DIR is the directory containing the test project
#
# Sets the TEST to 1 if the corresponding program could be compiled
# and linked
#

MACRO(PERFORM_CMAKE_TEST_CUSTOM DIR TEST)
  IF( VXL_UPDATE_CONFIGURATION )
    UNSET( ${TEST} )
  ENDIF( VXL_UPDATE_CONFIGURATION )
  IF(NOT DEFINED "${TEST}")
    # Perform test
    SET(MACRO_CHECK_FUNCTION_DEFINITIONS
        "-D${TEST} ${CMAKE_REQUIRED_FLAGS}")
    IF(CMAKE_REQUIRED_LIBRARIES)
      SET(TEST_ADD_LIBRARIES
          "-DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}")
    ENDIF(CMAKE_REQUIRED_LIBRARIES)
    MESSAGE(STATUS "Performing Test ${TEST}")

    TRY_COMPILE(${TEST}
                ${CMAKE_BINARY_DIR}/config/${DIR}
                ${vxl_config_SOURCE_DIR}/${DIR}
                ${TEST}
                CMAKE_FLAGS -DCOMPILE_DEFINITIONS:STRING=${MACRO_CHECK_FUNCTION_DEFINITIONS}
                -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
                -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
                "${TEST_ADD_LIBRARIES}"
                OUTPUT_VARIABLE OUTPUT)
    IF(${TEST})
      SET(${TEST} 1 CACHE INTERNAL "VXL test ${FUNCTION}")
      MESSAGE(STATUS "Performing Test ${TEST} - Success")
    ELSE(${TEST})
      MESSAGE(STATUS "Performing Test ${TEST} - Failed")
      SET(${TEST} 0 CACHE INTERNAL "Test ${FUNCTION}")
      FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
                 "Performing Test ${TEST} failed with the following output:\n"
                 "${OUTPUT}\n")
    ENDIF(${TEST})
  ENDIF()
  FILE(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/config/${DIR})
ENDMACRO(PERFORM_CMAKE_TEST_CUSTOM DIR TEST)

#
# Perform the VXL specific try-run test with status output
#
# Sets TEST to 1 if the corresponding program compiles, links, run,
# and returns 0 (indicating success).
#

MACRO(PERFORM_CMAKE_TEST_RUN FILE TEST)
  IF( VXL_UPDATE_CONFIGURATION )
    UNSET( ${TEST} )
  ENDIF( VXL_UPDATE_CONFIGURATION )
  IF(NOT DEFINED "${TEST}")
    # Perform test
    SET(MACRO_CHECK_FUNCTION_DEFINITIONS
        "-D${TEST} ${CMAKE_REQUIRED_FLAGS}")
    IF(CMAKE_REQUIRED_LIBRARIES)
      SET(TEST_ADD_LIBRARIES
          "-DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}")
    ENDIF(CMAKE_REQUIRED_LIBRARIES)
    MESSAGE(STATUS "Performing Test ${TEST}")

    TRY_RUN(${TEST} ${TEST}_COMPILED
            ${CMAKE_BINARY_DIR}
            ${vxl_config_SOURCE_DIR}/${FILE}
            CMAKE_FLAGS -DCOMPILE_DEFINITIONS:STRING=${MACRO_CHECK_FUNCTION_DEFINITIONS}
            "${TEST_ADD_LIBRARIES}"
            OUTPUT_VARIABLE OUTPUT)
    IF(${TEST}_COMPILED)
      IF(${TEST})
        MESSAGE(STATUS "Performing Test ${TEST} - Failed")
        SET(${TEST} 0 CACHE INTERNAL "Test ${FUNCTION} (failed to run)")
        FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
                   "Performing Test ${TEST} failed with the following output:\n"
                   "${OUTPUT}\n")
      ELSE(${TEST})
        SET(${TEST} 1 CACHE INTERNAL "VXL test ${FUNCTION} (successful run)")
        MESSAGE(STATUS "Performing Test ${TEST} - Success")
        FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
                   "Performing Test ${TEST} succeeded with the following output:\n"
                   "${OUTPUT}\n")
      ENDIF(${TEST})
    ELSE(${TEST}_COMPILED)
      MESSAGE(STATUS "Performing Try-Run Test ${TEST} - Test Compilation Failed")
      SET(${TEST} 0 CACHE INTERNAL "Test ${FUNCTION} (failed to compile)")
      FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
                 "Performing Try-Run Test ${TEST} failed to compile with the following output:\n"
                 "${OUTPUT}\n")
    ENDIF(${TEST}_COMPILED)
  ELSE()
    # Have result
    #FOREACH(tst ${TEST})
    #  MESSAGE("Test ${TEST} resulted in ${${tst}}")
    #ENDFOREACH(tst ${TEST})
  ENDIF()
ENDMACRO(PERFORM_CMAKE_TEST_RUN FILE TEST)

#
# Check for include file and if not found, set variable to 0
#

MACRO(PERFORM_CHECK_HEADER FILE VARIABLE)
  IF( VXL_UPDATE_CONFIGURATION )
    UNSET(${VARIABLE})
  ENDIF( VXL_UPDATE_CONFIGURATION )
  CHECK_INCLUDE_FILE_CXX(${FILE} ${VARIABLE})
  IF("x${${VARIABLE}}" STREQUAL "x")
    SET(${VARIABLE} 0)
  ENDIF()
ENDMACRO(PERFORM_CHECK_HEADER FILE VARIABLE)


MACRO(PERFORM_CHECK_C_HEADER FILE VARIABLE)
  IF( VXL_UPDATE_CONFIGURATION )
    UNSET(${VARIABLE})
  ENDIF( VXL_UPDATE_CONFIGURATION )
  CHECK_INCLUDE_FILES(${FILE} ${VARIABLE})
  IF("x${${VARIABLE}}" STREQUAL "x")
    SET(${VARIABLE} 0)
  ENDIF()
ENDMACRO(PERFORM_CHECK_C_HEADER FILE VARIABLE)

#
# Check value of variable and if true, set to VALUE_TRUE, otherwise to
# VALUE_FALSE
#

MACRO(SET_BOOL VAR VALUE_TRUE VALUE_FALSE)
  SET(SET_BOOL_VAR "${VAR}")
  IF(${SET_BOOL_VAR})
    SET(${VAR} ${VALUE_TRUE})
  ELSE(${SET_BOOL_VAR})
    SET(${VAR} ${VALUE_FALSE})
  ENDIF(${SET_BOOL_VAR})
ENDMACRO(SET_BOOL VAR VALUE_TRUE VALUE_FALSE)

#
# Set the variable to inverse of the given value
#

MACRO(SET_INVERT VAR VALUE)
  SET(SET_INVERT_VAR "${VALUE}")
  IF(SET_INVERT_VAR)
    SET(${VAR} "0")
  ELSE(SET_INVERT_VAR)
    SET(${VAR} "1")
  ENDIF(SET_INVERT_VAR)
ENDMACRO(SET_INVERT VAR VALUE)

#
# Check if the type exists (should really go to CMake/Modules)
#

MACRO(CHECK_TYPE_EXISTS TYPE FILES VARIABLE)
  IF( VXL_UPDATE_CONFIGURATION )
    UNSET( ${VARIABLE} )
  ENDIF( VXL_UPDATE_CONFIGURATION )
  IF(NOT DEFINED "${VARIABLE}")
    SET(CHECK_TYPE_EXISTS_CONTENT "/* */\n")
    SET(MACRO_CHECK_TYPE_EXISTS_FLAGS ${CMAKE_REQUIRED_FLAGS})
    IF(CMAKE_REQUIRED_LIBRARIES)
      SET(CHECK_TYPE_EXISTS_LIBS
          "-DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}")
    ENDIF(CMAKE_REQUIRED_LIBRARIES)
    FOREACH(FILE ${FILES})
      SET(CHECK_TYPE_EXISTS_CONTENT
          "${CHECK_TYPE_EXISTS_CONTENT}#include <${FILE}>\n")
    ENDFOREACH(FILE)
    SET(CHECK_TYPE_EXISTS_CONTENT
        "${CHECK_TYPE_EXISTS_CONTENT}\nvoid cmakeRequireSymbol(${TYPE} dummy){(void)dummy;}\nint main()\n{return 0;\n}\n")

    FILE(WRITE ${CMAKE_BINARY_DIR}/CMakeTmp/CheckTypeExists.cxx
         "${CHECK_TYPE_EXISTS_CONTENT}")

    MESSAGE(STATUS "Looking for ${TYPE}")
    TRY_COMPILE(${VARIABLE}
                ${CMAKE_BINARY_DIR}
                ${CMAKE_BINARY_DIR}/CMakeTmp/CheckTypeExists.cxx
                CMAKE_FLAGS
                -DCOMPILE_DEFINITIONS:STRING=${MACRO_CHECK_TYPE_EXISTS_FLAGS}
                "${CHECK_TYPE_EXISTS_LIBS}"
                OUTPUT_VARIABLE OUTPUT)
    IF(${VARIABLE})
      MESSAGE(STATUS "Looking for ${TYPE} - found")
      SET(${VARIABLE} 1 CACHE INTERNAL "Have symbol ${TYPE}")
      FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
           "Determining if the ${TYPE} "
           "exist passed with the following output:\n"
           "${OUTPUT}\nFile ${CMAKE_BINARY_DIR}/CMakeTmp/CheckTypeExists.cxx:\n"
           "${CHECK_TYPE_EXISTS_CONTENT}\n")
    ELSE(${VARIABLE})
      MESSAGE(STATUS "Looking for ${TYPE} - not found.")
      SET(${VARIABLE} "" CACHE INTERNAL "Have symbol ${TYPE}")
      FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
           "Determining if the ${TYPE} "
           "exist failed with the following output:\n"
           "${OUTPUT}\nFile ${CMAKE_BINARY_DIR}/CMakeTmp/CheckTypeExists.c:\n"
           "${CHECK_TYPE_EXISTS_CONTENT}\n")
    ENDIF(${VARIABLE})
  ENDIF()
ENDMACRO(CHECK_TYPE_EXISTS TYPE FILES VARIABLE)

#
# Check if the type exists and if not make result 0
#

MACRO(CHECK_TYPE_EXISTS_ZERO SYMBOL FILES VARIABLE)
  IF( VXL_UPDATE_CONFIGURATION )
    UNSET( ${VARIABLE} )
  ENDIF( VXL_UPDATE_CONFIGURATION )
  CHECK_TYPE_EXISTS("${SYMBOL}" "${FILES}" "${VARIABLE}")
  IF(NOT ${VARIABLE})
    SET(${VARIABLE} 0)
  ENDIF(NOT ${VARIABLE})
ENDMACRO(CHECK_TYPE_EXISTS_ZERO SYMBOL FILES VARIABLE)

#
# Check if the function exists and if not make result 0
#

MACRO(CHECK_FUNCTION_EXISTS_ZERO FUNCTION VARIABLE)
  IF( VXL_UPDATE_CONFIGURATION )
    UNSET(${VARIABLE})
  ENDIF( VXL_UPDATE_CONFIGURATION )
  CHECK_FUNCTION_EXISTS("${FUNCTION}" "${VARIABLE}")
  IF(NOT ${VARIABLE})
    SET(${VARIABLE} 0)
  ENDIF(NOT ${VARIABLE})
ENDMACRO(CHECK_FUNCTION_EXISTS_ZERO FUNCTION VARIABLE)

#
# Determine which C++ type matches the given size
#

MACRO( DETERMINE_TYPE VAR INTEGRAL_TYPE SIZE TYPE_LIST )
  IF( VXL_UPDATE_CONFIGURATION )
    SET( VXL_${VAR} "" )
  ENDIF( VXL_UPDATE_CONFIGURATION )
  # If we've tested this before, use the cached result and don't re-run
  IF( NOT VXL_${VAR} )
    # We can't have IF commands on a macro parameter. For example,
    # IF( INTEGRAL_TYPE ) doesn't seem to work. I think the
    # expansion is done at the wrong time. A macro is not a procedure
    # call. This is a workaround.
    SET( MSG1 "Looking for ${SIZE}-bit int." )
    SET( MSG0 "Looking for ${SIZE}-bit float." )
    SET( MSG ${MSG${INTEGRAL_TYPE}} )

    SET( VXL_${VAR} "void" )
    SET( VXL_HAS_${VAR} 0 )
    FOREACH( TYPE ${TYPE_LIST} )
      # Write the config to a file instead of passing on the command
      # line to avoid issues with spaces. (In "long double", for
      # example)
      FILE(WRITE ${CMAKE_BINARY_DIR}/CMakeTmp/config.h "#define THE_TYPE ${TYPE}\n#define THE_SIZE ${SIZE}\n#define INTEGRAL_TYPE ${INTEGRAL_TYPE}")
      SET( MACRO_DETERMINE_TYPE_FLAGS "-DVXL_HAS_TYPE_OF_SIZE" )
      MESSAGE( STATUS "${MSG} [Checking ${TYPE}...]" )
      TRY_COMPILE(COMPILE_RESULT
            ${CMAKE_BINARY_DIR}
            ${vxl_config_SOURCE_DIR}/vxl_platform_tests.cxx
            CMAKE_FLAGS -DCOMPILE_DEFINITIONS:STRING=${MACRO_DETERMINE_TYPE_FLAGS}
                        -DINCLUDE_DIRECTORIES:STRING=${CMAKE_BINARY_DIR}/CMakeTmp
                        -DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}
            OUTPUT_VARIABLE OUTPUT )
      IF( COMPILE_RESULT )
        SET( VXL_${VAR} ${TYPE} )
        SET( VXL_HAS_${VAR} 1 )
      ELSE( COMPILE_RESULT )
        FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
          "${MSG} Failed to compile with the following output:\n(FLAGS=${MACRO_DETERMINE_TYPE_FLAGS})\n${OUTPUT}\n")
      ENDIF( COMPILE_RESULT )
    ENDFOREACH( TYPE )
    IF( VXL_HAS_${VAR} )
      MESSAGE( STATUS "${MSG} Found ${VXL_${VAR}}." )
    ELSE( VXL_HAS_${VAR} )
      MESSAGE( STATUS "${MSG} Not found." )
    ENDIF( VXL_HAS_${VAR} )
    # Cache the value to prevent a second run of the test
    SET( VXL_${VAR} ${VXL_${VAR}} CACHE INTERNAL "VXL test result" )
    SET( VXL_HAS_${VAR} ${VXL_HAS_${VAR}} CACHE INTERNAL "VXL test result" )
  ENDIF( NOT VXL_${VAR} )
ENDMACRO( DETERMINE_TYPE VAR SIZE TYPE_LIST )


#
# Determine if a particular function is declared in the given header.
#

MACRO(PERFORM_C_CHECK_FUNCTION SYMBOL FILE VARIABLE)
  IF( VXL_UPDATE_CONFIGURATION )
    UNSET(${VARIABLE})
  ENDIF( VXL_UPDATE_CONFIGURATION )
  CHECK_SYMBOL_EXISTS(${SYMBOL} ${FILE} ${VARIABLE})
  IF(${VARIABLE})
    SET(${VARIABLE} "1")
  ELSE(${VARIABLE})
    SET(${VARIABLE} "0")
  ENDIF(${VARIABLE})
ENDMACRO(PERFORM_C_CHECK_FUNCTION SYMBOL FILE VARIABLE)
