# executables for segmentation processing
# brl/bseg/sbin/CMakeLists.txt
INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )
INCLUDE( ${vxl_SOURCE_DIR}/contrib/brl/bmods/find_xerces.cmake )
INCLUDE(${MODULE_PATH}/FindTIFF.cmake)

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )

IF( HAS_VGUI )
 IF(HAS_XERCES)
  ADD_DEFINITIONS(-DHAS_XERCES)
 ENDIF(HAS_XERCES)

 ADD_EXECUTABLE (seg seg.cxx)
 TARGET_LINK_LIBRARIES(seg segv vgui)

 ADD_EXECUTABLE (seg_vil seg_vil.cxx)
 TARGET_LINK_LIBRARIES(seg_vil segv vgui)

 ADD_EXECUTABLE (seg_misc seg_misc.cxx)
 TARGET_LINK_LIBRARIES(seg_misc segv vgui)
ENDIF( HAS_VGUI )

ADD_EXECUTABLE (negate negate.cxx)
TARGET_LINK_LIBRARIES(negate vbl vul vnl vil vcl)

ADD_EXECUTABLE (vil_convert vil_convert.cxx)
TARGET_LINK_LIBRARIES(vil_convert vbl vul vnl vil vcl)

ADD_EXECUTABLE (generate_tiff_pyramids generate_tiff_pyramids.cxx)
TARGET_LINK_LIBRARIES(generate_tiff_pyramids vpl vul vbl vnl vil)

ADD_EXECUTABLE (generate_tiff_rset generate_tiff_rset.cxx)
TARGET_LINK_LIBRARIES(generate_tiff_rset vpl vbl vul vil)
