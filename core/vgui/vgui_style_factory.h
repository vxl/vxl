#ifndef vgui_style_factory_h_
#define vgui_style_factory_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME vgui_style_factory - Undocumented class FIXME
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_style_factory.h
// .FILE vgui_style_factory.cxx
//
// .SECTION Description:
//
// vgui_style_factory is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author:
//              Philip C. Pritchett, 18 Oct 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vcl_functional.h>
#include <vcl_utility.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include "dll.h"
class vgui_style;
class vgui_soview;

class vgui_style_factory
{
public:
 ~vgui_style_factory();

  static vgui_style_factory* instance();

  static vgui_style* get_style(float r, float g, float b, float point_size, float line_width);
  static void get_styles(vcl_vector<vgui_style*>& styles_copy);
  static void get_soviews(vgui_style*, vcl_vector<vgui_soview*>& soviews);
  static void change_style(vgui_soview* so, vgui_style* st_new, vgui_style* st_old);



  typedef vcl_multimap<vgui_style*, vgui_soview*, vcl_less<vgui_style*> > MultiMap_styles;

  class so_equal {
  public:
    so_equal(vgui_soview* s1_) : s1(s1_) {}

    bool operator() (MultiMap_styles::value_type obj);

    vgui_soview *s1;
  };

  static vgui_DLLDATA bool use_factory;

protected:

  vgui_style_factory();
  static vgui_DLLDATA vgui_style_factory* instance_;

  vgui_style* get_style_impl(float r, float g, float b, float point_size, float line_width);
  void get_styles_impl(vcl_vector<vgui_style*>& styles_copy);
  void get_soviews_impl(vgui_style*, vcl_vector<vgui_soview*>& soviews);
  void change_style_impl(vgui_soview* so, vgui_style* st_new, vgui_style* st_old);



  //vgui_style* default_style;
  vcl_vector<vgui_style*> styles;
  MultiMap_styles styles_map;

};

#endif // vgui_style_factory_h_
