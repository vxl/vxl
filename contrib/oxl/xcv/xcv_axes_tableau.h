// This is oxl/xcv/xcv_axes_tableau.h
#ifndef xcv_axes_tableau_h_
#define xcv_axes_tableau_h_
//:
// \file
// \author  K.Y.McGaul
// \brief   A tableau to display points on a set of axes.
//
//  The graph is displayed using a text_tableau and an easy2D inside
//  a composite tableau.
//  Each point to be plotted is added using add_point and when all points
//  are added compute_axes should be called to lay out the graph.
//
// \verbatim
//  Modifications:
//    K.Y.McGaul   26-APR-2001  Initial version.
// \endverbatim

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_text_tableau_sptr.h>
#include <vgui/vgui_composite_tableau_sptr.h>

#include <xcv/xcv_axes_limits.h>
class vgui_tableau;

// <vgui_make_sptr>
#include "xcv_axes_tableau_sptr.h"
// </vgui_make_sptr>

class xcv_axes_tableau : public vgui_tableau
{
public:

  //: Constructor takes all the labels used on the graph.
  xcv_axes_tableau(std::string heading, std::string x_axes_label, std::string y_axes_label);
  //: Destructor.
  ~xcv_axes_tableau();
  //: Add another point to be plotted on the graph.
  void add_point(float x, float y);
  //: Lay out the graph
  void compute_axes();
  //: Return the name of this tableau.
  std::string type_name() const { return "xcv_axes_tableau";}
  //: Handles all events for this tableau.
  bool handle(const vgui_event&);

protected:

private:
  //: These compute the numbering and tick spacing on the axes.
  xcv_axes_limits xlimits_, ylimits_;
  //: Range of values for x and y.
  float xlow_, xhigh_, ylow_, yhigh_;
  //: Labels for the axes.
  std::string heading_, x_axes_label_, y_axes_label_;
  //: List of points.
  std::vector<float> xpoints_, ypoints_;
  vgui_easy2D_tableau_sptr easy_;
  vgui_text_tableau_sptr text_;
  vgui_composite_tableau_sptr comp_;
};


// <vgui_make_sptr>
struct xcv_axes_tableau_new : public xcv_axes_tableau_sptr
{
  typedef xcv_axes_tableau_sptr base;
  xcv_axes_tableau_new(std::string a="", std::string b="", std::string c="")
    : base(new xcv_axes_tableau(a,b,c)) { }
};
// <vgui_make_sptr>


#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS xcv_axes_tableau.
