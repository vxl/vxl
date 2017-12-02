// This is brl/bbas/bgui/bgui_histogram_tableau.h
#ifndef bgui_histogram_tableau_h_
#define bgui_histogram_tableau_h_
//:
// \file
// \author  Matt Leotta
// \brief   A tableau to a histogram for an image
//  The histogram is plotted on an easy2D tableau
//
// \verbatim
//  Modifications
//   Ozge Can Ozcanli    May 2006  Added vil support to the update method
//
// \endverbatim

#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>
#include <vil/vil_image_view.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>

#include "bgui_histogram_tableau_sptr.h"

class bgui_histogram_tableau : public vgui_tableau
{
 public:
  //: Constructor takes all the labels used on the graph.
  bgui_histogram_tableau();
  //: Destructor.
  ~bgui_histogram_tableau();
  //: Update the histogram
  void update(vil1_memory_image_of< vil1_rgb<unsigned char> >& img);
  void update(vil_image_view< vxl_byte >& img);
  //: Return the name of this tableau.
  std::string type_name() const { return "bgui_histogram_tableau";}
  //: Handles all events for this tableau.
  bool handle(const vgui_event&);
  //: Clear the data
  void clear();
 private:
  int left_offset_;
  int top_offset_;
  int graph_width_;
  int graph_height_;
  //: List of points.
  std::vector<float> xpoints_, ypoints_;
  vgui_easy2D_tableau_sptr easy_;
  vgui_soview2D_linestrip* plot_;
  std::vector<double> data_;
};


struct bgui_histogram_tableau_new : public bgui_histogram_tableau_sptr
{
  typedef bgui_histogram_tableau_sptr base;
  bgui_histogram_tableau_new()
    : base(new bgui_histogram_tableau()) { }
};

#endif // bgui_histogram_tableau_h_
