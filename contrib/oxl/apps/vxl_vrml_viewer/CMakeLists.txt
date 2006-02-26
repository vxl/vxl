# ./oxl/apps/vxl_vrml_viewer/CMakeLists.txt

INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )

INCLUDE(${MODULE_PATH}/FindQv.cmake)

IF(HAS_VGUI)
  IF(QV_FOUND)

    INCLUDE_DIRECTORIES( ${QV_INCLUDE_DIR} )

    SET( vxl_vrml_viewer_sources main.cxx )

    ADD_EXECUTABLE( vxl_vrml_viewer ${vxl_vrml_viewer_sources} )
    INSTALL_TARGETS( /bin vxl_vrml_viewer)
    TARGET_LINK_LIBRARIES( vxl_vrml_viewer vgui vrml ${QV_LIBRARIES} vcl )

  ENDIF(QV_FOUND)
ENDIF(HAS_VGUI)
