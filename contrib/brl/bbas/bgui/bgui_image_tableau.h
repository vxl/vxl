// This is brl/bbas/bgui/bgui_image_tableau.h
#ifndef bgui_image_tableau_h_
#define bgui_image_tableau_h_
//:
// \file
// \author  Marko Bacic (u97mb@robots.ox.ac.uk)
// \brief An image tableau with an associated region of interest.
//
// \verbatim
//  Modifications
//   Marko Bacic    AUG-2000  Initial version.
//   K.Y.McGaul  05-AUG-2002  Print mouse position on status bar.
//   K.Y.McGaul  06-AUG-2002  Print RGB value on status bar.
//   Joseph Mundy   DEC-2002  Copied to brl to serve as a basis for extended functions
//   Peter Vanroose AUG-2004  Moved bgui_roi_tableau_make_roi to separate file
// \endverbatim

#include "bgui_image_tableau_sptr.h"
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_rubberband_tableau.h>

class bgui_image_tableau : public vgui_image_tableau
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

  bgui_image_tableau();
  bgui_image_tableau(vil1_image const &);
  bgui_image_tableau(char const *);

  vcl_string type_name() const { return "vgui_image_tableau"; }

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

struct bgui_image_tableau_new : public bgui_image_tableau_sptr
{
  typedef bgui_image_tableau_sptr base;
  bgui_image_tableau_new() : base(new bgui_image_tableau()) { }
  bgui_image_tableau_new(vil1_image const &i) : base(new bgui_image_tableau(i)) { }
  bgui_image_tableau_new(char const *n) : base(new bgui_image_tableau(n)) { }
  operator vgui_image_tableau_sptr () const { vgui_image_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};

#endif // bgui_image_tableau_h_
