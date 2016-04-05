project(wx_example)

find_package(wxWidgets COMPONENTS mono) #base core gl xml xrc)
if(wxWidgets_FOUND)

  include( ${wxWidgets_USE_FILE} )
  #include_directories(${wxWidgets_INCLUDE_DIRS})

  # Simple example not using the Document/View architecture.
  set(bwm_wx_example_sources
    bwm_wx_app.h     bwm_wx_app.cxx
    bwm_wx_frame.h   bwm_wx_frame.cxx
    bwm_wx_menu.h
  )

  vxl_configure_file_copyonly(${CMAKE_CURRENT_SOURCE_DIR}/crosscut.xrc
                             ${CMAKE_CURRENT_BINARY_DIR}/crosscut.xrc
                             include/vxl/contrib/brl/bbas/bwm/wxWidgets )
  WXWIDGETS_ADD_RESOURCES(bwm_wx_example_sources crosscut.xrc)
  add_executable(bwm_wx_example WIN32 ${bwm_wx_example_sources} )
  target_link_libraries(bwm_wx_example bwm ${VXL_LIB_PREFIX}vgui ${VXL_LIB_PREFIX}vgui_wx ${wxWidgets_LIBRARIES})

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

endif()
