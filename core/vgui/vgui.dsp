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
# /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/v3p"  == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -D_AFXDLL -DHAS_QV == compiler defines
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
!MESSAGE "vgui - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "vgui - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vgui - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
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
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/v3p"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -D_AFXDLL -DHAS_QV /D "vgui_EXPORTS"
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
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /GZ /c
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/v3p"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -D_AFXDLL -DHAS_QV /D "vgui_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ELSEIF  "$(CFG)" == "vgui - Win32 MinSizeRel"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MinSizeRel"
# PROP BASE Intermediate_Dir "MinSizeRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinSizeRel"
# PROP Intermediate_Dir "MinSizeRel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "_MBCS" /D "_ATL_DLL"  /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FD /c
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/v3p"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -D_AFXDLL -DHAS_QV /D "vgui_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O1
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "vgui - Win32 RelWithDebInfo"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RelWithDebInfo"
# PROP BASE Intermediate_Dir "RelWithDebInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "RelWithDebInfo"
# PROP Intermediate_Dir "RelWithDebInfo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/v3p"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -D_AFXDLL -DHAS_QV /D "vgui_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /Zi /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "vgui - Win32 Release"
# Name "vgui - Win32 Debug"
# Name "vgui - Win32 MinSizeRel"
# Name "vgui - Win32 RelWithDebInfo"


# Begin Source File

SOURCE=.\"$(IUEROOT)/oxl/vgui/CMakeLists.txt"

!IF  "$(CFG)" == "vgui - Win32 Release"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindGLUT.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindQv.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindX11.cmake"\
	"$(IUEROOT)/oxl/CMakeLists.txt"\
	"$(IUEROOT)/oxl/vgui/CMakeLists.txt"
# Begin Custom Build

"vgui.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/oxl/vgui/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/oxl/vgui" -O"$(IUEROOT)/oxl/vgui" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "vgui - Win32 Debug"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindGLUT.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindQv.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindX11.cmake"\
	"$(IUEROOT)/oxl/CMakeLists.txt"\
	"$(IUEROOT)/oxl/vgui/CMakeLists.txt"
# Begin Custom Build

"vgui.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/oxl/vgui/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/oxl/vgui" -O"$(IUEROOT)/oxl/vgui" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "vgui - Win32 MinSizeRel"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindGLUT.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindQv.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindX11.cmake"\
	"$(IUEROOT)/oxl/CMakeLists.txt"\
	"$(IUEROOT)/oxl/vgui/CMakeLists.txt"
# Begin Custom Build

"vgui.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/oxl/vgui/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/oxl/vgui" -O"$(IUEROOT)/oxl/vgui" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "vgui - Win32 RelWithDebInfo"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindGLUT.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindQv.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindX11.cmake"\
	"$(IUEROOT)/oxl/CMakeLists.txt"\
	"$(IUEROOT)/oxl/vgui/CMakeLists.txt"
# Begin Custom Build

"vgui.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/oxl/vgui/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/oxl/vgui" -O"$(IUEROOT)/oxl/vgui" -B"$(IUEROOT)"

# End Custom Build

!ENDIF

# End Source File
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

SOURCE=.\Templates/vcl_vector+vgui_polytab+-item-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vgui_rubberbander_sptr-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vgui_slab~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vgui_slot-.cxx

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

SOURCE=.\Templates/vcl_vector+vgui_tview+-icon-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgui_simple_command+vgui_active_visible-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgui_simple_command+vgui_adaptor-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgui_simple_command+vgui_clear_tableau-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgui_simple_command+vgui_tableau-.cxx

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

SOURCE=.\internals/vgui_slot_data.cxx

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

SOURCE=.\vgui_active_visible.cxx

# End Source File
# Begin Source File

SOURCE=.\vgui_adaptor.cxx

# End Source File
# Begin Source File

SOURCE=.\vgui_blackbox.cxx

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

SOURCE=.\vgui_composite.cxx

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

SOURCE=.\vgui_displaybase.cxx

# End Source File
# Begin Source File

SOURCE=.\vgui_displaylist2D.cxx

# End Source File
# Begin Source File

SOURCE=.\vgui_displaylist3D.cxx

# End Source File
# Begin Source File

SOURCE=.\vgui_drag_mixin.cxx

# End Source File
# Begin Source File

SOURCE=.\vgui_drag_tableau.cxx

# End Source File
# Begin Source File

SOURCE=.\vgui_easy2D.cxx

# End Source File
# Begin Source File

SOURCE=.\vgui_easy3D.cxx

# End Source File
# Begin Source File

SOURCE=.\vgui_enhance.cxx

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

SOURCE=.\vgui_event_loop.cxx

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

SOURCE=.\vgui_image_blender.cxx

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

SOURCE=.\vgui_listmanager2D.cxx

# End Source File
# Begin Source File

SOURCE=.\vgui_load.cxx

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

SOURCE=.\vgui_polytab.cxx

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

SOURCE=.\vgui_rubberbander.cxx

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

SOURCE=.\vgui_slab.cxx

# End Source File
# Begin Source File

SOURCE=.\vgui_slot.cxx

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

SOURCE=.\vgui_test.cxx

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

SOURCE=.\vgui_tview.cxx

# End Source File
# Begin Source File

SOURCE=.\vgui_tview_launcher.cxx

# End Source File
# Begin Source File

SOURCE=.\vgui_utils.cxx

# End Source File
# Begin Source File

SOURCE=.\vgui_viewer2D.cxx

# End Source File
# Begin Source File

SOURCE=.\vgui_viewer3D.cxx

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
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

