# oxl/xcv/CMakeLists.txt

INCLUDE( ${vxl_SOURCE_DIR}/core/vgui/UseVGUI.cmake )

IF (HAS_VGUI)

SET(xcv_sources
 xcv_image_tableau.cxx      xcv_image_tableau.h  xcv_image_tableau_sptr.h
 xcv_file.cxx               xcv_file.h
 xcv_display.cxx            xcv_display.h
 xcv_processing.cxx         xcv_processing.h
 xcv_geometry.cxx           xcv_geometry.h
 xcv_segmentation.cxx       xcv_segmentation.h
 xcv_multiview.cxx          xcv_multiview.h
                            xcv_mview_manager.h
 xcv_twoview_manager.cxx    xcv_twoview_manager.h
 xcv_threeview_manager.cxx  xcv_threeview_manager.h
 xcv_tjunction.cxx          xcv_tjunction.h

 xcv.cxx

 xcv_multiview_create_multiview_menu.cxx
 xcv_picker_tableau.cxx     xcv_picker_tableau.h xcv_picker_tableau_sptr.h
 xcv_axes_tableau.cxx       xcv_axes_tableau.h   xcv_axes_tableau_sptr.h
 xcv_axes_limits.cxx        xcv_axes_limits.h
)

AUX_SOURCE_DIRECTORY(Templates xcv_sources)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(xcv_geometry.cxx COMPILE_FLAGS "-O0 -ftemplate-depth-100")
  SET_SOURCE_FILES_PROPERTIES(xcv_multiview.cxx COMPILE_FLAGS -O0)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_EXECUTABLE(xcv ${xcv_sources})
TARGET_LINK_LIBRARIES(xcv osl mvl vgui vgl vil1 vnl_algo vnl vul vbl vpl vcl)

ENDIF (HAS_VGUI)
