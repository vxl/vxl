// This is brl/bbas/bgui/bgui_bargraph_clipon_tableau.h
#ifndef bgui_bargraph_clipon_tableau_h_
#define bgui_bargraph_clipon_tableau_h_
//:
// \file
// \author  Joe Mundy
// \brief   A tableau to produce a bar graph
//  The graph is plotted on an easy2D tableau
//  which this tableau points to not inherits from
// 
//  Thus the bar graph is a "clip on" to an existing 
//  tableau hierarchy that must include something
//  with "easy2D" power.
//  The bars can have up to eight different colors
//  As specified by the input color vector, with elements
//  ranging from 0-7;
#include <vbl/vbl_ref_count.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
class bgui_bargraph_clipon_tableau : public vbl_ref_count
{
 public:
  //: Constructor 
  bgui_bargraph_clipon_tableau(vgui_easy2D_tableau_sptr const& easy);
  //: Destructor.
  ~bgui_bargraph_clipon_tableau();
  //: Set the color vector, 0-7 color code
  void set_color_vector(vcl_vector<unsigned char> const& colors);

  //: Update the data vector.  Defaults determine size of plot from window
  void update(vcl_vector<float> const& bars, const bool fixed = false, 
              const float scale=1.0);

  //: Return the name of this tableau.
  vcl_string type_name() const { return "bgui_bargraph_clipon_tableau";}

  //: Clear the data
  void clear();
 private:
  int left_offset_;
  int top_offset_;
  int graph_width_;
  int graph_height_;
  int nominal_bar_width_;
  vcl_vector<vcl_vector<float> > color_values_;
  vcl_vector<unsigned char> color_index_;
  vcl_vector<float> bars_;
  vgui_easy2D_tableau_sptr easy_;
  vcl_vector<vgui_soview2D_lineseg*> bar_plot_;
};
#include <bgui/bgui_bargraph_clipon_tableau_sptr.h>

#endif // bgui_bargraph_clipon_tableau_h_
