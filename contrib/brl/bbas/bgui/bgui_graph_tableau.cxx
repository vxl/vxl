// This is brl/bbas/bgui/bgui_graph_tableau.cxx
#include <bgui/bgui_graph_tableau.h>
#include <vcl_cmath.h> //for fabs()
#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_easy2D_tableau.h>

void bgui_graph_tableau::draw_box()
{
  // Draw a square around the histogram
  vcl_vector<float> x_corners, y_corners;
  x_corners.push_back(left_offset_);
  x_corners.push_back(left_offset_+graph_width_);
  x_corners.push_back(left_offset_+graph_width_);
  x_corners.push_back(left_offset_);
  y_corners.push_back(top_offset_);
  y_corners.push_back(top_offset_);
  y_corners.push_back(top_offset_+graph_height_);
  y_corners.push_back(top_offset_+graph_height_);
  vgui_soview2D_polygon* sov =
    this->add_polygon(4, &x_corners[0], &y_corners[0]);
  sov->set_selectable(false);
}

void bgui_graph_tableau::init()
{
  hardware_ = false;
  this->set_foreground(0.8f, 1.0f, 0.0);
  this->set_line_width(5.0f);
  this->draw_box();
  this->draw_graph();
}

//========================================================================
// Constructors

bgui_graph_tableau::bgui_graph_tableau(const char* n) :
  vgui_easy2D_tableau(n), min_bar_(0), max_bar_(0),
  left_offset_(10), top_offset_(10),
  graph_width_(256), graph_height_(200), plot_(0)
{   this->init(); }

bgui_graph_tableau::bgui_graph_tableau(vgui_tableau_sptr const& t,
                                                         const char* n) :
  vgui_easy2D_tableau(t, n), min_bar_(0), max_bar_(0),
  left_offset_(10), top_offset_(10),
  graph_width_(256), graph_height_(200), plot_(0)
{   this->init(); }

// Destructor.
bgui_graph_tableau::~bgui_graph_tableau()
{
}

//:map the data value of histogrammed intensity to display coordinates
int bgui_graph_tableau::map_val_to_display(const double val)
{
  //compute display scale
  double scale = 1.0;
  if (vcl_fabs(max_-min_)>0)
    scale = graph_width_/(max_-min_);
  int display_x = (int)((val-min_)*scale + left_offset_);
  return display_x;
}

//:map the data value of histogrammed intensity to display coordinates
double bgui_graph_tableau::map_display_to_val(const int display_x)
{
  //compute display scale
  double scale = 1.0;
  if (vcl_fabs(max_-min_)>0)
    scale = graph_width_/(max_-min_);
  double temp = display_x - left_offset_;
  temp/=scale;
  return temp + min_;
}

void bgui_graph_tableau::draw_graph()
{
  if(data_.size() == 0)
    return;

  double max = data_[0];
  for (unsigned int i=1; i<data_.size(); ++i)
    if (max < data_[i]) max = data_[i];

  // scale and shift the data points
  vcl_vector<float> xscaled, yscaled;
  for (unsigned int i=0; i<data_.size(); ++i) {
    xscaled.push_back(left_offset_ + i);
    yscaled.push_back(top_offset_ + graph_height_ - data_[i]/max*graph_height_);
  }

  if (plot_)
  {
    // Update the plot points
    // This is a bit more efficient that deleting and reconstructing
    //   but not as "clean"
    for (unsigned int i=0; i<xscaled.size(); ++i) {
      plot_->x[i] = xscaled[i];
      plot_->y[i] = yscaled[i];
    }
  }
  else
  {
    plot_ = this->add_linestrip(xscaled.size(), &xscaled[0], &yscaled[0]);
    plot_->set_selectable(false);
  }
  this->post_redraw();
}

void bgui_graph_tableau::clear()
{
  this->remove(plot_);
  delete plot_;
  plot_ = 0;
  this->post_redraw();
}

bool bgui_graph_tableau::update(vcl_vector<double> const& data)
{
  if (!data.size())
    return false;
  
  data_ = data;
  draw_graph();
  return true;
}

bool bgui_graph_tableau::update(const double min, const double max,
                                         vcl_vector<double> const& data)
{
  if (!data.size())
    return false;
  min_ = min;
  max_ = max;
  data_ = data;

  draw_graph();
  return true;
}
