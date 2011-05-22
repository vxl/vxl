# core/vidl/gui/CMakeLists.txt
# This library adds vgui features to vidl

INCLUDE( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

IF( HAS_VGUI )

SET(vidl_gui_sources
    vidl_gui_param_dialog.cxx   vidl_gui_param_dialog.h

    vidl_capture_tableau.cxx    vidl_capture_tableau.h     vidl_capture_tableau_sptr.h
   )

ADD_LIBRARY(vidl_gui ${vidl_gui_sources})
INSTALL_TARGETS(/lib vidl_gui)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vidl/gui ${vidl_gui_sources})
TARGET_LINK_LIBRARIES( vidl_gui vidl vgui vil )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF( HAS_VGUI )
