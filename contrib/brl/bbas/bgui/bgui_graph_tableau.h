// This is brl/bbas/bgui/bgui_graph_tableau.h
#ifndef bgui_graph_tableau_h_
#define bgui_graph_tableau_h_
//:
// \file
// \author  K. Kang 
// \brief   intended to be general graph tableau
// \verbatim
// modified extensively by J.L. Mundy June 3, 2006
// \endverbatim
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_text_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include "bgui_graph_tableau_sptr.h"

class bgui_graph_tableau : public  vgui_tableau
{
 public:

  //: Constructors 
  bgui_graph_tableau(const unsigned graph_width,
                     const unsigned graph_height);

  //: Destructor.
  ~bgui_graph_tableau();

  //: Update the graph with new data
  //: Single plot
  void update(vcl_vector<double> const& pos, vcl_vector<double> const & vals);
  void update(vcl_vector<float> const& pos, vcl_vector<float> const & vals);
  //: multiple plots
  void update(vcl_vector<vcl_vector<double> > const& pos,
              vcl_vector<vcl_vector<double> >const & vals);
  //: Clear the data
  void clear();

  //: Accessors
  float xmin(){return xmin_;}
  float xmax(){return xmax_;}
  float ymin(){return ymin_;}
  float ymax(){return ymax_;}

  //: Get a conveniently wrapped popup dialog
  vgui_dialog* popup_graph(vcl_string const& info,
                          const unsigned sizex =0,
                          const unsigned sizey=0);

  //: Handles all events for this tableau.
  virtual bool handle(const vgui_event&);

 private:
  //Utility functions
  void init();
  void rem();
  void del();
  void draw_box();
  void draw_tics();
  void draw_graph();
  void draw_multi_graph();
  void compute_scale();
  float map_x_to_display(const float x);
  float map_y_to_display(const float y);
  //Members
  float xmin_;
  float xmax_;
  float ymin_;
  float ymax_;
  float xscale_;
  float yscale_;
  unsigned left_offset_;
  unsigned top_offset_;
  unsigned graph_width_;
  unsigned graph_height_;
  float tic_length_;
  vgui_text_tableau_sptr tt_;
  vgui_easy2D_tableau_sptr easy_;
  //: The graph
  vgui_soview2D_linestrip* plot_;
  vcl_vector<vgui_soview2D_linestrip*> m_plot_;
  //: the tics
  float xinc_, yinc_;
  float yorigin_;
  vcl_vector<vgui_soview2D_lineseg*> xtics_;
  vcl_vector<vgui_soview2D_lineseg*> ytics_;
  unsigned n_;
  float* pos_;
  float* vals_;
  unsigned n_plots_;
  vcl_vector<vcl_vector<double> > mpos_;
  vcl_vector<vcl_vector<double> > mvals_;
};

struct bgui_graph_tableau_new : public bgui_graph_tableau_sptr
{
  typedef bgui_graph_tableau_sptr base;
  bgui_graph_tableau_new(const unsigned gwidth, const unsigned gheight)
    : base(new bgui_graph_tableau(gwidth, gheight)) { }
};


#endif // bgui_graph_tableau_h_
