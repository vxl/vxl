class vgui_soview;
#include <vgui/vgui_style.h>
#include <vcl_map.txx>

VCL_MAP_INSTANTIATE(vgui_style*, vgui_soview*, vcl_less<vgui_style*> );
VCL_MULTIMAP_INSTANTIATE(vgui_style*, vgui_soview*, vcl_less<vgui_style*> );

#include <vgui/vgui_style_factory.h>
#include <vcl_algorithm.txx>

typedef vcl_multimap<vgui_style*, vgui_soview*, vcl_less<vgui_style*> >::iterator iter_t;
VCL_FIND_IF_INSTANTIATE(iter_t, vgui_style_factory::so_equal);

#ifdef VCL_SUNPRO_CC
template iter_t std::find_if(iter_t, iter_t, vgui_style_factory::so_equal);
template vgui_style **std::find_if(vgui_style **, vgui_style **, vgui_style_equal);
#endif
