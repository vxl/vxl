// This is oxl/xcv/xcv_image_tableau.h
#ifndef xcv_image_tableau_h_
#define xcv_image_tableau_h_
//:
// \file
// \author  Marko Bacic (u97mb@robots.ox.ac.uk)
// \brief An image tableau with an associated region of interest.
//
// \verbatim
//  Modifications:
//    Marko Bacic   AUG-2000   Initial version.
// \endverbatim

#include "xcv_image_tableau_sptr.h"
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_rubberband_tableau.h>

class xcv_image_tableau : public vgui_image_tableau
{
  struct ROI
  {
    float x;
    float y;
    float width;
    float height;
  };
  ROI roi_;
  bool defined_;

 public:
  typedef vgui_image_tableau base;

  xcv_image_tableau();
  xcv_image_tableau(vil1_image const &);
  xcv_image_tableau(char const *);

  vcl_string type_name() const;

  //: Return the vil1_image
  vil1_image get_image() const;

  //: Width of the ROI, or if not defined then the width of the whole image.
  unsigned width() const;
  //: Height of the ROI, or if not defined then the height of the whole image.
  unsigned height() const;
  //: Get the low and high values for the ROI.
  bool get_bounding_box(float low[3], float high[3]) const;

  //: Define region of interest
  void set_roi(float x,float y,float w,float h);
  //: Undefine region of interest
  void unset_roi();

 protected:
  //: Handle all events for this tableau.
  bool handle(vgui_event const &e);
};

struct xcv_image_tableau_new : public xcv_image_tableau_sptr
{
  typedef xcv_image_tableau_sptr base;
  xcv_image_tableau_new() : base(new xcv_image_tableau()) { }
  xcv_image_tableau_new(vil1_image const &i) : base(new xcv_image_tableau(i)) { }
  xcv_image_tableau_new(char const *n) : base(new xcv_image_tableau(n)) { }
  //operator vgui_image_tableau_sptr () const { vgui_image_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};

class vgui_roi_tableau_make_roi : public vgui_rubberband_client
{
  bool done_;
  xcv_image_tableau_sptr image_tableau_;
 public:
  vgui_roi_tableau_make_roi(xcv_image_tableau_sptr const&);
  void add_box(float,float,float,float);
  void add_point(float,float) {}
  void add_line(float,float,float,float) {}
  void add_circle(float,float,float) {}
  void add_linestrip(int,float const *,float const *) {}
  void add_infinite_line(float,float,float) {}
  bool is_done() { return done_; }
};

#endif // xcv_image_tableau_h_
