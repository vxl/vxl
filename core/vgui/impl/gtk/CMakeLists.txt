#
# ./oxl/vgui/impl/gtk/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

ADD_LIBRARY(vgui_gtk vgui_gtk_sources)

SOURCE_FILES(vgui_gtk_sources
vgui_gtk              vgui_gtk_tag
vgui_gtk_adaptor      vgui_gtk_utils
vgui_gtk_dialog_impl  vgui_gtk_window
vgui_gtk_statusbar
)
