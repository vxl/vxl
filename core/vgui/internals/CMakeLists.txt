

ADD_LIBRARY(vgui_internals vgui_internals_sources)

SOURCE_FILES(vgui_internals_sources
vgui_accelerate.cxx       vgui_dialog_field.cxx    vgui_rasterpos.cxx
vgui_accelerate_mfc.cxx   vgui_dialog_impl.cxx     vgui_simple_field.cxx
vgui_accelerate_tag.cxx   vgui_draw_line.cxx       vgui_slot_data.cxx
vgui_accelerate_x11.cxx   vgui_file_field.cxx      vgui_string_field.cxx
vgui_adaptor_mixin.cxx    vgui_invert_homg4x4.cxx  vgui_transpose_4x4.cxx
vgui_adaptor_tableau.cxx  vgui_multiply_4x4.cxx    vgui_un_project.cxx
vgui_back_project.cxx     vgui_overlay_helper.cxx
trackball.c
)