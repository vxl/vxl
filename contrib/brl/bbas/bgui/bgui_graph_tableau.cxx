// This is brl/bbas/bgui/bgui_graph_tableau.cxx
#include <iostream>
#include <cmath>
#include <sstream>
#include "bgui_graph_tableau.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_math.h>
#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

void bgui_graph_tableau::init()
{
  tt_ = vgui_text_tableau_new();
  tt_->set_colour(1,1,1);
  easy_ = vgui_easy2D_tableau_new();
  easy_->set_foreground(0.0, 1.0f, 0.0);
  easy_->set_line_width(2.0f);
  n_ = 0;
  pos_ = nullptr;
  vals_ = nullptr;
  tic_length_ = 10.0f;
  left_offset_ = 75;
  top_offset_ = 20;
  n_plots_ = 1;
}

//========================================================================
// Constructors

bgui_graph_tableau::bgui_graph_tableau(const unsigned gwidth,
                                       const unsigned gheight) :
  graph_width_(gwidth), graph_height_(gheight), plot_(nullptr)
{   this->init(); }

// Destructor.
bgui_graph_tableau::~bgui_graph_tableau()
{
  this->rem();
  this->del();
}

//: map the x position of a graph point to display coordinates
float bgui_graph_tableau::map_x_to_display(const float xpos)
{
  return left_offset_+ (xpos - xmin_)*xscale_;
}

//: map the y position of a graph point to display coordinates
float bgui_graph_tableau::map_y_to_display(const float ypos)
{
  return tic_length_ + graph_height_ - (ypos - yorigin_)*yscale_ + top_offset_;
}

//: find the nearest discrete y value less than or equal to ymin
static float find_y_origin(const float ymin, const float yinc)
{
  if (yinc==0)
    return 0;
  float nincs = std::floor(ymin/yinc);
  return nincs*yinc;
}
//-----------------------------------------------------------------------------
//: returns a "nice" tic mark increment, given the scale factor between user's coordinates and screen coordinates.
static float find_increment(float scale, float def = 1.0f)
{
  if (scale <= 0)
    return def;

  float separation = 50.0f / scale;

  // Find increment > separation

  float inc = 1;

  while (inc < separation)
    inc *= 10;

  // Find increment so sep/10 <= inc < sep

  while (inc > separation)
    inc /= 10;

  // Find smallest multiple of 1, 2 or 5 * 10^k > separation

  if (2 * inc > separation)
    inc *= 2;
  else if (5 * inc > separation)
    inc *= 5;
  else
    inc *= 10;

  return inc;
}

void bgui_graph_tableau::compute_scale()
{
  xscale_=1.0f; yscale_=1.0f;
  if (std::fabs(xmax_-xmin_)>0)
    xscale_ = (graph_width_-left_offset_)/(xmax_-xmin_);
  if (std::fabs(ymax_-ymin_)>0)
    yscale_ = (graph_height_-top_offset_)/(ymax_-ymin_);

  yinc_ = find_increment(yscale_);
  xinc_ = find_increment(xscale_);
  if (yinc_ == 0.0)
    yinc_ = 1;
  if (xinc_ == 0.0)
    xinc_ = 1;
  yorigin_ = find_y_origin(ymin_,yinc_);
}

void bgui_graph_tableau::update(std::vector<double> const& pos,
                                std::vector<double> const & vals)
{
  n_ = pos.size();
  pos_ = new float[n_];
  vals_ = new float[n_];
  ymin_ = xmin_ = vnl_numeric_traits<float>::maxval;
  ymax_ = xmax_ = -xmin_;
  for (unsigned i = 0; i<n_; ++i)
  {
    pos_[i]=static_cast<float>(pos[i]);
    xmin_ = std::min(xmin_, pos_[i]);
    xmax_ = std::max(xmax_, pos_[i]);
    vals_[i]=static_cast<float>(vals[i]);
    ymin_ = std::min(ymin_, vals_[i]);
    ymax_ = std::max(ymax_, vals_[i]);
  }
  compute_scale();
  draw_graph();
}


void bgui_graph_tableau::update(std::vector<float> const& pos,
                                std::vector<float> const & vals)
{
  n_ = pos.size();
  pos_ = new float[n_];
  vals_ = new float[n_];
  for (unsigned i = 0; i<n_; ++i)
  {
    pos_[i]=pos[i];
    xmin_ = std::min(xmin_, pos_[i]);
    xmax_ = std::max(xmax_, pos_[i]);
    vals_[i]=vals[i];
    ymin_ = std::min(ymin_, vals_[i]);
    ymax_ = std::max(ymax_, vals_[i]);
  }
  compute_scale();
  draw_graph();
}
// In the current implementation,
// all plots have to have the same number of positions and values.
// The vector position input is for future development where this
// class takes care of the multiple position case - JLM
void bgui_graph_tableau::update(std::vector<std::vector<double> > const& pos,
                                std::vector<std::vector<double> >const & vals)
{
  n_plots_ = pos.size();
  if (!n_plots_)
    return;
  n_ = pos[0].size();
  mpos_ = pos; mvals_=vals;
  double xmin, ymin, xmax, ymax;
  xmin = vnl_numeric_traits<double>::maxval;
  xmax = -xmin;
  ymin = xmin;
  ymax = xmax;
  for (unsigned p = 0; p<n_plots_; ++p)
    for (unsigned i = 0; i<n_; ++i)
    {
      xmin = std::min(xmin, pos[p][i]);
      xmax = std::max(xmax, pos[p][i]);
      ymin = std::min(ymin, vals[p][i]);
      ymax = std::max(ymax, vals[p][i]);
    }
  xmin_ = static_cast<float>(xmin);
  xmax_ = static_cast<float>(xmax);
  ymin_ = static_cast<float>(ymin);
  ymax_ = static_cast<float>(ymax);
  compute_scale();
  draw_multi_graph();
}

// Create the graph axes with tic marks. Every other tic is red and longer.
void bgui_graph_tableau::draw_tics()
{
  float xs = xmin_;
  float ys = yorigin_;
  float tl = tic_length_;
  unsigned ix = 0, iy = 0;
  //The bottom of the display
  float y0 = map_y_to_display(yorigin_);
  //The tic marks on the horizontal axis
  while (xs <= xmax_+xinc_)
  {
    float xd = map_x_to_display(xs);
    if (ix%2!=0)
      xtics_.push_back(easy_->add_line(xd, y0, xd, y0-tl));
    else
    {
      easy_->set_foreground(1.0f, 0.0, 0.0);
      xtics_.push_back(easy_->add_line(xd, y0, xd, y0-1.5f*tl));
      easy_->set_foreground(0.0f, 1.0f, 0.0);
    }
    std::stringstream ts;
    ts<<xs;
    std::string xval = ts.str();
    unsigned nchars = xval.size();
    float offset = static_cast<float>(nchars)/2;
    offset *= 10.0f;
    tt_->add(xd-offset, y0+15, xval);
    xs += xinc_;
    ++ix;
  }
  xtics_.push_back(easy_->add_line(map_x_to_display(xmin_), y0,
                                   map_x_to_display(xmax_+xinc_), y0));
  //The tic marks on the vertical axis
  //the vertical axis is at one tic length from left edge of graph
  while (ys <= ymax_+yinc_)
  {
    float yd = map_y_to_display(ys);
    if (iy%2!=0)
      ytics_.push_back(easy_->add_line(left_offset_,yd,
                                       tl+left_offset_,yd));
    else
    {
      easy_->set_foreground(1.0f, 0.0, 0.0);
      ytics_.push_back(easy_->add_line(left_offset_,yd,
                                       1.5f*tl+left_offset_,yd));
      easy_->set_foreground(0.0f, 1.0f, 0.0);
    }
    std::stringstream ts;
    ts<<ys;
    std::string yval = ts.str();
    float len = yval.size()+1;
     len*= 10.0f;
    tt_->add(left_offset_-len, yd+5.0f, yval);

    ys += yinc_;
    ++iy;
  }
  ytics_.push_back(easy_->
                   add_line(left_offset_,map_y_to_display(yorigin_),
                            left_offset_,
                            map_y_to_display(ymax_+yinc_)));

  //Make the tics and axes unselectable
  for (std::vector<vgui_soview2D_lineseg*>::iterator cit = xtics_.begin();
       cit!=xtics_.end(); ++cit)
    (*cit)->set_selectable(false);
  for (std::vector<vgui_soview2D_lineseg*>::iterator cit = ytics_.begin();
       cit!=ytics_.end(); ++cit)
    (*cit)->set_selectable(false);
}

//Draw the graph including the axes and tic marks
void bgui_graph_tableau::draw_graph()
{
  if (n_ == 0)
    return;
  std::vector<float> x(n_), y(n_);
  if (m_plot_.size())
  {
    for (unsigned i =0; i<m_plot_.size(); ++i)
      easy_->remove(m_plot_[i]);
    m_plot_.clear();
    tt_->clear();
  }
  if (plot_)
  {
    easy_->remove(plot_);
    tt_->clear();
    delete plot_;
    plot_ = nullptr;
  }
  for (unsigned i = 0; i<n_; ++i)
  {
    x[i]=map_x_to_display(pos_[i]);
    y[i]=map_y_to_display(vals_[i]);
  }
  plot_ = easy_->add_linestrip(n_, &x[0], &y[0]);
  plot_->set_selectable(false);
  draw_tics();
 this->post_redraw();
}

//Draw the multiple plots including the axes and tic marks
void bgui_graph_tableau::draw_multi_graph()
{
  //graph colors, where more than 7 plots repeat colors
  float r[7]={1,0,0,1,0,1,1};
  float g[7]={0,1,0,0,1,1,1};
  float b[7]={0,0,1,1,1,0,1};
  if (n_ == 0)
    return;
  if (m_plot_.size())
  {
    for (unsigned i =0; i<m_plot_.size(); ++i)
      easy_->remove(m_plot_[i]);
    m_plot_.clear();
    tt_->clear();
  }
  for (unsigned p = 0; p<n_plots_; ++p)
  {
    std::vector<float> x(n_), y(n_);
    for (unsigned i = 0; i<n_; ++i)
    {
      x[i]=map_x_to_display(static_cast<float>(mpos_[p][i]));
      y[i]=map_y_to_display(static_cast<float>(mvals_[p][i]));
    }
    unsigned c = p%7;
    easy_->set_foreground(r[c], g[c], b[c]);
    vgui_soview2D_linestrip* ls = easy_->add_linestrip(n_, &x[0], &y[0]);
    ls->set_selectable(false);
    m_plot_.push_back(ls);
  }
  draw_tics();
  this->post_redraw();
}

//remove display items and delete the soviews
void bgui_graph_tableau::rem()
{
  if (plot_)
  {
    easy_->remove(plot_);
    //delete plot_;
    plot_ = nullptr;
  }
  for (std::vector<vgui_soview2D_lineseg*>::iterator cit = xtics_.begin();
       cit!=xtics_.end(); ++cit)
    if (*cit)
    {
      easy_->remove(*cit);
     // delete *cit;
    }
  for (std::vector<vgui_soview2D_lineseg*>::iterator cit = ytics_.begin();
       cit!=ytics_.end(); ++cit)
    if (*cit)
    {
      easy_->remove(*cit);
      //delete *cit;
    }
  for (unsigned i = 0; i< m_plot_.size(); ++i)
    easy_->remove(m_plot_[i]);
  m_plot_.clear();
}

//Delete graph point data
void bgui_graph_tableau::del()
{
  if (pos_)
  {
    delete [] pos_;
    pos_ =nullptr;
  }
  if (vals_)
  {
    delete [] vals_;
    vals_ = nullptr;
  }
}

//Clear the graph
void bgui_graph_tableau::clear()
{
  this->rem();
  this->del();
  this->post_redraw();
}

// Provide a popup dialog that contains the graph.  The string info
// contains user defined documentation for the graph.
vgui_dialog* bgui_graph_tableau::popup_graph(std::string const& info,
                                             const unsigned sizex,
                                             const unsigned sizey)
{
  unsigned w = graph_width_+25, h = graph_height_+25;
  if (sizex>0)
    w = sizex;
  if (sizey>0)
    h = sizey;
  vgui_viewer2D_tableau_sptr v = vgui_viewer2D_tableau_new(this);
  vgui_shell_tableau_sptr s = vgui_shell_tableau_new(v);
  std::string temp = info;
  std::stringstream xinc, yinc, ymin, ymax;
  xinc << xinc_; yinc << yinc_; ymin << ymin_; ymax<< ymax_;
#if 0
  temp += " xinc:" + xinc.str();
  temp += " yinc:" + yinc.str();
#endif
  temp += " ymin:" + ymin.str();
  temp += " ymax:" + ymax.str();
  vgui_dialog* d = new vgui_dialog(temp.c_str());
  d->inline_tableau(s, w, h);
  return d;
}

//========================================================================
//: Handles all events for this tableau.
bool bgui_graph_tableau::handle(const vgui_event& event)
{
  // Pass events on down to the child tableaux:
  if (event.type == vgui_DRAW)
  {
    std::cout << "Graph Handle\n";
    easy_->handle(event);
    tt_->handle(event);
  }
  return false;
}
