# basic vgui add-ons
# brl/bbas/bgui/CMakeLists.txt
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )

# determines if vgui is built and opengl is available
INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )

IF(HAS_VGUI)
 SET(bgui_sources
     bgui_vsol_soview2D.cxx            bgui_vsol_soview2D.h
     bgui_vsol2D_tableau.cxx           bgui_vsol2D_tableau.h      bgui_vsol2D_tableau_sptr.h
     bgui_vtol2D_tableau.cxx           bgui_vtol2D_tableau.h      bgui_vtol2D_tableau_sptr.h
     bgui_vtol2D_rubberband_client.cxx bgui_vtol2D_rubberband_client.h
     bgui_vtol_soview2D.cxx            bgui_vtol_soview2D.h
     bgui_vsol_camera_tableau.cxx      bgui_vsol_camera_tableau.h bgui_vsol_camera_tableau_sptr.h
     bgui_picker_tableau.cxx           bgui_picker_tableau.h      bgui_picker_tableau_sptr.h
     bgui_image_tableau.cxx            bgui_image_tableau.h       bgui_image_tableau_sptr.h
     bgui_roi_tableau_make_roi.h
     bgui_bmrf_soview2D.cxx            bgui_bmrf_soview2D.h
     bgui_histogram_tableau.cxx        bgui_histogram_tableau.h   bgui_histogram_tableau_sptr.h
     bgui_selector_tableau.cxx         bgui_selector_tableau.h    bgui_selector_tableau_sptr.h
 )
 AUX_SOURCE_DIRECTORY(Templates bgui_sources)

 ADD_LIBRARY(bgui ${bgui_sources})

 TARGET_LINK_LIBRARIES(bgui bmrf vdgl vtol vsol vgui vgl_algo vgl vil_algo vil vil1)

ENDIF(HAS_VGUI)
