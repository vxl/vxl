// This is oxl/vgui/vrml/vgui_vrml_texture_map.h
#ifndef vgui_vrml_texture_map_h_
#define vgui_vrml_texture_map_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
// 
// .NAME vgui_vrml_texture_map
// .LIBRARY vgui-vrml
// .HEADER vxl Package
// .INCLUDE vgui/vrml/vgui_vrml_texture_map.h
// .FILE vgui_vrml_texture_map.cxx
//
// .SECTION Author
//              Philip C. Pritchett, 28 Sep 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//
//-----------------------------------------------------------------------------

#include <vcl_string.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_rgb.h>

struct vgui_vrml_texture_map
{
  vcl_string filename_;
  vil_memory_image_of<vil_rgb<unsigned char> > rgb;

  vgui_vrml_texture_map(char const* fn, int w, int h):
    filename_(fn),
    rgb(w,h)
    {
    }
  static vcl_string vrml_dirname;
  static vgui_vrml_texture_map* create(char const* filename);
};

#endif // vgui_vrml_texture_map_h_
