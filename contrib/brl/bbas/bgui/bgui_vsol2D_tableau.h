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
//   Mark Johnson June 13, 2003      Stopped using interior class functions to
//                                   highlight objects. Added support for
//                                   specifying colors of individual objects.
//   Mark Johnson June 20, 2003      Added support for 'linked' digital curve.
//   Matt Leotta  April 3, 2004      Cleaned up the interface and add switched to
//                                   vgui_style_sptr instead of vgui_style_factory
// \endverbatim
//-----------------------------------------------------------------------------
#include <vcl_vector.h>

#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>

#include <vdgl/vdgl_digital_curve_sptr.h>

#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_style_sptr.h>
#include <bgui/bgui_vsol2D_tableau_sptr.h>

class bgui_vsol_soview2D_point;
class bgui_vsol_soview2D_line_seg;
class bgui_vsol_soview2D_polyline;
class bgui_vsol_soview2D_polygon;
class bgui_vsol_soview2D_digital_curve;
class bgui_vsol_soview2D_edgel_curve;

class bgui_vsol2D_tableau : public vgui_easy2D_tableau
{
 public:
  bgui_vsol2D_tableau(const char* n="unnamed");

  bgui_vsol2D_tableau(vgui_image_tableau_sptr const& it,
                             const char* n="unnamed");

  bgui_vsol2D_tableau(vgui_tableau_sptr const& t,
                             const char* n="unnamed");

  ~bgui_vsol2D_tableau();

  //: Returns the type of this tableau ('bgui_vsol2D_tableau').
  vcl_string type_name() const { return "bgui_vsol2D_tableau";}

  //: display for vsol_point_2d
  bgui_vsol_soview2D_point*
    add_vsol_point_2d(vsol_point_2d_sptr const& p,
                      const vgui_style_sptr& style = NULL);

  //: display for vsol_line_2d
  bgui_vsol_soview2D_line_seg*
    add_vsol_line_2d(vsol_line_2d_sptr const& line,
                     const vgui_style_sptr& style = NULL);

  //: display for vsol_polyline_2d
  bgui_vsol_soview2D_polyline*
    add_vsol_polyline_2d(vsol_polyline_2d_sptr const& pline,
                         const vgui_style_sptr& style = NULL);

  //: display for vsol_polygon_2d
  bgui_vsol_soview2D_polygon*
    add_vsol_polygon_2d(vsol_polygon_2d_sptr const& pline,
                        const vgui_style_sptr& style = NULL);

  //: display for vsol_digital_curve_2d
  bgui_vsol_soview2D_digital_curve*
    add_digital_curve(vsol_digital_curve_2d_sptr const& dc,
                      const vgui_style_sptr& style = NULL);

  //: display for vsol_digital_curve_2d (dotted)
  bgui_vsol_soview2D_digital_curve*
    add_dotted_digital_curve(vsol_digital_curve_2d_sptr const& dc,
                             const vgui_style_sptr& style = NULL);
                             
  //: display for vdgl_digital_curve
  bgui_vsol_soview2D_edgel_curve*
    add_edgel_curve(vdgl_digital_curve_sptr const& dc,
                      const vgui_style_sptr& style = NULL);

  //: display for vdgl_digital_curve (dotted)
  bgui_vsol_soview2D_edgel_curve*
    add_dotted_edgel_curve(vdgl_digital_curve_sptr const& dc,
                             const vgui_style_sptr& style = NULL);

  void add_spatial_object(vsol_spatial_object_2d_sptr const& sos,
                          const vgui_style_sptr& style = NULL);

  //: display methods for vectors of vsol classes (not grouped)
  void add_spatial_objects(vcl_vector<vsol_spatial_object_2d_sptr> const& sos,
                           const vgui_style_sptr& style = NULL);


  //: Methods for changing the default style of displayable objects
  void set_vsol_spatial_object_2d_style(vsol_spatial_object_2d_sptr sos,
                                        const vgui_style_sptr& style);
  void set_vsol_point_2d_style(const vgui_style_sptr& style);
  void set_vsol_line_2d_style(const vgui_style_sptr& style);
  void set_vsol_polyline_2d_style(const vgui_style_sptr& style);
  void set_digital_curve_style(const vgui_style_sptr& style);
  void set_dotted_digital_curve_style(const vgui_style_sptr& style);
  void set_edgel_curve_style(const vgui_style_sptr& style);
  void set_dotted_edgel_curve_style(const vgui_style_sptr& style);

 protected:

  //: Default styles
  vgui_style_sptr point_style_;
  vgui_style_sptr line_style_;
  vgui_style_sptr polyline_style_;
  vgui_style_sptr digital_curve_style_;
  vgui_style_sptr dotted_digital_curve_style_;
  vgui_style_sptr edgel_curve_style_;
  vgui_style_sptr dotted_edgel_curve_style_;

  void init();
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
