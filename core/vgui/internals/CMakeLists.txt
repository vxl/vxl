#
# ./oxl/vgui/internals/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


ADD_LIBRARY(vgui_internals vgui_internals_sources)


SOURCE_FILES(vgui_internals_sources
vgui_accelerate_tag
vgui_rasterpos
trackball
vgui_dialog_field
vgui_simple_field
vgui_string_field
vgui_file_field
vgui_dialog_impl
vgui_invert_homg4x4
vgui_back_project
vgui_un_project
vgui_adaptor_tableau
vgui_slot_data
vgui_adaptor_mixin
vgui_overlay_helper
vgui_multiply_4x4
vgui_transpose_4x4
vgui_draw_line
vgui_accelerate
)

IF(WIN32)
  SOURCE_FILES(vgui_internals_sources vgui_accelerate_mfc)
ENDIF(WIN32)

INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindX11.cmake)
IF(X11_LIBRARY)
  INCLUDE_DIRECTORIES( ${X11_INCLUDE_PATH} )
  SOURCE_FILES(vgui_internals_sources
    vgui_accelerate_tag
    vgui_accelerate_x11
  )
ENDIF(X11_LIBRARY)

