// This is brl/bbas/bgui/bgui_bmrf_soview2D.h
#ifndef bgui_bmrf_soview2D_h_
#define bgui_bmrf_soview2D_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief vgui_soview2D objects for bmrf classes
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 1/23/04
//
// \verbatim
//  Modifications:
// \endverbatim
//--------------------------------------------------------------------------------

#include <vcl_iosfwd.h>

#include <bmrf/bmrf_epi_seg_sptr.h>

#include <vgui/vgui_soview2D.h>

//: This is a soview2D for a bmrf_epi_seg with a smart pointer back to the bmrf_epi_seg
class bgui_bmrf_epi_seg_soview2D : public vgui_soview2D
{
 public:
  //: Constructor - creates a default view
  bgui_bmrf_epi_seg_soview2D( bmrf_epi_seg_sptr const & seg);

  //: Destructor
  ~bgui_bmrf_epi_seg_soview2D() {}

  //: Render the bmrf_epi_seg on the display.
  virtual void draw() const;

  //: Print details about this bmrf_epi_seg to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('dbgui_bmrf_epi_seg_soview2D').
  vcl_string type_name() const { return "bgui_bmrf_epi_seg_soview2D"; }

  //: Returns the distance squared from this epi_seg to the given position.
  virtual float distance_squared(float x, float y) const;

  //: Returns the centroid of this epi_seg.
  void get_centroid(float* x, float* y) const;

  //: Returns a smart pointer to the segment
  bmrf_epi_seg_sptr seg_sptr() const { return seg_sptr_; }

protected:
  //: Smart pointer to a bmrf_epi_seg
  bmrf_epi_seg_sptr seg_sptr_;
};


#endif // bgui_bmrf_soview2D_h_
