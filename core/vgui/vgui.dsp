# Microsoft Developer Studio Project File - Name="vgui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(GLUT)/include" /I "$(VXLROOT)/v3p" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -D_AFXDLL -DHAS_GLUT -DHAS_QV == compiler defines
#  == override in output directory
# vgui  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vgui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vgui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vgui.mak" CFG="vgui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vgui - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vgui - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vgui - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(GLUT)/include" /I "$(VXLROOT)/v3p" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -D_AFXDLL -DHAS_GLUT -DHAS_QV /D "vgui_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "vgui - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /GZ /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(GLUT)/include" /I "$(VXLROOT)/v3p" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -D_AFXDLL -DHAS_GLUT -DHAS_QV /D "vgui_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ENDIF 

# Begin Target

# Name "vgui - Win32 Release"
# Name "vgui - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\Templates/vbl_array_2d+vgui_grid_tableau+-grid_data-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vgui_command-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vgui_tableau-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_algorithm+vgui_style~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_list+vgui_event-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_map+vgui_style~.vgui_soview~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vcl_pair+vil_image.vcl_vector+GLuint-~-~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vgui_dialog_impl+-element-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vgui_event-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vgui_image_tableau~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vgui_menu_item-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vgui_observer~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vgui_poly_tableau+-item-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vgui_rubberband_tableau_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vgui_parent_child_link-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vgui_soview2D_lineseg~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vgui_soview2D~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vgui_soview3D~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vgui_soview~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vgui_style~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vgui_tableau_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vgui_tableau~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vgui_toolkit~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vgui_tview_tableau+-icon-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_command_simple+vgui_active_tableau-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_command_simple+vgui_adaptor-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_command_simple+vgui_clear_tableau-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_command_simple+vgui_tableau-.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/trackball.c
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_accelerate.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_accelerate_mfc.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_accelerate_tag.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_adaptor_mixin.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_adaptor_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_back_project.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_dialog_field.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_dialog_impl.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_draw_line.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_file_field.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_invert_homg4x4.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_multiply_4x4.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_overlay_helper.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_rasterpos.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_simple_field.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_string_field.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_transpose_4x4.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_un_project.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_adaptor.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_button.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_cache_wizard.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_camera.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_clear_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_color.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_color_text.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_command.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_debug_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_deck_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_dialog.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_drag_mixin.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_drag_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_error_dialog.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_event.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_event_condition.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_event_server.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_find.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_function_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_glut.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_grid_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_image_renderer.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_image_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_key.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_macro.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_matrix_state.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_menu.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_message.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_modifier.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_observable.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_observer.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_pixel.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_popup_params.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_projection_inspector.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_quit_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_roi_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_section_buffer.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_section_buffer_of.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_section_render.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_shell_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_soview.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_soview2D.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_soview3D.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_statusbar.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_statusbuf.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_style.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_style_factory.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_tag.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_text_graph.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_text_put.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_text_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_texture_hacks.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_toolkit.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_utils.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_window.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_wrapper_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vrml/Templates/vcl_vector+vgui_vrml_tableau_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\vrml/Templates/vcl_vector+vgui_vrml_texture_map~-.cxx
# End Source File
# Begin Source File
SOURCE=.\vrml/vgui_vrml_draw_visitor.cxx
# End Source File
# Begin Source File
SOURCE=.\vrml/vgui_vrml_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vrml/vgui_vrml_texture_map.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_register_all.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/glut/Templates/vcl_vector+vgui_glut_adaptor~-.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/glut/Templates/vcl_vector+vgui_glut_menu_hack+-per_window_record~-.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/glut/menu_hack_X11.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/glut/menu_hack_none.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/glut/vgui_glut_adaptor.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/glut/vgui_glut_impl.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/glut/vgui_glut_popup_impl.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/glut/vgui_glut_tag.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/glut/vgui_glut_window.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/gtk/Templates/vcl_vector+GtkWidget~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_algorithm+vgui_rubberband_tableau_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vgui_easy2D_tableau_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/gtk/vgui_gtk.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/gtk/vgui_gtk_adaptor.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/gtk/vgui_gtk_dialog_impl.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/gtk/vgui_gtk_statusbar.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/gtk/vgui_gtk_tag.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/gtk/vgui_gtk_utils.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/gtk/vgui_gtk_window.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_adaptor.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_app.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_app_init.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_dialog_impl.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_doc.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_mainfrm.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_statusbar.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_tag.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_utils.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_view.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_window.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/qt/vgui_qt.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/qt/vgui_qt_adaptor.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/qt/vgui_qt_dialog_impl.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/qt/vgui_qt_menu.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/qt/vgui_qt_statusbar.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/qt/vgui_qt_tag.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/qt/vgui_qt_window.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/qt/vgui_qt_adaptor_mocced.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/qt/vgui_qt_dialog_impl_mocced.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/qt/vgui_qt_menu_mocced.cxx
# End Source File
# Begin Source File
SOURCE=.\impl/qt/vgui_qt_statusbar_mocced.cxx
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_parent_child_link_data.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_active_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_blackbox_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_blender_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_composite_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_displaybase_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_displaylist3D_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_easy2D_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_enhance_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_listmanager2D_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_parent_child_link.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_poly_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_rubberband_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_tview_launcher_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_tview_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_viewer2D_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_displaylist2D_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_easy3D_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_loader_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_viewer3D_tableau.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_renderer+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_renderer+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_renderer+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_renderer+int_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_renderer+int_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_renderer+sbyte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_renderer+uint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_renderer+uint_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_renderer+vil2_rgb+byte--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_renderer+vil2_rgb+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_renderer+vil2_rgb+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_renderer+vil2_rgb+int_16--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_renderer+vil2_rgb+int_32--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_renderer+vil2_rgb+sbyte--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_renderer+vil2_rgb+uint_16--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_renderer+vil2_rgb+uint_32--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_renderer+vil2_rgba+byte--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_renderer+vil2_rgba+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_tableau+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_tableau+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_tableau+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_tableau+int_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_tableau+int_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_tableau+sbyte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_tableau+uint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_tableau+uint_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_tableau+vil2_rgb+byte--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_tableau+vil2_rgb+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_tableau+vil2_rgb+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_tableau+vil2_rgb+int_16--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_tableau+vil2_rgb+int_32--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_tableau+vil2_rgb+sbyte--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_tableau+vil2_rgb+uint_16--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_tableau+vil2_rgb+uint_32--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_tableau+vil2_rgba+byte--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_image_tableau+vil2_rgba+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_section_buffer_apply+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_section_buffer_apply+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_section_buffer_apply+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_section_buffer_apply+int_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_section_buffer_apply+int_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_section_buffer_apply+sbyte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_section_buffer_apply+uint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgui_vil2_section_buffer_apply+uint_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\vgui_vil2_section_buffer.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\internals/trackball.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_accelerate.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_accelerate_mfc.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_accelerate_x11.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_adaptor_mixin.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_adaptor_tableau.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_back_project.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_dialog_field.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_dialog_impl.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_draw_line.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_file_field.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_invert_homg4x4.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_multiply_4x4.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_overlay_helper.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_rasterpos.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_simple_field.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_string_field.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_transpose_4x4.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_un_project.h
# End Source File
# Begin Source File
SOURCE=.\vgui.h
# End Source File
# Begin Source File
SOURCE=.\vgui_adaptor.h
# End Source File
# Begin Source File
SOURCE=.\vgui_button.h
# End Source File
# Begin Source File
SOURCE=.\vgui_cache_wizard.h
# End Source File
# Begin Source File
SOURCE=.\vgui_camera.h
# End Source File
# Begin Source File
SOURCE=.\vgui_clear_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_color.h
# End Source File
# Begin Source File
SOURCE=.\vgui_color_text.h
# End Source File
# Begin Source File
SOURCE=.\vgui_command.h
# End Source File
# Begin Source File
SOURCE=.\vgui_debug_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_deck_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_dialog.h
# End Source File
# Begin Source File
SOURCE=.\vgui_drag_mixin.h
# End Source File
# Begin Source File
SOURCE=.\vgui_drag_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_error_dialog.h
# End Source File
# Begin Source File
SOURCE=.\vgui_event.h
# End Source File
# Begin Source File
SOURCE=.\vgui_event_condition.h
# End Source File
# Begin Source File
SOURCE=.\vgui_event_server.h
# End Source File
# Begin Source File
SOURCE=.\vgui_find.h
# End Source File
# Begin Source File
SOURCE=.\vgui_function_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_glut.h
# End Source File
# Begin Source File
SOURCE=.\vgui_grid_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_image_renderer.h
# End Source File
# Begin Source File
SOURCE=.\vgui_image_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_key.h
# End Source File
# Begin Source File
SOURCE=.\vgui_macro.h
# End Source File
# Begin Source File
SOURCE=.\vgui_matrix_state.h
# End Source File
# Begin Source File
SOURCE=.\vgui_menu.h
# End Source File
# Begin Source File
SOURCE=.\vgui_message.h
# End Source File
# Begin Source File
SOURCE=.\vgui_modifier.h
# End Source File
# Begin Source File
SOURCE=.\vgui_observable.h
# End Source File
# Begin Source File
SOURCE=.\vgui_observer.h
# End Source File
# Begin Source File
SOURCE=.\vgui_pixel.h
# End Source File
# Begin Source File
SOURCE=.\vgui_popup_params.h
# End Source File
# Begin Source File
SOURCE=.\vgui_projection_inspector.h
# End Source File
# Begin Source File
SOURCE=.\vgui_quit_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_roi_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_section_buffer.h
# End Source File
# Begin Source File
SOURCE=.\vgui_section_buffer_of.h
# End Source File
# Begin Source File
SOURCE=.\vgui_section_render.h
# End Source File
# Begin Source File
SOURCE=.\vgui_shell_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_soview.h
# End Source File
# Begin Source File
SOURCE=.\vgui_soview2D.h
# End Source File
# Begin Source File
SOURCE=.\vgui_soview3D.h
# End Source File
# Begin Source File
SOURCE=.\vgui_statusbar.h
# End Source File
# Begin Source File
SOURCE=.\vgui_statusbuf.h
# End Source File
# Begin Source File
SOURCE=.\vgui_style.h
# End Source File
# Begin Source File
SOURCE=.\vgui_style_factory.h
# End Source File
# Begin Source File
SOURCE=.\vgui_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_tag.h
# End Source File
# Begin Source File
SOURCE=.\vgui_text_graph.h
# End Source File
# Begin Source File
SOURCE=.\vgui_text_put.h
# End Source File
# Begin Source File
SOURCE=.\vgui_text_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_texture_hacks.h
# End Source File
# Begin Source File
SOURCE=.\vgui_toolkit.h
# End Source File
# Begin Source File
SOURCE=.\vgui_utils.h
# End Source File
# Begin Source File
SOURCE=.\vgui_window.h
# End Source File
# Begin Source File
SOURCE=.\vgui_wrapper_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vrml/vgui_vrml_draw_visitor.h
# End Source File
# Begin Source File
SOURCE=.\vrml/vgui_vrml_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vrml/vgui_vrml_texture_map.h
# End Source File
# Begin Source File
SOURCE=.\vgui_clear_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_debug_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_deck_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_fwd.h
# End Source File
# Begin Source File
SOURCE=.\vgui_gl.h
# End Source File
# Begin Source File
SOURCE=.\vgui_glu.h
# End Source File
# Begin Source File
SOURCE=.\vgui_glx.h
# End Source File
# Begin Source File
SOURCE=.\impl/glut/vgui_glut_adaptor.h
# End Source File
# Begin Source File
SOURCE=.\impl/glut/vgui_glut_impl.h
# End Source File
# Begin Source File
SOURCE=.\impl/glut/vgui_glut_popup_impl.h
# End Source File
# Begin Source File
SOURCE=.\impl/glut/vgui_glut_window.h
# End Source File
# Begin Source File
SOURCE=.\impl/gtk/vgui_gtk.h
# End Source File
# Begin Source File
SOURCE=.\impl/gtk/vgui_gtk_adaptor.h
# End Source File
# Begin Source File
SOURCE=.\impl/gtk/vgui_gtk_dialog_impl.h
# End Source File
# Begin Source File
SOURCE=.\impl/gtk/vgui_gtk_statusbar.h
# End Source File
# Begin Source File
SOURCE=.\impl/gtk/vgui_gtk_utils.h
# End Source File
# Begin Source File
SOURCE=.\impl/gtk/vgui_gtk_window.h
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc.h
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_adaptor.h
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_app.h
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_app_init.h
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_dialog_impl.h
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_doc.h
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_mainfrm.h
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_statusbar.h
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_utils.h
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_view.h
# End Source File
# Begin Source File
SOURCE=.\impl/mfc/vgui_mfc_window.h
# End Source File
# Begin Source File
SOURCE=.\impl/qt/vgui_qt.h
# End Source File
# Begin Source File
SOURCE=.\impl/qt/vgui_qt_adaptor.h
# End Source File
# Begin Source File
SOURCE=.\impl/qt/vgui_qt_dialog_impl.h
# End Source File
# Begin Source File
SOURCE=.\impl/qt/vgui_qt_menu.h
# End Source File
# Begin Source File
SOURCE=.\impl/qt/vgui_qt_statusbar.h
# End Source File
# Begin Source File
SOURCE=.\impl/qt/vgui_qt_window.h
# End Source File
# Begin Source File
SOURCE=.\vgui_grid_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_image_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_quit_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_roi_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_shell_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_text_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_wrapper_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_active_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_active_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_blackbox_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_blackbox_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_blender_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_blender_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_composite_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_composite_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_displaybase_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_displaybase_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_displaylist2D_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_displaylist3D_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_drag_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_easy2D_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_easy3D_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_enhance_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_function_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_listmanager2D_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_listmanager2D_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_loader_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_parent_child_link.h
# End Source File
# Begin Source File
SOURCE=.\vgui_poly_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_poly_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_rubberband_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_rubberband_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_satellite_tableau.txx
# End Source File
# Begin Source File
SOURCE=.\vgui_tview_launcher_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_tview_launcher_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_tview_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_viewer2D_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_viewer2D_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_viewer3D_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\impl/glut/menu_hack.h
# End Source File
# Begin Source File
SOURCE=.\vgui_displaylist2D_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_displaylist3D_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_easy2D_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_easy3D_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_enhance_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_loader_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_satellite_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_tview_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_viewer3D_tableau.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_parent_child_link_data.h
# End Source File
# Begin Source File
SOURCE=.\internals/vgui_gl_selection_macros.h
# End Source File
# Begin Source File
SOURCE=.\vgui_vil2_image_renderer.h
# End Source File
# Begin Source File
SOURCE=.\vgui_vil2_image_renderer.txx
# End Source File
# Begin Source File
SOURCE=.\vgui_vil2_image_tableau.h
# End Source File
# Begin Source File
SOURCE=.\vgui_vil2_image_tableau.txx
# End Source File
# Begin Source File
SOURCE=.\vgui_vil2_image_tableau_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vgui_vil2_section_buffer.h
# End Source File
# Begin Source File
SOURCE=.\vgui_vil2_section_buffer_apply.h
# End Source File
# Begin Source File
SOURCE=.\vgui_vil2_section_buffer_apply.txx
# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
