# segmentation view library
# brl/bseg/segv/CMakeLists.txt

doxygen_add_library(contrib/brl/bseg/segv
  DEPENDS
    contrib/brl/bbas/bgui
    contrib/brl/bbas/bxml
  PACKAGE contrib/brl
  )

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR}/mrc)
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR}/mrc/vpgl)
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindECW.cmake )
IF(ECW_FOUND)
  INCLUDE_DIRECTORIES(${ECW_INCLUDE_DIR})
ENDIF(ECW_FOUND)
# determines if vgui is built and opengl is available
INCLUDE( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

IF(HAS_VGUI)

 SET(segv_sources
     dll.h vil_dll.h
     segv_vil_segmentation_manager.cxx segv_vil_segmentation_manager.h
     segv_vil_menus.cxx                segv_vil_menus.h
     segv_vtol2D_tableau.cxx           segv_vtol2D_tableau.h   segv_vtol2D_tableau_sptr.h
     segv_vtol_soview2D.cxx            segv_vtol_soview2D.h
     segv_misc_menus.cxx               segv_misc_menus.h
     segv_misc_manager.cxx segv_misc_manager.h
    )

 ADD_LIBRARY(segv ${segv_sources})
 INSTALL_TARGETS(/lib segv)
 INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/segv ${segv_sources})

 TARGET_LINK_LIBRARIES(segv bgui vgui sdet bsol bvgl brip vpgl_file_formats vpgl vtol vdgl vsol vil vil_algo vil1 vnl vgl vul vbl)

 IF (BUILD_TESTING)
  SUBDIRS(tests)
 ENDIF (BUILD_TESTING)

ENDIF(HAS_VGUI)
