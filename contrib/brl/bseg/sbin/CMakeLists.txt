# executables for segmentation processing
# brl/bseg/sbin/CMakeLists.txt
INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )
INCLUDE( ${vxl_SOURCE_DIR}/contrib/brl/bmods/find_xerces.cmake )
INCLUDE(${MODULE_PATH}/FindTIFF.cmake)

IF( HAS_VGUI )
 INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
 INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
 INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )

 IF(HAS_XERCES)
  ADD_DEFINITIONS(-DHAS_XERCES)
 ENDIF(HAS_XERCES)

SET(seg_sources seg.cxx )	
ADD_EXECUTABLE (seg ${seg_sources})
TARGET_LINK_LIBRARIES(seg segv vgui)


ENDIF( HAS_VGUI )

SET(negate_sources negate.cxx)
ADD_EXECUTABLE (negate ${negate_sources})
TARGET_LINK_LIBRARIES(negate brip)

SET(convert_sources vil_convert.cxx)
ADD_EXECUTABLE (vil_convert ${convert_sources})
TARGET_LINK_LIBRARIES(vil_convert brip)

ADD_EXECUTABLE (generate_tiff_pyramids generate_tiff_pyramids.cxx)
TARGET_LINK_LIBRARIES(generate_tiff_pyramids brip vpl vil)
SET(seg_vil_sources seg_vil.cxx )	
ADD_EXECUTABLE (seg_vil ${seg_vil_sources})
TARGET_LINK_LIBRARIES(seg_vil segv vgui)


