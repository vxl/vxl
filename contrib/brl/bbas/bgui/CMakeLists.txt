# basic vgui add-ons
# brl/bbas/bgui/CMakeLists.txt
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

# determines if vgui is built and opengl is available
INCLUDE( ${VXLCORE_INCLUDE_DIR}/vgui/UseVGUI.cmake )

IF(HAS_VGUI)                                       
SET(bgui_sources
bgui_vtol2D_tableau.cxx     bgui_vtol2D_tableau.h  bgui_vtol2D_tableau_sptr.h
bgui_vtol_soview2D.cxx      bgui_vtol_soview2D.h
bgui_picker_tableau.cxx     bgui_picker_tableau.h  bgui_picker_tableau_sptr.h
)
AUX_SOURCE_DIRECTORY(Templates bgui_sources)

ADD_LIBRARY(bgui ${bgui_sources})

TARGET_LINK_LIBRARIES(bgui btol vdgl vtol vsol vgui vil vnl vbl)
ENDIF(HAS_VGUI)
