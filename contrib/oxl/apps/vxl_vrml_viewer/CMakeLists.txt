# ./oxl/apps/vxl_vrml_viewer/CMakeLists.txt

INCLUDE( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

INCLUDE(${MODULE_PATH}/FindQv.cmake)

IF(HAS_VGUI)
  IF(QV_FOUND)

    INCLUDE_DIRECTORIES( ${QV_INCLUDE_DIR} )

    SET( vxl_vrml_viewer_sources main.cxx )

    ADD_EXECUTABLE( vxl_vrml_viewer ${vxl_vrml_viewer_sources} )
    TARGET_LINK_LIBRARIES( vxl_vrml_viewer osl mvl vgui vrml vgl vil1 vnl vul vbl vpl ${QV_LIBRARIES} vcl )

  ENDIF(QV_FOUND)
ENDIF(HAS_VGUI)
