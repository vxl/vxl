#executables for segmentation processing
# brl/bseg/sbin/

INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )
INCLUDE( ${vxl_SOURCE_DIR}/contrib/brl/bmods/find_xerces.cmake )

IF( HAS_VGUI )

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )

#SOURCE_FILES(sbin_sources segv_menus.cxx segv_menus.h)

IF(HAS_XERCES)
 ADD_DEFINITIONS(-DHAS_XERCES)
ENDIF(HAS_XERCES)

ADD_EXECUTABLE (seg seg.cxx)

TARGET_LINK_LIBRARIES(seg segv bgui vgui)

ENDIF( HAS_VGUI )

