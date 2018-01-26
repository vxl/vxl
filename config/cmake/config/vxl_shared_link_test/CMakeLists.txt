# vxl/config/cmake/config/vxl_shared_link_test/CMakeLists.txt
#
cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)
# Set policies consistent with newer versions of cmake
# to ease integration with projects that require newer
# cmake versions.

foreach(p
    ## Only policies introduced after the cmake_minimum_required
    ## version need to explicitly be set to NEW.

    ##----- Policies Introduced by CMake 3.10¶
    CMP0071  #: Let AUTOMOC and AUTOUIC process GENERATED files.
    CMP0070  #: Define file(GENERATE) behavior for relative paths.
    ##----- Policies Introduced by CMake 3.9
    CMP0069  #: INTERPROCEDURAL_OPTIMIZATION is enforced when enabled.
    CMP0068  #: RPATH settings on macOS do not affect install_name.
    ##----- Policies Introduced by CMake 3.8
    CMP0067  #: Honor language standard in try_compile() source-file signature.
    ##----- Policies Introduced by CMake 3.7
    CMP0066  #: Honor per-config flags in try_compile() source-file signature.
    ##----- Policies Introduced by CMake 3.4
    CMP0065  #: Do not add flags to export symbols from executables without the ENABLE_EXPORTS target property.
    CMP0064  #: Support new TEST if() operator.
    ##----- Policies Introduced by CMake 3.3
    CMP0063  #: Honor visibility properties for all target types.
    CMP0062  #: Disallow install() of export() result.
    CMP0061  #: CTest does not by default tell make to ignore errors (-i).
    CMP0060  #: Link libraries by full path even in implicit directories.
    CMP0059  #: Do not treat DEFINITIONS as a built-in directory property.
    CMP0058  #: Ninja requires custom command byproducts to be explicit.
    CMP0057  #: Support new IN_LIST if() operator.
    ##----- Policies Introduced by CMake 3.2
    CMP0056  #: Honor link flags in try_compile() source-file signature.
    CMP0055  #: Strict checking for break() command.
    ##----- Policies Introduced by CMake 3.1
    CMP0054  #: Only interpret if() arguments as variables or keywords when unquoted.
    CMP0053  #: Simplify variable reference and escape sequence evaluation.
    CMP0052  #: Reject source and build dirs in installed INTERFACE_INCLUDE_DIRECTORIES.
    CMP0051  #: List TARGET_OBJECTS in SOURCES target property.
    ##----- Policies Introduced by CMake 3.0
    CMP0050  #: Disallow add_custom_command SOURCE signatures.
    CMP0049  #: Do not expand variables in target source entries.
    CMP0048  #: project() command manages VERSION variables.
    CMP0047  #: Use QCC compiler id for the qcc drivers on QNX.
    CMP0046  #: Error on non-existent dependency in add_dependencies.
    CMP0045  #: Error on non-existent target in get_target_property.
    CMP0044  #: Case sensitive Lang_COMPILER_ID generator expressions.
    CMP0043  #: Ignore COMPILE_DEFINITIONS_Config properties.
    CMP0042  #: MACOSX_RPATH is enabled by default.
    CMP0041  #: Error on relative include with generator expression.
    CMP0040  #: The target in the TARGET signature of add_custom_command() must exist.
    CMP0039  #: Utility targets may not have link dependencies.
    CMP0038  #: Targets may not link directly to themselves.
    CMP0037  #: Target names should not be reserved and should match a validity pattern.
    CMP0036  #: The build_name command should not be called.
    CMP0035  #: The variable_requires command should not be called.
    CMP0034  #: The utility_source command should not be called.
    CMP0033  #: The export_library_dependencies command should not be called.
    CMP0032  #: The output_required_files command should not be called.
    CMP0031  #: The load_command command should not be called.
    CMP0030  #: The use_mangled_mesa command should not be called.
    CMP0029  #: The subdir_depends command should not be called.
    CMP0028  #: Double colon in target name means ALIAS or IMPORTED target.
    CMP0027  #: Conditionally linked imported targets with missing include directories.
    CMP0026  #: Disallow use of the LOCATION target property.
    CMP0025  #: Compiler id for Apple Clang is now AppleClang.
    CMP0024  #: Disallow include export result.
    )
  if(POLICY ${p})
    cmake_policy(SET ${p} NEW)
  endif()
endforeach()
project(vxl_pic_compatible)

add_library(cmTryCompileStaticLib STATIC static_src.cxx)
add_library(cmTryCompileSharedLib SHARED shared_src.cxx)
target_link_libraries(cmTryCompileSharedLib cmTryCompileStaticLib ${LINK_LIBRARIES})
