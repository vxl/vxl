# core/vgui/CMakeLists.txt

project(vgui)

doxygen_add_library(core/vgui
  DEPENDS core/vgl core/vpl core/vbl core/vul core/vnl core/vil core/vil1
  PACKAGE core-L2
  DESCRIPTION "Multi-platform user-interface using OpenGL"
)


# Flag that determines if we were able to successfully build vgui.
# Initialize to NO. Change below if yes.
set( VGUI_FOUND "NO" CACHE INTERNAL "Was vgui successfully built?" )

if( VXL_BUILD_VGUI )

find_package(OpenGL)

# Build here only if OpenGL is available:
if(OPENGL_FOUND)
 if(OPENGL_GLU_FOUND)

  # For (not) reporting an error below
  set( CAN_VXL_BUILD_VGUI "YES" )

  # Apple can use OpenGL either with or without X11
  # Finding X11 can cause problems when the native
  # Apple OpenGL is to be used.
  if(APPLE)
    option(VGUI_USE_X11 "Use OpenGL for X11" NO)
    if(VGUI_USE_X11)
      find_package(X11)
    endif()
  else()
    find_package(X11)
  endif()
  # glut optionally used by vgui_text and by the glut toolkit
  # implementation
  find_package(GLUT)

  # The following are for the optional toolkit
  # implementations
  find_package(Qt)
  find_package(GTK)
  find_package(GTK2)
  include(${VXL_CMAKE_DIR}/NewCMake/FindWin32SDK.cmake)
  if(NOT MINGW)
    # MFC is not available in MinGW.
    # This should be fixed in FindMFC.cmake and then this IF can be removed
    find_package(MFC)
  endif()

  #set(WXWIDGETS_USE_LIBS base core gl)
  #find_package(wxWidgets)
  #include(${VXL_CMAKE_DIR}/NewCMake/FindwxWidgets.cmake)
  find_package(wxWidgets COMPONENTS base core gl)

  include_directories(${OPENGL_INCLUDE_DIR})
  if( GLUT_FOUND )
    include_directories(${GLUT_INCLUDE_DIR})
  endif()

set(vgui_sources
  vgui_fwd.h

  # basic classes
  vgui.cxx                      vgui.h
  vgui_toolkit.cxx              vgui_toolkit.h
  vgui_tag.cxx                  vgui_tag.h
  vgui_window.cxx               vgui_window.h
  vgui_adaptor.cxx              vgui_adaptor.h
  vgui_tableau.cxx              vgui_tableau.h    vgui_tableau_sptr.h
  vgui_parent_child_link.cxx    vgui_parent_child_link.h
  vgui_key.cxx                  vgui_key.h
  vgui_button.cxx               vgui_button.h
  vgui_modifier.cxx             vgui_modifier.h
  vgui_event.cxx                vgui_event.h
  vgui_command.cxx              vgui_command.h    vgui_command_sptr.h
  vgui_menu.cxx                 vgui_menu.h
  vgui_popup_params.h
  vgui_message.h
  vgui_observer.cxx             vgui_observer.h
  vgui_observable.cxx           vgui_observable.h
  vgui_satellite_tableau.hxx vgui_satellite_tableau.h
  vgui_font.cxx                 vgui_font.h       vgui_font_sptr.h
  vgui_font_bitmap.cxx          vgui_font_bitmap.h
  vgui_font_textured.cxx        vgui_font_textured.h

  # utility
  vgui_find.cxx                 vgui_find.h
  vgui_utils.cxx                vgui_utils.h
  vgui_macro.cxx                vgui_macro.h
  vgui_color.cxx                vgui_color.h
  vgui_projection_inspector.cxx vgui_projection_inspector.h
  vgui_matrix_state.cxx         vgui_matrix_state.h
  vgui_text_graph.cxx           vgui_text_graph.h
  vgui_camera.cxx               vgui_camera.h
  vgui_event_condition.cxx      vgui_event_condition.h

  # images
  internals/vgui_rasterpos.cxx  internals/vgui_rasterpos.h
  vgui_pixel.cxx                vgui_pixel.h
  vgui_section_render.cxx       vgui_section_render.h
  vgui_section_buffer.cxx       vgui_section_buffer.h
  vgui_range_map_params.h       vgui_range_map_params_sptr.h
  vgui_range_map_params.cxx
  vgui_range_map.hxx            vgui_range_map.h
  vgui_texture_hacks.cxx        vgui_texture_hacks.h
  vgui_cache_wizard.cxx         vgui_cache_wizard.h
  vgui_image_renderer.cxx       vgui_image_renderer.h
  vgui_vil_image_renderer.cxx   vgui_vil_image_renderer.h
  vgui_image_tableau.cxx        vgui_image_tableau.h      vgui_image_tableau_sptr.h
  vgui_blender_tableau.cxx      vgui_blender_tableau.h    vgui_blender_tableau_sptr.h

  # tableau mixins and miscellaneous
  vgui_wrapper_tableau.cxx      vgui_wrapper_tableau.h    vgui_wrapper_tableau_sptr.h
  vgui_deck_tableau.cxx         vgui_deck_tableau.h       vgui_deck_tableau_sptr.h
  vgui_composite_tableau.cxx    vgui_composite_tableau.h  vgui_composite_tableau_sptr.h
  vgui_poly_tableau.cxx         vgui_poly_tableau.h       vgui_poly_tableau_sptr.h
  vgui_grid_tableau.cxx         vgui_grid_tableau.h       vgui_grid_tableau_sptr.h
  vgui_drag_mixin.cxx           vgui_drag_mixin.h
  vgui_active_tableau.cxx       vgui_active_tableau.h     vgui_active_tableau_sptr.h
  vgui_selector_tableau.cxx     vgui_selector_tableau.h   vgui_selector_tableau_sptr.h
                                vgui_drag_tableau.h       vgui_drag_tableau_sptr.h
  vgui_clear_tableau.cxx        vgui_clear_tableau.h      vgui_clear_tableau_sptr.h
  vgui_debug_tableau.cxx        vgui_debug_tableau.h      vgui_debug_tableau_sptr.h
  vgui_quit_tableau.cxx         vgui_quit_tableau.h       vgui_quit_tableau_sptr.h
  vgui_tview_launcher_tableau.cxx vgui_tview_launcher_tableau.h vgui_tview_launcher_tableau_sptr.h
  vgui_shell_tableau.cxx        vgui_shell_tableau.h      vgui_shell_tableau_sptr.h
  vgui_text_put.cxx             vgui_text_put.h
  vgui_text_tableau.cxx         vgui_text_tableau.h       vgui_text_tableau_sptr.h
  vgui_blackbox_tableau.cxx     vgui_blackbox_tableau.h   vgui_blackbox_tableau_sptr.h
  vgui_enhance_tableau.cxx      vgui_enhance_tableau.h    vgui_enhance_tableau_sptr.h
  vgui_tview_tableau.cxx        vgui_tview_tableau.h      vgui_tview_tableau_sptr.h
  vgui_function_tableau.cxx     vgui_function_tableau.h   vgui_function_tableau_sptr.h
  vgui_roi_tableau.cxx          vgui_roi_tableau.h        vgui_roi_tableau_sptr.h
  vgui_rubberband_tableau.cxx   vgui_rubberband_tableau.h vgui_rubberband_tableau_sptr.h
  vgui_loader_tableau.cxx       vgui_loader_tableau.h     vgui_loader_tableau_sptr.h
  vgui_color_text.cxx           vgui_color_text.h
  vgui_event_server.cxx         vgui_event_server.h
  vgui_error_dialog.cxx         vgui_error_dialog.h

  vgui_soview.cxx               vgui_soview.h
  vgui_soview2D.cxx             vgui_soview2D.h
  vgui_soview3D.cxx             vgui_soview3D.h
  vgui_style.cxx                vgui_style.h                vgui_style_sptr.h
  vgui_displaybase_tableau.cxx  vgui_displaybase_tableau.h  vgui_displaybase_tableau_sptr.h
  vgui_displaylist2D_tableau.cxx  vgui_displaylist2D_tableau.h vgui_displaylist2D_tableau_sptr.h
  vgui_displaylist3D_tableau.cxx  vgui_displaylist3D_tableau.h vgui_displaylist3D_tableau_sptr.h
  vgui_easy2D_tableau.cxx       vgui_easy2D_tableau.h       vgui_easy2D_tableau_sptr.h
  vgui_easy3D_tableau.cxx       vgui_easy3D_tableau.h       vgui_easy3D_tableau_sptr.h
  vgui_listmanager2D_tableau.cxx  vgui_listmanager2D_tableau.h  vgui_listmanager2D_tableau_sptr.h
  vgui_viewer2D_tableau.cxx vgui_viewer2D_tableau.h         vgui_viewer2D_tableau_sptr.h
  vgui_viewer3D_tableau.cxx vgui_viewer3D_tableau.h         vgui_viewer3D_tableau_sptr.h
  internals/trackball.c              internals/trackball.h

  vgui_slider_tableau.cxx       vgui_slider_tableau.h       vgui_slider_tableau_sptr.h

  # dialog classes
  vgui_dialog.cxx                    vgui_dialog.h
  vgui_dialog_extensions.cxx         vgui_dialog_extensions.h
  internals/vgui_dialog_impl.cxx     internals/vgui_dialog_impl.h
  internals/vgui_dialog_extensions_impl.cxx     internals/vgui_dialog_extensions_impl.h
  internals/vgui_dialog_field.h
  internals/vgui_simple_field.h      internals/vgui_simple_field.cxx
  internals/vgui_string_field.h
  internals/vgui_file_field.h
  internals/vgui_button_field.h

  # statusbar classes
  vgui_statusbuf.cxx            vgui_statusbuf.h
  vgui_statusbar.h

  # extra header files
  dll.h
  vgui_gl.h
  vgui_glu.h
  vgui_glx.h

  # internals
  internals/vgui_invert_homg4x4.cxx  internals/vgui_invert_homg4x4.h
  internals/vgui_back_project.cxx    internals/vgui_back_project.h
  internals/vgui_un_project.cxx      internals/vgui_un_project.h
  internals/vgui_adaptor_tableau.cxx internals/vgui_adaptor_tableau.h
  internals/vgui_parent_child_link_data.h
  internals/vgui_adaptor_mixin.h
  internals/vgui_overlay_helper.cxx  internals/vgui_overlay_helper.h
  internals/vgui_multiply_4x4.cxx    internals/vgui_multiply_4x4.h
  internals/vgui_transpose_4x4.cxx   internals/vgui_transpose_4x4.h
  internals/vgui_draw_line.cxx       internals/vgui_draw_line.h
  # Mesa-X11-mfc accelerated functions using Hermes
  internals/vgui_accelerate.cxx      internals/vgui_accelerate.h
  internals/vgui_accelerate_tag.cxx
  internals/vgui_gl_selection_macros.h
)

if( GLUT_FOUND )
  set(vgui_sources ${vgui_sources}
    vgui_glut.cxx                    vgui_glut.h
  )
endif()

aux_source_directory(Templates vgui_sources)

if(MFC_FOUND)
  #set(vgui_sources ${vgui_sources}
  #    internals/vgui_accelerate_mfc.cxx internals/vgui_accelerate_mfc.h
  #   )
endif()

if(X11_FOUND)
  include_directories(${X11_INCLUDE_DIR})
  set(vgui_sources ${vgui_sources}
      internals/vgui_accelerate_x11.cxx internals/vgui_accelerate_x11.h
     )
endif()

if(MFC_FOUND)
  # add stuff to use MFC in this executable
  add_definitions( "-D_AFXDLL" )
  set(CMAKE_MFC_FLAG 2)
endif()


######################################################################
# TOOLKIT IMPLEMENTATIONS
######################################################################

##################################################
# glut
##################################################

# Build here only if GLUT is available:
if(GLUT_FOUND)

  option(VGUI_USE_GLUT "Should Glut support be compiled into vgui?" YES)

  if(VGUI_USE_GLUT)

    include_directories(${GLUT_INCLUDE_DIR})

    set(vgui_glut_sources
      impl/glut/vgui_glut_adaptor.cxx    impl/glut/vgui_glut_adaptor.h
      impl/glut/vgui_glut_tag.cxx
      impl/glut/vgui_glut_popup_impl.cxx impl/glut/vgui_glut_popup_impl.h
      impl/glut/vgui_glut_window.cxx     impl/glut/vgui_glut_window.h
      impl/glut/vgui_glut_impl.cxx       impl/glut/vgui_glut_impl.h

      impl/glut/menu_hack.h
    )
    if(X11_FOUND AND NOT APPLE)
      include_directories(${X11_INCLUDE_DIR})
     # FreeGLUT does not support this menu hack
      if(EXISTS ${GLUT_INCLUDE_DIR}/GL/freeglut.h)
        set(vgui_glut_sources ${vgui_glut_sources}
          impl/glut/menu_hack_none.cxx
        )
      else()
        set(vgui_glut_sources ${vgui_glut_sources}
          impl/glut/menu_hack_X11.cxx
        )
      endif()
    else()
      set(vgui_glut_sources ${vgui_glut_sources}
        impl/glut/menu_hack_none.cxx
      )
    endif()

    set( vgui_sources ${vgui_sources} ${vgui_glut_sources} )

  endif()

endif()

##################################################
# Qt
##################################################

if(QT_FOUND)

  option(VGUI_USE_QT "Should Qt support be compiled into vgui?" YES)
  if(QT3_INSTALLED AND QT4_INSTALLED)
    option(VGUI_USE_QT4 "Specify QT version" YES)
  else()
     if(QT4_INSTALLED)
          set(VGUI_USE_QT4 YES)
     else()
         set(VGUI_USE_QT4 NO)
     endif()
  endif()
  if(VGUI_USE_QT)
    if(QT4_INSTALLED AND VGUI_USE_QT4)
    include_directories(${QT_INCLUDE_DIR} ${QT_QT_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR}
                        ${QT_QTOPENGL_INCLUDE_DIR})
    add_definitions(-DQT3_SUPPORT_WARNINGS ${QT_DEFINITIONS} ${QT_USE_QTOPENGL})
    set(QT_SRCS_DIR impl/qt4)
    else()
    include_directories(${QT_INCLUDE_DIR})
    add_definitions(${QT_DEFINITIONS})
    set(QT_SRCS_DIR impl/qt)
    endif()

    set(vgui_qt_sources
      ${QT_SRCS_DIR}/vgui_qt.cxx             ${QT_SRCS_DIR}/vgui_qt.h
      ${QT_SRCS_DIR}/vgui_qt_tag.cxx
      ${QT_SRCS_DIR}/vgui_qt_window.cxx      ${QT_SRCS_DIR}/vgui_qt_window.h
      ${QT_SRCS_DIR}/vgui_qt_adaptor.cxx     ${QT_SRCS_DIR}/vgui_qt_adaptor.h
      ${QT_SRCS_DIR}/vgui_qt_statusbar.cxx   ${QT_SRCS_DIR}/vgui_qt_statusbar.h
      ${QT_SRCS_DIR}/vgui_qt_dialog_impl.cxx ${QT_SRCS_DIR}/vgui_qt_dialog_impl.h
      ${QT_SRCS_DIR}/vgui_qt_menu.cxx        ${QT_SRCS_DIR}/vgui_qt_menu.h
    )

    qt_wrap_cpp(vgui vgui_qt_moc_files
      ${QT_SRCS_DIR}/vgui_qt_adaptor.h
      ${QT_SRCS_DIR}/vgui_qt_dialog_impl.h
      ${QT_SRCS_DIR}/vgui_qt_menu.h
      ${QT_SRCS_DIR}/vgui_qt_statusbar.h
    )

    set( vgui_sources ${vgui_sources} ${vgui_qt_sources} ${vgui_qt_moc_files} )

    if(CMAKE_COMPILER_IS_GNUCXX)
      set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/moc_vgui_qt_adaptor.cxx GENERATED PROPERTIES COMPILE_FLAGS -O0)
    endif()

  endif()

endif()

##################################################
# Win32SDK
##################################################

if(WIN32SDK_FOUND)

  option(VGUI_USE_WIN32SDK "Should Win32SDK support be compiled into vgui?" YES)

  if(VGUI_USE_WIN32SDK)

    set(vgui_win32sdk_sources
      impl/win32/vgui_win32.cxx             impl/win32/vgui_win32.h
      impl/win32/vgui_win32_adaptor.cxx     impl/win32/vgui_win32_adaptor.h
      impl/win32/vgui_win32_cmdtarget.cxx   impl/win32/vgui_win32_cmdtarget.h
      impl/win32/vgui_win32_dialog_impl.cxx impl/win32/vgui_win32_dialog_impl.h
      impl/win32/vgui_win32_statusbar.cxx   impl/win32/vgui_win32_statusbar.h
      impl/win32/vgui_win32_tag.cxx
      impl/win32/vgui_win32_utils.h         impl/win32/vgui_win32_utils.cxx
      impl/win32/vgui_win32_window.cxx      impl/win32/vgui_win32_window.h
    )

    set( vgui_sources ${vgui_sources} ${vgui_win32sdk_sources} )

  endif()

endif()

##################################################
# MFC
##################################################

if(MFC_FOUND)

  option(VGUI_USE_MFC "Should MFC support be compiled into vgui?" YES)

  if(VGUI_USE_MFC)

    set(vgui_mfc_sources
      impl/mfc/vgui_mfc.cxx             impl/mfc/vgui_mfc.h
      impl/mfc/vgui_mfc_adaptor.cxx     impl/mfc/vgui_mfc_adaptor.h
      impl/mfc/vgui_mfc_app.cxx         impl/mfc/vgui_mfc_app.h
      impl/mfc/vgui_mfc_app_init.cxx    impl/mfc/vgui_mfc_app_init.h
      impl/mfc/vgui_mfc_dialog_impl.cxx impl/mfc/vgui_mfc_dialog_impl.h
      impl/mfc/vgui_mfc_dialog_extensions_impl.cxx impl/mfc/vgui_mfc_dialog_extensions_impl.h
      impl/mfc/vgui_mfc_doc.cxx         impl/mfc/vgui_mfc_doc.h
      impl/mfc/vgui_mfc_mainfrm.cxx     impl/mfc/vgui_mfc_mainfrm.h
      impl/mfc/vgui_mfc_statusbar.cxx   impl/mfc/vgui_mfc_statusbar.h
      impl/mfc/vgui_mfc_tag.cxx
      impl/mfc/vgui_mfc_utils.cxx       impl/mfc/vgui_mfc_utils.h
      impl/mfc/vgui_mfc_view.cxx        impl/mfc/vgui_mfc_view.h
      impl/mfc/vgui_mfc_window.cxx      impl/mfc/vgui_mfc_window.h
      impl/mfc/vgui_dir_dialog.cxx      impl/mfc/vgui_dir_dialog.h
    )

    set( vgui_sources ${vgui_sources} ${vgui_mfc_sources} )

  endif()

endif()

##################################################
# GTK
##################################################

if(GTK_GL_FOUND)

  option(VGUI_USE_GTK "Should GTK support be compiled into vgui?" YES)

  if(VGUI_USE_GTK)

    include_directories(${GTK_INCLUDE_DIR})

    set(vgui_gtk_sources
      impl/gtk/vgui_gtk.cxx             impl/gtk/vgui_gtk.h
      impl/gtk/vgui_gtk_adaptor.cxx     impl/gtk/vgui_gtk_adaptor.h
      impl/gtk/vgui_gtk_dialog_impl.cxx impl/gtk/vgui_gtk_dialog_impl.h
      impl/gtk/vgui_gtk_statusbar.cxx   impl/gtk/vgui_gtk_statusbar.h
      impl/gtk/vgui_gtk_tag.cxx
      impl/gtk/vgui_gtk_utils.cxx       impl/gtk/vgui_gtk_utils.h
      impl/gtk/vgui_gtk_window.cxx      impl/gtk/vgui_gtk_window.h
    )

    set( vgui_sources ${vgui_sources} ${vgui_gtk_sources} )

  endif()

endif()


##################################################
# GTK2
##################################################

if(GTK2_FOUND)
  # We need to make sure we have the gl extension before building this component.
  find_package(PkgConfig)
  if (PkgConfig_FOUND)
    pkg_search_module(GTKGL2 gtkglext-1.0) # despite the 1.0, works w/ gtk2
    if (GTKGL2_FOUND)
      list(APPEND GTK2_INCLUDE_DIRS ${GTKGL2_INCLUDE_DIRS})
      list(REMOVE_DUPLICATES GTK2_INCLUDE_DIRS)
      list(APPEND GTK2_LIBRARIES ${GTKGL2_LIBRARIES})
      list(REMOVE_DUPLICATES GTK2_LIBRARIES)
      # TODO: libraries are not full paths, a known limitation of pkg_*_module.
      # If a GTK library is installed on a non-standard path, this will fail.
      # One can either work on the cmake dev list to improve PkgConfig,
      # or else use a function to append a full path to each library as
      # provided by a patch in the original Mantis issue for 
      # https://gitlab.kitware.com/cmake/cmake/issues/15804
    endif()
  endif()
  # message("GTK2_INCLUDE_DIRS: ${GTK2_INCLUDE_DIRS}")
  # message("GTK2 FINAL Libraries" "${GTK2_LIBRARIES}")
  find_file(GTKGL gtk/gtkgl.h PATHS ${GTK2_INCLUDE_DIRS})
  if (GTKGL)
    option(VGUI_USE_GTK2 "Should GTK2 support be compiled into vgui?" YES)

    if(VGUI_USE_GTK2)

      if(VGUI_USE_GTK)
        message(SEND_ERROR "You cannot build against both GTK1 and"
          " GTK2 at the same time")
      endif()


      # This puts the include directories and whatever compiler flags
      # are necessary for GTK2
      add_definitions( ${GTK2_DEFINITIONS} )
      include_directories(${GTK2_INCLUDE_DIRS})

      set(vgui_gtk2_sources
        impl/gtk2/vgui_gtk2.cxx             impl/gtk2/vgui_gtk2.h
        impl/gtk2/vgui_gtk2_adaptor.cxx     impl/gtk2/vgui_gtk2_adaptor.h
        impl/gtk2/vgui_gtk2_dialog_impl.cxx impl/gtk2/vgui_gtk2_dialog_impl.h
        impl/gtk2/vgui_gtk2_statusbar.cxx   impl/gtk2/vgui_gtk2_statusbar.h
        impl/gtk2/vgui_gtk2_tag.cxx
        impl/gtk2/vgui_gtk2_utils.cxx       impl/gtk2/vgui_gtk2_utils.h
        impl/gtk2/vgui_gtk2_window.cxx      impl/gtk2/vgui_gtk2_window.h
        )

      set( vgui_sources ${vgui_sources} ${vgui_gtk2_sources} )

    endif()
  endif()

endif()

#########################################i#########
# wxWidgets
##################################################

if(wxWidgets_FOUND)

  option(VGUI_USE_WX "Should wxWidgets support be compiled into vgui?" NO)

  if(VGUI_USE_WX)

    include( ${wxWidgets_USE_FILE} )

    set(vgui_wx_sources
      impl/wx/vgui_wx_tag.cxx
      impl/wx/vgui_wx.cxx             impl/wx/vgui_wx.h
      impl/wx/vgui_wx_adaptor.cxx     impl/wx/vgui_wx_adaptor.h
      impl/wx/vgui_wx_window.cxx      impl/wx/vgui_wx_window.h
      impl/wx/vgui_wx_dialog_impl.cxx impl/wx/vgui_wx_dialog_impl.h
      impl/wx/vgui_wx_menu.cxx        impl/wx/vgui_wx_menu.h
      impl/wx/vgui_wx_statusbar.cxx   impl/wx/vgui_wx_statusbar.h
    )

    set( vgui_sources ${vgui_sources} ${vgui_wx_sources} )

    option(VXL_BUILD_VGUI_WX "Build extras for VGUI use in wxWidgets apps" NO)
    if(VXL_BUILD_VGUI_WX)
      add_subdirectory(wx)
    endif()

  endif()

endif()

######################################################################
# END TOOLKIT IMPLEMENTATIONS
######################################################################

# Store chosen configuration in to the config file. This allows files
# that depend on these configuration parameters to be automatically
# recompiled when the config changes.

vxl_configure_file( ${VXL_ROOT_SOURCE_DIR}/core/vgui/vgui_config.h.in
                    ${PROJECT_BINARY_DIR}/vgui_config.h include/vxl/core/vgui)

# Register each toolkit that was compiled in.
set( vgui_sources ${vgui_sources}
  vgui_register_all.cxx
)

vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vgui LIBRARY_SOURCES ${vgui_sources})
set( VGUI_FOUND "YES" CACHE INTERNAL "Was vgui successfully built?" )

# Dependencies on other vxl libraries
target_link_libraries(${VXL_LIB_PREFIX}vgui ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul)

#imagehlp.dll is needed for directory browsing (vgui_dir_dialog)
if(MFC_FOUND)
  target_link_libraries(${VXL_LIB_PREFIX}vgui imagehlp)
endif()

# Dependencies created by the toolkit implementations

if(VGUI_USE_QT)
  if(QT4_INSTALLED AND VGUI_USE_QT4)
    target_link_libraries(${VXL_LIB_PREFIX}vgui ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY})
  else()
    target_link_libraries(${VXL_LIB_PREFIX}vgui ${QT_LIBRARIES})
  endif()
endif()
if(VGUI_USE_GTK)
  target_link_libraries(${VXL_LIB_PREFIX}vgui ${GTK_LIBRARIES})
endif()
if(VGUI_USE_GTK2)
  target_link_libraries(${VXL_LIB_PREFIX}vgui ${GTK2_LIBRARIES})
endif()
if(VGUI_USE_WX)
  if(wxWidgets_LINK_DIRECTORIES)
    link_directories(${wxWidgets_LINK_DIRECTORIES})
  endif()

  if(wxWidgets_LIBRARIES)
    target_link_libraries(${VXL_LIB_PREFIX}vgui ${wxWidgets_LIBRARIES})
  endif()
endif()

# Basic vgui dependencies.
# These should appear after the above, because most of
# the toolkits depend on OpenGL.

if(VGUI_USE_GLUT)
  target_link_libraries(${VXL_LIB_PREFIX}vgui ${GLUT_LIBRARIES})
endif()

target_link_libraries(${VXL_LIB_PREFIX}vgui ${OPENGL_LIBRARIES})

if(X11_FOUND)
  target_link_libraries(${VXL_LIB_PREFIX}vgui ${X11_LIBRARIES})
endif()

if(VXL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

endif()
endif()

if( NOT CAN_VXL_BUILD_VGUI )
  # Dashboard builds will try to build vgui if possible,
  # so don't warn if we can't.
  message(SEND_ERROR "vgui could not be built.\n"
                     "Make sure the OpenGL libraries "
                     "(GL and GLU) are specified correctly,\n"
                     "or set VXL_BUILD_VGUI to OFF." )
endif()

endif()
