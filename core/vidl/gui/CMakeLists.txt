# core/vidl/gui/CMakeLists.txt
# This library adds vgui features to vidl

include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

if( HAS_VGUI )

set(vidl_gui_sources
    vidl_gui_param_dialog.cxx   vidl_gui_param_dialog.h

    vidl_capture_tableau.cxx    vidl_capture_tableau.h     vidl_capture_tableau_sptr.h
   )

vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vidl_gui LIBRARY_SOURCES ${vidl_gui_sources})
target_link_libraries( ${VXL_LIB_PREFIX}vidl_gui ${VXL_LIB_PREFIX}vidl ${VXL_LIB_PREFIX}vgui ${VXL_LIB_PREFIX}vil )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

endif()
