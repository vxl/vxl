# executables for segmentation processing
# brl/bseg/sbin/CMakeLists.txt
INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )
INCLUDE( ${vxl_SOURCE_DIR}/contrib/brl/bmods/find_xerces.cmake )

IF( HAS_VGUI )
 INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
 INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
 INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )

 IF(HAS_XERCES)
  ADD_DEFINITIONS(-DHAS_XERCES)
 ENDIF(HAS_XERCES)

 ADD_EXECUTABLE (seg seg.cxx)
 #                   segv_menus.cxx segv_menus.h

 TARGET_LINK_LIBRARIES(seg segv vgui)

ENDIF( HAS_VGUI )
