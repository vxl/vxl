# vxl/config/cmake/Modules/UseVXL.cmake
# (also copied by CMake to the top-level of the vxl build tree)
#
# This CMake file may be included by projects outside VXL.  It
# configures them to make use of VXL headers and libraries.  The file
# is written to work in one of two ways.
#
# The preferred way to use VXL from an outside project with UseVXL.cmake:
#
#  find_package(VXL)
#  if(VXL_FOUND)
#    include(${VXL_CMAKE_DIR}/UseVXL.cmake)
#  else(VXL_FOUND)
#    message("VXL_DIR should be set to the VXL build directory.")
#  endif(VXL_FOUND)
#
# Read vxl/config/cmake/VXLConfig.cmake for the list of variables
# provided.  The names have changed to reduce namespace pollution.
# The old names can be made available by placing this line before
# including UseVXL.cmake:
#
#  set(VXL_PROVIDE_OLD_CACHE_NAMES 1)
#
# This UseVXL.cmake no longer adds options and testing features automatically
# to projects including it unless this line appears before including it:
#
#  set(VXL_PROVIDE_STANDARD_OPTIONS 1)
#
# For example, in order to enable full backward-compatibility while
# still using FIND_PACKAGE, use these lines:
#
#  find_package(VXL)
#  if(VXL_FOUND)
#    set(VXL_PROVIDE_OLD_CACHE_NAMES 1)
#    set(VXL_PROVIDE_STANDARD_OPTIONS 1)
#    include(${VXL_CMAKE_DIR}/UseVXL.cmake)
#  else(VXL_FOUND)
#    message("VXL_DIR should be set to the VXL build directory.")
#  endif(VXL_FOUND)
#
# The old way to use VXL from an outside project with UseVXL.cmake is
# also supported for backward-compatibility:
#
#  set(VXL_BINARY_PATH "" CACHE PATH "VXL build directory (location of UseVXL.cmake)")
#  if(VXL_BINARY_PATH)
#    include(${VXL_BINARY_PATH}/UseVXL.cmake)
#  else(VXL_BINARY_PATH)
#    message("VXL_BINARY_PATH should be set to the VXL build directory (location of UseVXL.cmake)" )
#  endif(VXL_BINARY_PATH)
#

# If this file has been included directly by a user project instead of
# through VXL_USE_FILE from VXLConfig.cmake, simulate old behavior.
if(NOT VXL_CONFIG_CMAKE)
  if(VXL_BINARY_PATH)

    # Let FIND_PACKAGE import the VXLConfig.cmake module.
    set(VXL_DIR ${VXL_BINARY_PATH})
    find_package(VXL)

    # Enable compatibility mode.
    set(VXL_PROVIDE_OLD_CACHE_NAMES 1)
    set(VXL_PROVIDE_STANDARD_OPTIONS 1)

  endif(VXL_BINARY_PATH)
endif(NOT VXL_CONFIG_CMAKE)

# VXLConfig.cmake has now been included.  Use its settings.
if(VXL_CONFIG_CMAKE)
  # Load the compiler settings used for VXL.
  if(VXL_BUILD_SETTINGS_FILE)
    option( VXL_IMPORT_BUILD_SETTINGS "Import build settings (compiler flags, generator) from VXL?" YES )
    mark_as_advanced( VXL_IMPORT_BUILD_SETTINGS )
    if( VXL_IMPORT_BUILD_SETTINGS )
      include(${CMAKE_ROOT}/Modules/CMakeImportBuildSettings.cmake)
      CMAKE_IMPORT_BUILD_SETTINGS(${VXL_BUILD_SETTINGS_FILE})
    endif( VXL_IMPORT_BUILD_SETTINGS )
  endif(VXL_BUILD_SETTINGS_FILE)

  # Use the standard VXL include directories.
  include_directories(${VXL_VCL_INCLUDE_DIR} ${VXL_CORE_INCLUDE_DIR})

  # Add link directories needed to use VXL.
  link_directories(${VXL_LIBRARY_DIR})

  # Provide backwards compatibility if it is requested.
  if(VXL_PROVIDE_OLD_CACHE_NAMES)
    # Translate include directory variables back to old names.
    set(VTHREEP_INCLUDE_DIR ${VXL_V3P_INCLUDE_DIR_deprecated})
    set(VCL_INCLUDE_DIR ${VXL_VCL_INCLUDE_DIR})
    set(VXLCORE_INCLUDE_DIR ${VXL_CORE_INCLUDE_DIR})
    set(BRL_INCLUDE_DIR ${VXL_BRL_INCLUDE_DIR})
    set(GEL_INCLUDE_DIR ${VXL_GEL_INCLUDE_DIR})
    set(MUL_INCLUDE_DIR ${VXL_MUL_INCLUDE_DIR})
    set(OUL_INCLUDE_DIR ${VXL_OUL_INCLUDE_DIR})
    set(OXL_INCLUDE_DIR ${VXL_OXL_INCLUDE_DIR})
    set(RPL_INCLUDE_DIR ${VXL_RPL_INCLUDE_DIR})
    set(TBL_INCLUDE_DIR ${VXL_TBL_INCLUDE_DIR})
    set(PYTHON_INCLUDE_PATH ${VXL_PYTHON_INCLUDE_PATH})
    set(CONVERSIONS_INCLUDE_DIR ${VXL_CONVERSIONS_INCLUDE_DIR})

    set(BUILD_VGUI ${VXL_VGUI_FOUND})
    set(BUILD_BRL ${VXL_BRL_FOUND})
    set(BUILD_BGUI3D ${VXL_BGUI3D_FOUND})
    set(COIN3D_FOUND ${VXL_COIN3D_FOUND})
    set(PYTHON_FOUND ${VXL_PYTHON_FOUND})
    set(BUILD_OUL ${VXL_OUL_FOUND})
    set(BUILD_CONTRIB ${VXL_CONTRIB_FOUND})
    set(BUILD_TARGETJR ${VXL_TARGETJR_FOUND})
    # These were excluded by LOAD_CACHE in old UseVXL.cmake.
    # set(BUILD_CONVERSIONS ${VXL_CONVERSIONS_FOUND})
    # set(BUILD_GEL ${VXL_GEL_FOUND})
    # set(BUILD_MUL ${VXL_MUL_FOUND})
    # set(BUILD_OXL ${VXL_OXL_FOUND})
    # set(BUILD_RPL ${VXL_RPL_FOUND})
    # set(BUILD_TBL ${VXL_TBL_FOUND})

    set(VGUI_USE_GLUT ${VXL_VGUI_USE_GLUT_deprecated})
    set(VGUI_USE_QT ${VXL_VGUI_USE_QT_deprecated})
    set(VGUI_USE_MFC ${VXL_VGUI_USE_MFC_deprecated})
    set(VGUI_USE_GTK ${VXL_VGUI_USE_GTK_deprecated})
    set(VGUI_USE_GTK2 ${VXL_VGUI_USE_GTK2_deprecated})

    set(VXL_FORCE_V3P_ZLIB ${VXL_FORCE_V3P_ZLIB_deprecated})
    set(VXL_FORCE_V3P_JPEG ${VXL_FORCE_V3P_JPEG_deprecated})
    set(VXL_FORCE_V3P_TIFF ${VXL_FORCE_V3P_TIFF_deprecated})
    set(VXL_FORCE_V3P_PNG ${VXL_FORCE_V3P_PNG_deprecated})
    set(VXL_FORCE_V3P_MPEG2 ${VXL_FORCE_V3P_MPEG2_deprecated})

    set(MODULE_PATH ${VXL_CMAKE_DIR})
    set(VXL_LIBRARY_PATH ${VXL_LIBRARY_DIR})
  endif(VXL_PROVIDE_OLD_CACHE_NAMES)

  if(VXL_CMAKE_DOXYGEN_DIR)
    # Allow use of VXL's cmake/doxygen framework
    include(${VXL_CMAKE_DOXYGEN_DIR}/doxygen.cmake)
  endif(VXL_CMAKE_DOXYGEN_DIR)

  if(VXL_PROVIDE_STANDARD_OPTIONS)
    # Provide the standard set of VXL CMake options to the project.
    include(${VXL_CMAKE_DIR}/VXLStandardOptions.cmake)
  endif(VXL_PROVIDE_STANDARD_OPTIONS)
endif(VXL_CONFIG_CMAKE)
