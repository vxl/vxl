# ./oxl/apps/vxl_vrml_viewer/CMakeLists.txt

INCLUDE( ${allvxl_SOURCE_DIR}/oxl/vgui/UseVGUI.cmake )

INCLUDE(${MODULE_PATH}/FindQv.cmake)

IF(HAS_VGUI)
IF(HAS_QV)

INCLUDE_DIRECTORIES( ${QV_INCLUDE_PATH} )

SET( vxl_vrml_viewer_sources main.cxx )

ADD_EXECUTABLE( vxl_vrml_viewer ${vxl_vrml_viewer_sources} )
TARGET_LINK_LIBRARIES( vxl_vrml_viewer osl mvl vgui vrml vgl vil vnl vul vbl vpl ${QV_LIBRARIES} vcl )

ENDIF(HAS_QV)
ENDIF(HAS_VGUI)
