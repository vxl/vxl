#
# ./oxl/vgui/internals/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


ADD_LIBRARY(vgui_internals vgui_internals_sources)

SOURCE_FILES(vgui_internals_sources
vgui_accelerate       vgui_dialog_field    vgui_rasterpos
vgui_accelerate_mfc   vgui_dialog_impl     vgui_simple_field
vgui_accelerate_tag   vgui_draw_line       vgui_slot_data
vgui_accelerate_x11   vgui_file_field      vgui_string_field
vgui_adaptor_mixin    vgui_invert_homg4x4  vgui_transpose_4x4
vgui_adaptor_tableau  vgui_multiply_4x4    vgui_un_project
vgui_back_project     vgui_overlay_helper
trackball
)