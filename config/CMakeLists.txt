# This CMakeLists.txt handles the creation of files needed by other
# projects that need to use VXL.  Nothing is built by this
# CMakeLists.txt file.

INCLUDE( ${MODULE_PATH}/FindMPEG2.cmake )

# Save the compiler settings so another project can import them.
INCLUDE(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
SET(VXL_BUILD_SETTINGS_FILE ${vxl_BINARY_DIR}/VXLBuildSettings.cmake)
CMAKE_EXPORT_BUILD_SETTINGS(${VXL_BUILD_SETTINGS_FILE})

# Save library dependencies.
SET(VXL_LIBRARY_DEPENDS_FILE ${vxl_BINARY_DIR}/VXLLibraryDepends.cmake)
EXPORT_LIBRARY_DEPENDENCIES(${VXL_LIBRARY_DEPENDS_FILE})

# Create the VXLConfig.cmake file for the build tree.
CONFIGURE_FILE(${VXL_CMAKE_DIR}/VXLConfig.cmake.in
               ${vxl_BINARY_DIR}/VXLConfig.cmake @ONLY IMMEDIATE)
