// This is brl/bbas/bgui/bgui_vsol2D_tableau.h
#ifndef bgui_vsol2D_tableau_h_
#define bgui_vsol2D_tableau_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief A child tableau of vgui_easy2D_tableau that knows how to display vsol objects.
// \author
//   Amir Tamrakar
//
//   Default styles are defined for each geometry object soview.
//   Users can change the default style by using the set_*_style commands,
//
// \verbatim
//  Modifications:
//   Amir Tamrakar April 22, 2002    Initial version: Seperated from vtol_tableau_2d
// \endverbatim
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vgui/vgui_style.h>

#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#if 0
#include <vsol/vsol_conic_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_rectangle_2d_sptr.h>
#include <vsol/vsol_triangle_2d_sptr.h>
#include <vsol/vsol_group_2d_sptr.h>
#endif

#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bgui/bgui_style_sptr.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <bgui/bgui_vsol2D_tableau_sptr.h>

class bgui_vsol_soview2D_point;
class bgui_vsol_soview2D_line_seg;
class bgui_vsol_soview2D_polyline;
class bgui_vsol_soview2D_digital_curve;
class bgui_vsol_soview2D_dotted_digital_curve;

class bgui_vsol2D_tableau : public vgui_easy2D_tableau
{
 public:
  bgui_vsol2D_tableau(const char* n="unnamed");

  bgui_vsol2D_tableau(vgui_image_tableau_sptr const& it,
                      const char* n="unnamed");

  bgui_vsol2D_tableau(vgui_tableau_sptr const& t,
                      const char* n="unnamed");

  ~bgui_vsol2D_tableau();

  //:virtual handle method for events
  virtual bool handle(vgui_event const &);

  //: display for vsol_point_2d
  bgui_vsol_soview2D_point*
    add_vsol_point_2d(vsol_point_2d_sptr const& p);

  //: display for vsol_line_2d
  bgui_vsol_soview2D_line_seg*
    add_vsol_line_2d(vsol_line_2d_sptr const& line);

  //: display for vsol_polyline_2d
  bgui_vsol_soview2D_polyline*
    add_vsol_polyline_2d(vsol_polyline_2d_sptr const& pline);

  //: display for digital_curve
  bgui_vsol_soview2D_digital_curve*
    add_digital_curve(vdgl_digital_curve_sptr const& dc);

  //: display for digital_curve
  bgui_vsol_soview2D_dotted_digital_curve*
    add_dotted_digital_curve(vdgl_digital_curve_sptr const& dc);

  void add_spatial_object(vsol_spatial_object_2d_sptr const& sos);

  //: display methods for vectors of vsol classes (not grouped)
  void add_spatial_objects(vcl_vector<vsol_spatial_object_2d_sptr> const& sos);

  //: clear the tableau including the highlight map
  void clear_all();

  //: Methods for getting mapped objects
  void enable_highlight(){highlight_ = true;}
  void disable_highlight(){highlight_ = false;}


  //: Methods for changing the default style of displayable objects
  void set_vsol_spatial_object_2d_style(vsol_spatial_object_2d_sptr sos,
                                        const float r, const float g, const float b,
                                        const float line_width, const float point_radius);
  void set_vsol_point_2d_style(const float r, const float g, const float b,
                               const float point_radius);

  void set_vsol_line_2d_style(const float r, const float g, const float b,
                              const float line_width);

  void set_vsol_polyline_2d_style(const float r, const float g, const float b,
                                  const float line_width);

  void set_digital_curve_style(const float r, const float g, const float b,
                               const float line_width);

  void set_dotted_digital_curve_style(const float r, const float g,
                                      const float b,
                                      const float line_width,
                                      const float point_radius);
 protected:
  bool highlight_;
  void init();
  vcl_map<vcl_string, bgui_style_sptr> style_map_;
  int old_id_;
  bgui_style_sptr highlight_style_;
  bgui_style_sptr old_style_;
};

//this stuff is needed to establish inheritance between tableau  smart pointers
//cloned from xcv_image_tableau
struct bgui_vsol2D_tableau_new : public bgui_vsol2D_tableau_sptr
{
  typedef bgui_vsol2D_tableau_sptr base;

  bgui_vsol2D_tableau_new(const char* n="unnamed") :
    base(new bgui_vsol2D_tableau(n)) { }
  bgui_vsol2D_tableau_new(vgui_image_tableau_sptr const& it,
                          const char* n="unnamed") :
    base(new bgui_vsol2D_tableau(it,n)) { }

  bgui_vsol2D_tableau_new(vgui_tableau_sptr const& t, const char* n="unnamed")
    :  base(new bgui_vsol2D_tableau(t, n)) { }

  operator vgui_easy2D_tableau_sptr () const { vgui_easy2D_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};

#endif // bgui_vsol2D_tableau_h_
