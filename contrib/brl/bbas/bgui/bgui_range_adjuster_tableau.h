// This is brl/bbas/bgui/bgui_range_adjuster_tableau.h
#ifndef bgui_range_adjuster_tableau_h_
#define bgui_range_adjuster_tableau_h_
//:
// \file
// \author  J. L. Mundy based on Matt Leotta's histogram tableau
// \brief   Display an interactive histogram to adjust image dynamic range

#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_range_map_params_sptr.h>
#include <bsta/bsta_histogram.h>
#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_range_adjuster_tableau_sptr.h>

class bgui_range_adjuster_tableau : public vgui_easy2D_tableau
{
 public:

  //: Constructor takes all the labels used on the graph.
  bgui_range_adjuster_tableau(const char* n="unnamed");

  bgui_range_adjuster_tableau(vgui_image_tableau_sptr const& it,
                              const char* n="unnamed");

  bgui_range_adjuster_tableau(vgui_tableau_sptr const& t, const char* n="unnamed");

  //: Returns the type of this tableau ('bgui_range_adjuster_tableau').
  //  vcl_string type_name() const { return "bgui_range_adjuster_tableau";}

  //: Destructor.
  ~bgui_range_adjuster_tableau();

  //: Update the histogram from image resource
  bool update(vil_image_resource_sptr const& ir);

  //: Update the histogram from image
  bool update(vil1_memory_image_of< vil1_rgb<unsigned char> >& img);

  //: Update the histogram from client
  bool update();

  //: Update the data from a histogram
  bool update(const double min, const double max,
              vcl_vector<double> const& hist);


  //: Low range limit histogram area fraction
  double low_range_limit(const double frac);

  //: Upper range limit histogram area fraction
  double upper_range_limit(const double frac);

  //: Use OpenGL Hardware mapping
  void set_hardware(bool hardware ){hardware_=hardware;}

  //: Handles all events for this tableau.
  virtual bool handle(const vgui_event&);
  //: Clear the data
  void clear();

  //: virtual
 private:
  //Utility functions
  void init();
  void draw_box();
  void draw_histogram();

  vgui_image_tableau_sptr get_child_image_tableau();

  int map_val_to_display(const double val);
  double map_display_to_val(const int display_x);
  void draw_stretch_bars();
  float screen_to_bar(const float sx);
  float bar_to_screen(const float bx);
  void adjust_min_bar(const float x);
  void adjust_max_bar(const float x);
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
  bsta_histogram<double> hist_;
};


//this stuff is needed to establish inheritance between tableau  smart pointers
//cloned from xcv_image_tableau
struct bgui_range_adjuster_tableau_new : public bgui_range_adjuster_tableau_sptr
{
  typedef bgui_range_adjuster_tableau_sptr base;

  bgui_range_adjuster_tableau_new(const char* n="unnamed") :
    base(new bgui_range_adjuster_tableau(n)) { }
  bgui_range_adjuster_tableau_new(vgui_image_tableau_sptr const& it,
                                  const char* n="unnamed") :
    base(new bgui_range_adjuster_tableau(it,n)) { }

  bgui_range_adjuster_tableau_new(vgui_tableau_sptr const& t, const char* n="unnamed")
    :  base(new bgui_range_adjuster_tableau(t, n)) { }

  operator vgui_easy2D_tableau_sptr () const { vgui_easy2D_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};

#endif // bgui_range_adjuster_tableau_h_
