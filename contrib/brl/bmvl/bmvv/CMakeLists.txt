# view library for multi-view gui
# brl/bmvl/bmvv/CMakeLists.txt
INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bmvl )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )

# determines if vgui is built and opengl is available
INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )

IF(HAS_VGUI)
 # sources that depend on xerces and vgui  being present
 SET(bmvv_sources
    #bmvv_multiview_manager.cxx     bmvv_multiview_manager.h
    #bmvv_tomography_manager.cxx    bmvv_tomography_manager.h
     bmvv_recon_manager.cxx         bmvv_recon_manager.h
     bmvv_cal_manager.cxx           bmvv_cal_manager.h
    #bmvv_menus.cxx                 bmvv_menus.h
    #bmvv_tomg_menus.cxx            bmvv_tomg_menus.h
     bmvv_recon_menus.cxx           bmvv_recon_menus.h
     bmvv_cal_menus.cxx             bmvv_cal_menus.h
 )

 ADD_LIBRARY(bmvv ${bmvv_sources})

 # find the path to the xerces libraries and includes
 INCLUDE( ${vxl_SOURCE_DIR}/contrib/brl/bmods/find_xerces.cmake )

 IF(HAS_XERCES)
  ADD_DEFINITIONS(-DHAS_XERCES)
  TARGET_LINK_LIBRARIES(bmvv bxml)
 ENDIF(HAS_XERCES)

 TARGET_LINK_LIBRARIES(bmvv brct sdet brip bdgl mvl vtol vdgl bgui vgui vsol vgl vnl vil vil1)

 # the main executable
 SUBDIRS(mvbin)

ENDIF(HAS_VGUI)
