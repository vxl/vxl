// This is core/vgui/vgui_style_factory.h
#ifndef vgui_style_factory_h_
#define vgui_style_factory_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  Factory that manufactures vgui_style objects.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   18 Oct 1999
//
// \verbatim
//  Modifications
//   18-OCT-1999 P.Pritchett - Initial version.
// \endverbatim

#include <vcl_functional.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include "dll.h"
class vgui_style;
class vgui_soview;

//: Factory to manufactures vgui_style objects.
//
//  If the style requested already exists then the vgui_style object for that
//  style is return.  If not a new vgui_style is created for that style.
//
//  This is a singleton class - there should only be one style_factory.
class vgui_style_factory
{
 public:

  //: Destructor - delete all styles held in the factory.
 ~vgui_style_factory();

  //: Returns the singleton instance of the style_factory.
  static vgui_style_factory* instance();

  //: Returns a pointer to the vgui_style corresponding to the given values.
  static vgui_style* get_style(float r, float g, float b, float point_size,
                               float line_width);

  //: Gets a list of pointers to all styles held in the factory.
  static void get_styles(vcl_vector<vgui_style*>& styles_copy);

  //: Gets a list of soviews with the given style.
  static void get_soviews(vgui_style*, vcl_vector<vgui_soview*>& soviews);

  //: Change the style of the given soview.
  static void change_style(vgui_soview* so, vgui_style* st_new,
                           vgui_style* st_old);

  //: Remove a style association.
  //
  // This should be done prior to destroying a soview.
  static void remove_style(vgui_soview* so);

  typedef vcl_multimap<vgui_style*, vgui_soview*, vcl_less<vgui_style*> > MultiMap_styles;

  class so_equal
  {
   public:
    so_equal(vgui_soview* s1_) : s1(s1_) {}

    bool operator() (MultiMap_styles::value_type obj);

    vgui_soview *s1;
  };

  static vgui_DLLDATA bool use_factory;

 protected:

  vgui_style_factory();
  static vgui_DLLDATA vgui_style_factory* instance_;

  vgui_style* get_style_impl(float r, float g, float b, float point_size,
                             float line_width);
  void get_styles_impl(vcl_vector<vgui_style*>& styles_copy);
  void get_soviews_impl(vgui_style*, vcl_vector<vgui_soview*>& soviews);
  void change_style_impl(vgui_soview* so, vgui_style* st_new, vgui_style* st_old);
  void remove_style_impl(vgui_soview* so);

  //vgui_style* default_style;
  vcl_vector<vgui_style*> styles;
  MultiMap_styles styles_map;
};

#endif // vgui_style_factory_h_
