# basic vgui add-ons
# brl/bbas/bgui/CMakeLists.txt

doxygen_add_library(contrib/brl/bbas/bgui
  DEPENDS contrib/brl/bbas/btol core/vgui
  PACKAGE contrib/brl
  DESCRIPTION "Brown VGUI Augmentation"
 )

include_directories(${BRL_INCLUDE_DIR} )
include_directories(${BRL_INCLUDE_DIR}/bbas )
include_directories(${GEL_INCLUDE_DIR} )

# determines if vgui is built and opengl is available
include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

if(HAS_VGUI)
 set(bgui_sources
     bgui_vsol_soview2D.cxx            bgui_vsol_soview2D.h
     bgui_vsol2D_tableau.cxx           bgui_vsol2D_tableau.h          bgui_vsol2D_tableau_sptr.h
     bgui_vtol2D_tableau.cxx           bgui_vtol2D_tableau.h          bgui_vtol2D_tableau_sptr.h
     bgui_vtol2D_rubberband_client.cxx bgui_vtol2D_rubberband_client.h
     bgui_vtol_soview2D.cxx            bgui_vtol_soview2D.h
     bgui_vsol_camera_tableau.cxx      bgui_vsol_camera_tableau.h     bgui_vsol_camera_tableau_sptr.h
     bgui_picker_tableau.cxx           bgui_picker_tableau.h          bgui_picker_tableau_sptr.h
     bgui_image_tableau.cxx            bgui_image_tableau.h           bgui_image_tableau_sptr.h
     bgui_histogram_tableau.cxx        bgui_histogram_tableau.h       bgui_histogram_tableau_sptr.h
     bgui_bargraph_clipon_tableau.cxx  bgui_bargraph_clipon_tableau.h bgui_bargraph_clipon_tableau_sptr.h
     bgui_range_adjuster_tableau.cxx   bgui_range_adjuster_tableau.h  bgui_range_adjuster_tableau_sptr.h
     bgui_graph_tableau.cxx            bgui_graph_tableau.h           bgui_graph_tableau_sptr.h
     bgui_image_utils.cxx              bgui_image_utils.h
    )

 aux_source_directory(Templates bgui_sources)
 vxl_add_library(LIBRARY_NAME bgui LIBRARY_SOURCES ${bgui_sources})

 target_link_libraries(bgui bsta vdgl vtol vsol ${VXL_LIB_PREFIX}vgui ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vcl)

 if(VXL_BUILD_EXAMPLES)
   add_subdirectory(examples)
 endif()

 if(BUILD_TESTING)
   add_subdirectory(tests)
 endif()

endif()
