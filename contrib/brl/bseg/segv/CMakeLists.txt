# segmentation view library
# brl/segv/CMakeLists.txt

INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

# determines if vgui is built and opengl is available
INCLUDE( ${VXLCORE_INCLUDE_DIR}/vgui/UseVGUI.cmake )
INCLUDE( ${BRL_INCLUDE_DIR}/bmods/find_xerces.cmake )
 
IF(HAS_VGUI)
# we have vgui enabled so ..

SET(segv_sources
 segv_segmentation_manager.cxx segv_segmentation_manager.h 
 segv_menus.cxx                segv_menus.h
)

IF(HAS_XERCES)
 ADD_DEFINITIONS(-DHAS_XERCES)
ENDIF(HAS_XERCES)

AUX_SOURCE_DIRECTORY(Templates segv_sources)

ADD_LIBRARY(segv ${segv_sources})

IF(HAS_XERCES)
 TARGET_LINK_LIBRARIES(segv bxml)
ENDIF(HAS_XERCES)

TARGET_LINK_LIBRARIES(segv bgui sdet gevd vdgl vtol vsol vil vnl vbl)

ENDIF(HAS_VGUI)

