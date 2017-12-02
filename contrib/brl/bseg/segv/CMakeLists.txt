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

 vxl_add_library(LIBRARY_NAME segv LIBRARY_SOURCES ${segv_sources})

 target_link_libraries(segv bgui ${VXL_LIB_PREFIX}vgui sdet bsol bvgl brip ${VXL_LIB_PREFIX}vpgl_file_formats ${VXL_LIB_PREFIX}vpgl vtol vdgl vsol ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl)

 if(BUILD_TESTING)
  add_subdirectory(tests)
 endif()

endif()
