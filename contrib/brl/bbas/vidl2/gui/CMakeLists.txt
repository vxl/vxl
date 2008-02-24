# contrib/brl/bbas/vidl2/gui/CMakeLists.txt
# This library adds vgui features to vidl2

INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )

IF( HAS_VGUI )

SET(vidl2_gui_sources
    vidl2_gui_param_dialog.cxx      vidl2_gui_param_dialog.h
)

ADD_LIBRARY(vidl2_gui ${vidl2_gui_sources})
INSTALL_TARGETS(/lib vidl2_gui)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/vidl2/gui ${vidl2_gui_sources})
TARGET_LINK_LIBRARIES( vidl2_gui vidl2 vgui )


ENDIF( HAS_VGUI )
