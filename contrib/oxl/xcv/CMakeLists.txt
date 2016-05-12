# oxl/xcv/CMakeLists.txt

include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

if(HAS_VGUI)

set(xcv_sources
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

if(CMAKE_COMPILER_IS_GNUCXX)
  set_source_files_properties(xcv_geometry.cxx PROPERTIES COMPILE_FLAGS "-O0 -ftemplate-depth-100")
  set_source_files_properties(xcv_multiview.cxx PROPERTIES COMPILE_FLAGS -O0)
endif()

add_executable(xcv ${xcv_sources})
install_targets(/bin xcv)
target_link_libraries(xcv osl mvl ${VXL_LIB_PREFIX}vgui ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vcl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

endif()
