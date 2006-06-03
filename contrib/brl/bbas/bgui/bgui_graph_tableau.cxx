// This is brl/bbas/bgui/bgui_graph_tableau.cxx
#include <bgui/bgui_graph_tableau.h>
//:
// \file
#include <vcl_cmath.h> //for fabs()
#include <vcl_sstream.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_math.h>
#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

void bgui_graph_tableau::init()
{
  easy_ = vgui_easy2D_tableau_new();
  easy_->set_foreground(0.0, 1.0f, 0.0);
  easy_->set_line_width(2.0f);
  n_ = 0;
  pos_ = 0;
  vals_ = 0;
  tic_length_ = 10.0f;
  left_offset_ = 10.0f + tic_length_;
  top_offset_ = 10.0f + tic_length_;
}

//========================================================================
// Constructors

bgui_graph_tableau::bgui_graph_tableau(const unsigned gwidth,
                                       const unsigned gheight) :
  graph_width_(gwidth), graph_height_(gheight), plot_(0)
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
  return tic_length_ + graph_height_ - (ypos - ymin_)*yscale_ + top_offset_;
}

void bgui_graph_tableau::compute_scale()
{
  xscale_=1.0f; yscale_=1.0f;
  if (vcl_fabs(xmax_-xmin_)>0)
    xscale_ = (graph_width_-left_offset_)/(xmax_-xmin_);
  if (vcl_fabs(ymax_-ymin_)>0)
    yscale_ = (graph_height_-top_offset_)/(ymax_-ymin_);
}
void bgui_graph_tableau::update(vcl_vector<double> const& pos,
                                vcl_vector<double> const & vals)
{
  n_ = pos.size();
  pos_ = new float[n_];
  vals_ = new float[n_];
  xmin_ = vnl_numeric_traits<float>::maxval;
  xmax_ = -xmin_;
  ymin_ = xmin_;
  ymax_ = ymax_;
  for(unsigned i = 0; i<n_; ++i)
    {
      pos_[i]=static_cast<float>(pos[i]);
      xmin_ = vnl_math_min(xmin_, pos_[i]);
      xmax_ = vnl_math_max(xmax_, pos_[i]);
      vals_[i]=static_cast<float>(vals[i]);
      ymin_ = vnl_math_min(ymin_, vals_[i]);
      ymax_ = vnl_math_max(ymax_, vals_[i]);
    }
  compute_scale();
  draw_graph();
}


void bgui_graph_tableau::update(vcl_vector<float> const& pos,
                                vcl_vector<float> const & vals)
{
  n_ = pos.size();
  pos_ = new float[n_];
  vals_ = new float[n_];
  for(unsigned i = 0; i<n_; ++i)
    {
      pos_[i]=pos[i];
      xmin_ = vnl_math_min(xmin_, pos_[i]);
      xmax_ = vnl_math_max(xmax_, pos_[i]);
      vals_[i]=vals[i];
      ymin_ = vnl_math_min(ymin_, vals_[i]);
      ymax_ = vnl_math_max(ymax_, vals_[i]);
    }
  compute_scale();
  draw_graph();
}
//-----------------------------------------------------------------------------
//: find_increment will return a "nice" tic mark increment, given the
// scale factor between user's coordinates and screen coordinates.
static float find_increment(float scale, float def = 1.0)
{

  if(scale <= 0)
    return def;

  float separation = 50.0 / scale;

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
// Create the graph axes with tic marks. Every other tic is red and longer.
void bgui_graph_tableau::draw_tics()
{
  yinc_ = find_increment(yscale_);
  xinc_ = find_increment(xscale_);
  if(yinc_ == 0.0)
    yinc_ = 1;
  if(xinc_ == 0.0)
    xinc_ = 1;
  float xs = xmin_;
  float ys = ymin_;
  float tl = tic_length_;
  unsigned ix = 0, iy = 0;
  //The bottom of the display
  float y0 = map_y_to_display(ymin_);
  //The tic marks on the horizontal axis
  while(xs <= xmax_+xinc_)
    {
      float xd = map_x_to_display(xs);
      if(ix%2!=0)
        xtics_.push_back(easy_->add_line(xd, y0, xd, y0-tl));
      else
        {
          easy_->set_foreground(1.0f, 0.0, 0.0);
          xtics_.push_back(easy_->add_line(xd, y0, xd, y0-1.5f*tl));
          easy_->set_foreground(0.0f, 1.0f, 0.0);
        }
      xs += xinc_;
      ++ix;
    }
  xtics_.push_back(easy_->add_line(map_x_to_display(xmin_), y0,
                                   map_x_to_display(xmax_+xinc_), y0));
  //The tic marks on the vertical axis
  //the vertical axis is at one tic length from left edge of graph
  while(ys <= ymax_+yinc_)
    {
      float yd = map_y_to_display(ys);
      if(iy%2!=0)
        ytics_.push_back(easy_->add_line(tl,yd,2.0f*tl,yd));
      else
        {
          easy_->set_foreground(1.0f, 0.0, 0.0);
          ytics_.push_back(easy_->add_line(tl,yd,2.5f*tl,yd));
          easy_->set_foreground(0.0f, 1.0f, 0.0);
        }
      ys += yinc_;
      ++iy;
    }
  ytics_.push_back(easy_->add_line(tl,map_y_to_display(ymin_), 
                                   tl,map_y_to_display(ymax_+yinc_)));

  //Make the tics and axes unselectable
  for(vcl_vector<vgui_soview2D_lineseg*>::iterator cit = xtics_.begin();
      cit!=xtics_.end(); ++cit)
    (*cit)->set_selectable(false);
  for(vcl_vector<vgui_soview2D_lineseg*>::iterator cit = ytics_.begin();
      cit!=ytics_.end(); ++cit)
    (*cit)->set_selectable(false);
}

//Draw the graph including the axes and tic marks
void bgui_graph_tableau::draw_graph()
{
  if (n_ == 0)
    return;
  vcl_vector<float> x(n_), y(n_);
  if(plot_)
    {
      easy_->remove(plot_);
      delete plot_;
      plot_ = 0;
    }
  for(unsigned i = 0; i<n_; ++i)
      {
        x[i]=map_x_to_display(pos_[i]);
        y[i]=map_y_to_display(vals_[i]);
      }
    plot_ = easy_->add_linestrip(n_, &x[0], &y[0]);
    plot_->set_selectable(false);
    draw_tics();
    easy_->post_redraw();
}

//remove display items and delete the soviews
void bgui_graph_tableau::rem()
{
  if(plot_)
    {
      easy_->remove(plot_);
      delete plot_;
      plot_ = 0;
    }
  for(vcl_vector<vgui_soview2D_lineseg*>::iterator cit = xtics_.begin();
      cit!=xtics_.end(); ++cit)
    if(*cit)
      {
        easy_->remove(*cit);
        delete *cit;
      }
  for(vcl_vector<vgui_soview2D_lineseg*>::iterator cit = ytics_.begin();
      cit!=ytics_.end(); ++cit)
    if(*cit)
      {
        easy_->remove(*cit);
        delete *cit;
      }
}

//Delete graph point data
void bgui_graph_tableau::del()
{
  if(pos_)
    {
      delete [] pos_;
      pos_ =0;
    }
  if(vals_)
    {
      delete [] vals_;
      vals_ = 0;
    }
}
//Clear the graph
void bgui_graph_tableau::clear()
{
  this->rem();
  this->del();
  this->post_redraw();
}
//Provide a popup dialog that contains the graph.  The string info
//contains user defined documention for the graph. 
vgui_dialog* bgui_graph_tableau::popup_graph(vcl_string const& info,
                                            const unsigned sizex,
                                            const unsigned sizey)
{
  unsigned w = graph_width_+25, h = graph_height_+25;
  if(sizex>0)
    w = sizex;
  if(sizey>0)
    h = sizey;
  vgui_viewer2D_tableau_sptr v = vgui_viewer2D_tableau_new(this);
  vgui_shell_tableau_sptr s = vgui_shell_tableau_new(v);
  vcl_string temp = info;
  vcl_stringstream xinc, yinc, ymin, ymax;
  xinc << xinc_; yinc << yinc_; ymin << ymin_; ymax<< ymax_;
  temp += " xinc:" + xinc.str();
  temp += " yinc:" + yinc.str();
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
  return easy_->handle(event);

}

