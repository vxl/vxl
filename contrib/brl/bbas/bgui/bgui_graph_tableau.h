// This is brl/bbas/bgui/bgui_graph_tableau.h
#ifndef bgui_graph_tableau_h_
#define bgui_graph_tableau_h_
//:
// \file
// \author  K. Kang 
// \brief   intended to be general graph tableau

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_easy2D_tableau.h>
#include "bgui_graph_tableau_sptr.h"

class bgui_graph_tableau : public vgui_easy2D_tableau
{
 public:

  //: Constructor takes all the labels used on the graph.
  bgui_graph_tableau(const char* n="unnamed");

  bgui_graph_tableau(vgui_tableau_sptr const& t, const char* n="unnamed");

  bgui_graph_tableau(const vcl_vector<double> data)
  {
    data_ = data;
  }

  //: Destructor.
  ~bgui_graph_tableau();

  //: Update the histogram from client
  bool update(vcl_vector<double> const & data);

  //: Update the data from a histogram
  bool update(const double min, const double max,
              vcl_vector<double> const& hist);

  //: Clear the data
  void clear();

  //: virtual
 private:
  //Utility functions
  void init();
  void draw_box();
  void draw_graph();
  int map_val_to_display(const double val);
  double map_display_to_val(const int display_x);
 
  //Members
  double min_;
  double max_;
  bool hardware_;
  vgui_soview2D_lineseg* min_bar_;
  vgui_soview2D_lineseg* max_bar_;
  int left_offset_;
  int top_offset_;
  int graph_width_;
  int graph_height_;
  //: List of points.
  vcl_vector<float> xpoints_, ypoints_;
  vgui_soview2D_linestrip* plot_;
  vcl_vector<double> data_;
};


//this stuff is needed to establish inheritance between tableau  smart pointers
//cloned from xcv_image_tableau
struct bgui_graph_tableau_new : public bgui_graph_tableau_sptr
{
  typedef bgui_graph_tableau_sptr base;

  bgui_graph_tableau_new(const char* n="unnamed") :
    base(new bgui_graph_tableau(n)) { }
  bgui_graph_tableau_new(vgui_image_tableau_sptr const& it,
                                  const char* n="unnamed") :
    base(new bgui_graph_tableau(it,n)) { }

  bgui_graph_tableau_new(vgui_tableau_sptr const& t, const char* n="unnamed")
    :  base(new bgui_graph_tableau(t, n)) { }

  operator vgui_easy2D_tableau_sptr () const { vgui_easy2D_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};

#endif // bgui_graph_tableau_h_
