# segmentation view library
# brl/segv/CMakeLists.txt

INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

# determines if vgui is built and opengl is available
INCLUDE( ${VXLCORE_INCLUDE_DIR}/vgui/UseVGUI.cmake )
 
IF(HAS_VGUI)
# we have vgui enabled so ..

SET(segv_sources
 segv_segmentation_manager.cxx segv_segmentation_manager.h 
)

AUX_SOURCE_DIRECTORY(Templates segv_sources)

ADD_LIBRARY(segv ${segv_sources})

TARGET_LINK_LIBRARIES(segv sdet bxml gevd vdgl vtol vsol vil vnl vbl)

ENDIF(HAS_VGUI)

