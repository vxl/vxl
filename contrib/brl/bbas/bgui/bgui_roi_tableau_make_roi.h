#ifndef bgui_roi_tableau_make_roi_h_
#define bgui_roi_tableau_make_roi_h_
//:
// \file
// \author  Marko Bacic (u97mb@robots.ox.ac.uk)
// \brief a region of interest associated to an image tableau.
//
// \verbatim
//  Modifications
//   Marko Bacic    AUG-2000  Initial version.
//   Peter Vanroose AUG-2004  Moved into separate file
// \endverbatim

#include <bgui/bgui_image_tableau.h>
#include <vgui/vgui_rubberband_tableau.h>

class bgui_roi_tableau_make_roi : public vgui_rubberband_client
{
  bool done_;
  bgui_image_tableau_sptr image_tableau_;
 public:
  bool is_done() const { return done_; }

  bgui_roi_tableau_make_roi(bgui_image_tableau_sptr const& imt)
   : done_(false), image_tableau_(imt)  {}

  void add_box(float x0,float y0,float x1,float y1)
  {
    inline void swap(float& a, float& b) { float t=a; a=b; b=t; }
    if (x0<x1) swap(x0,x1);
    if (y0<y1) swap(y0,y1);
    image_tableau_->set_roi(x1,y1,x0-x1,y0-y1);
    done_ = true;
  }
};

#endif // bgui_roi_tableau_make_roi_h_
