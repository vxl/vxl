# segmentation view library
# brl/bseg/segv/CMakeLists.txt

doxygen_add_library(contrib/brl/bseg/segv
  DEPENDS
    contrib/brl/bbas/bgui
    contrib/brl/bbas/bxml
  PACKAGE contrib/brl
  )

include_directories(${BRL_INCLUDE_DIR})
include_directories(${BRL_INCLUDE_DIR}/bseg )
include_directories(${GEL_INCLUDE_DIR})
include_directories(${GEL_INCLUDE_DIR}/mrc)
include_directories(${GEL_INCLUDE_DIR}/mrc/vpgl)
include( ${VXL_CMAKE_DIR}/NewCMake/FindECW.cmake )
if(ECW_FOUND)
  include_directories(${ECW_INCLUDE_DIR})
endif()
# determines if vgui is built and opengl is available
include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

if(HAS_VGUI)

 set(segv_sources
     dll.h vil_dll.h
     segv_vil_segmentation_manager.cxx segv_vil_segmentation_manager.h
     segv_vil_menus.cxx                segv_vil_menus.h
     segv_vtol2D_tableau.cxx           segv_vtol2D_tableau.h   segv_vtol2D_tableau_sptr.h
     segv_vtol_soview2D.cxx            segv_vtol_soview2D.h
     segv_misc_menus.cxx               segv_misc_menus.h
     segv_misc_manager.cxx segv_misc_manager.h
    )

 add_library(segv ${segv_sources})
 install_targets(/lib segv)
 INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/segv ${segv_sources})

 target_link_libraries(segv bgui vgui sdet bsol bvgl brip vpgl_file_formats vpgl vtol vdgl vsol vil vil_algo vil1 vnl vgl vul vbl)

 if(BUILD_TESTING)
  add_subdirectory(tests)
 endif()

endif()
