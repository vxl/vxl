# vxl/config/cmake/UseVXL.cmake
# (also copied by CMake to the top-level of the vxl build tree)
#
# This CMake file may be included by projects outside VXL.  It
# configures them to make use of VXL headers and libraries.  The file
# is written to work in one of two ways.
#
# The preferred way to use VXL from an outside project with UseVXL.cmake:
#
#  FIND_PACKAGE(VXL)
#  IF(VXL_FOUND)
#    INCLUDE(${VXL_CMAKE_DIR}/UseVXL.cmake)
#  ELSE(VXL_FOUND)
#    MESSAGE("VXL_DIR should be set to the VXL build directory.")
#  ENDIF(VXL_FOUND)
#
# Read vxl/config/cmake/VXLConfig.cmake for the list of variables
# provided.  The names have changed to reduce namespace pollution.
# The old names can be made available by placing this line before
# including UseVXL.cmake:
#
#  SET(VXL_PROVIDE_OLD_CACHE_NAMES 1)
#
# This UseVXL.cmake no longer adds options and testing features automatically
# to projects including it unless this line appears before including it:
#
#  SET(VXL_PROVIDE_STANDARD_OPTIONS 1)
#
# For example, in order to enable full backward-compatibility while
# still using FIND_PACKAGE, use these lines:
#
#  FIND_PACKAGE(VXL)
#  IF(VXL_FOUND)
#    SET(VXL_PROVIDE_OLD_CACHE_NAMES 1)
#    SET(VXL_PROVIDE_STANDARD_OPTIONS 1)
#    INCLUDE(${VXL_CMAKE_DIR}/UseVXL.cmake)
#  ELSE(VXL_FOUND)
#    MESSAGE("VXL_DIR should be set to the VXL build directory.")
#  ENDIF(VXL_FOUND)
#
# The old way to use VXL from an outside project with UseVXL.cmake is
# also supported for backward-compatibility:
#
#  SET(VXL_BINARY_PATH "" CACHE PATH "VXL build directory (location of UseVXL.cmake)")
#  IF(VXL_BINARY_PATH)
#    INCLUDE(${VXL_BINARY_PATH}/UseVXL.cmake)
#  ELSE(VXL_BINARY_PATH)
#    MESSAGE("VXL_BINARY_PATH should be set to the VXL build directory (location of UseVXL.cmake)" )
#  ENDIF(VXL_BINARY_PATH)
#

# If this file has been included directly by a user project instead of
# through VXL_USE_FILE from VXLConfig.cmake, simulate old behavior.
IF(NOT VXL_CONFIG_CMAKE)
  IF(VXL_BINARY_PATH)
    
    # Let FIND_PACKAGE import the VXLConfig.cmake module.
    SET(VXL_DIR ${VXL_BINARY_PATH})
    FIND_PACKAGE(VXL)
    
    # Enable compatibility mode.
    SET(VXL_PROVIDE_OLD_CACHE_NAMES 1)
    SET(VXL_PROVIDE_STANDARD_OPTIONS 1)
    
  ENDIF(VXL_BINARY_PATH)
ENDIF(NOT VXL_CONFIG_CMAKE)

# VXLConfig.cmake has now been included.  Use its settings.
IF(VXL_CONFIG_CMAKE)
  # Load the compiler settings used for VXL.
  IF(VXL_BUILD_SETTINGS_FILE)
    INCLUDE(${CMAKE_ROOT}/Modules/CMakeImportBuildSettings.cmake)
    CMAKE_IMPORT_BUILD_SETTINGS(${VXL_BUILD_SETTINGS_FILE})
  ENDIF(VXL_BUILD_SETTINGS_FILE)
  
  # Use the standard VXL include directories.
  INCLUDE_DIRECTORIES(${VXL_VCL_INCLUDE_DIR} ${VXL_CORE_INCLUDE_DIR})
  
  # Add link directories needed to use VXL.
  LINK_DIRECTORIES(${VXL_LIBRARY_DIR})
  
  # Provide backwards compatibility if it is requested.
  IF(VXL_PROVIDE_OLD_CACHE_NAMES)
    # Translate include directory variables back to old names.
    SET(VTHREEP_INCLUDE_DIR ${VXL_V3P_INCLUDE_DIR})
    SET(VCL_INCLUDE_DIR ${VXL_VCL_INCLUDE_DIR})
    SET(VXLCORE_INCLUDE_DIR ${VXL_CORE_INCLUDE_DIR})
    SET(BRL_INCLUDE_DIR ${VXL_BRL_INCLUDE_DIR})
    SET(GEL_INCLUDE_DIR ${VXL_GEL_INCLUDE_DIR})
    SET(MUL_INCLUDE_DIR ${VXL_MUL_INCLUDE_DIR})
    SET(OUL_INCLUDE_DIR ${VXL_OUL_INCLUDE_DIR})
    SET(OXL_INCLUDE_DIR ${VXL_OXL_INCLUDE_DIR})
    SET(RPL_INCLUDE_DIR ${VXL_RPL_INCLUDE_DIR})
    SET(TBL_INCLUDE_DIR ${VXL_TBL_INCLUDE_DIR})
    SET(CONVERSIONS_INCLUDE_DIR ${VXL_CONVERSIONS_INCLUDE_DIR})
    
    SET(BUILD_VCSL ${VXL_BUILD_VCSL})
    SET(BUILD_VGUI ${VXL_BUILD_VGUI})
    SET(BUILD_BRL ${VXL_BUILD_BRL})
    SET(BUILD_OUL ${VXL_BUILD_OUL})
    SET(BUILD_CONTRIB ${VXL_BUILD_CONTRIB})
    SET(BUILD_TARGETJR ${VXL_BUILD_TARGETJR})
    # These were excluded by LOAD_CACHE in old UseVXL.cmake.
    # SET(BUILD_CONVERSIONS ${VXL_BUILD_CONVERSIONS})
    # SET(BUILD_GEL ${VXL_BUILD_GEL})
    # SET(BUILD_MUL ${VXL_BUILD_MUL})
    # SET(BUILD_OXL ${VXL_BUILD_OXL})
    # SET(BUILD_RPL ${VXL_BUILD_RPL})
    # SET(BUILD_TBL ${VXL_BUILD_TBL})
    
    SET(VGUI_USE_GLUT ${VXL_VGUI_USE_GLUT})
    SET(VGUI_USE_QT ${VXL_VGUI_USE_QT})
    SET(VGUI_USE_MFC ${VXL_VGUI_USE_MFC})
    SET(VGUI_USE_GTK ${VXL_VGUI_USE_GTK})
    SET(VGUI_USE_GTK2 ${VXL_VGUI_USE_GTK2})
    
    SET(MODULE_PATH ${VXL_CMAKE_DIR}/Modules)
    SET(VXL_LIBRARY_PATH ${VXL_LIBRARY_DIR})
  ENDIF(VXL_PROVIDE_OLD_CACHE_NAMES)
  
  IF(VXL_PROVIDE_STANDARD_OPTIONS)
    # Provide the standard set of VXL CMake options to the project.
    INCLUDE(${VXL_CMAKE_DIR}/VXLStandardOptions.cmake)  
  ENDIF(VXL_PROVIDE_STANDARD_OPTIONS)
ENDIF(VXL_CONFIG_CMAKE)
