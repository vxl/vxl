# view library for multi-view gui 
# brl/bmvl/bmvv/CMakeLists.txt

INCLUDE_DIRECTORIES( ${OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bmvl) 

# find the path to the xerces libraries and includes
INCLUDE( ${vxl_SOURCE_DIR}/contrib/brl/bmods/find_xerces.cmake )

# determines if vgui is built and opengl is available
INCLUDE( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

IF(HAS_VGUI)                                       
# sources that depend on xerces and vgui  being present
SET(bmvv_sources
bmvv_multiview_manager.cxx           bmvv_multiview_manager.h
bmvv_tomography_manager.cxx          bmvv_tomography_manager.h
bmvv_menus.cxx                       bmvv_menus.h
bmvv_tomg_menus.cxx                  bmvv_tomg_menus.h
)

AUX_SOURCE_DIRECTORY(Templates bmvv_sources)

ADD_LIBRARY(bmvv ${bmvv_sources})

IF(HAS_XERCES)
 ADD_DEFINITIONS(-DHAS_XERCES)
 TARGET_LINK_LIBRARIES(bmvv bxml)
ENDIF(HAS_XERCES)
TARGET_LINK_LIBRARIES(bmvv btom sdet brip btol bdgl vtol vdgl bgui vgui mvl vsol vul vbl vcl)

# the main executable
SUBDIRS(mvbin)

ENDIF(HAS_VGUI)



