// This is brl/bbas/bgui/bgui_bargraph_clipon_tableau.cxx
#include <bgui/bgui_bargraph_clipon_tableau.h>
//:
// \file
// \author  Joe Mundy
#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_easy2D_tableau.h>

//========================================================================
//: Constructor
bgui_bargraph_clipon_tableau::
bgui_bargraph_clipon_tableau(vgui_easy2D_tableau_sptr const& easy) 
  : left_offset_(10), top_offset_(10),
    graph_width_(256), graph_height_(200), nominal_bar_width_(5)
{
  easy_ = easy;
  easy_->set_foreground(1.0, 1.0, 0);
  easy_->set_line_width(3.0);
  // Draw a square around the bargraph_clipon
  vcl_vector<float> x_corners, y_corners;
  x_corners.push_back(left_offset_);
  x_corners.push_back(left_offset_+graph_width_);
  x_corners.push_back(left_offset_+graph_width_);
  x_corners.push_back(left_offset_);
  y_corners.push_back(top_offset_);
  y_corners.push_back(top_offset_);
  y_corners.push_back(top_offset_+graph_height_);
  y_corners.push_back(top_offset_+graph_height_);
  easy_->add_polygon(4, &x_corners[0], &y_corners[0]);
  // set up color values
  vcl_vector<float> c0(3), c1(3), c2(3), c3(3), c4(3);
  vcl_vector<float> c5(3), c6(3), c7(3);
  c0[0]=0.0;   c0[1]=0.0;   c0[2]=1.0;
  c1[0]=0.0;   c1[1]=1.0;   c1[2]=0.0;
  c2[0]=0.0;   c2[1]=1.0;   c2[2]=1.0;
  c3[0]=1.0;   c3[1]=0.0;   c3[2]=0.0;
  c4[0]=1.0;   c4[1]=0.0;   c4[2]=1.0;
  c5[0]=1.0;   c5[1]=1.0;   c5[2]=0.0;
  c6[0]=1.0;   c6[1]=1.0;   c6[2]=1.0;
  c7[0]=0.75;   c7[1]=0.0;   c7[2]=0.25;
  color_values_.push_back(c0);   color_values_.push_back(c1);
  color_values_.push_back(c2);   color_values_.push_back(c3);
  color_values_.push_back(c4);   color_values_.push_back(c5);
  color_values_.push_back(c6);   color_values_.push_back(c7);    
}

//========================================================================
//: Destructor.
bgui_bargraph_clipon_tableau::~bgui_bargraph_clipon_tableau()
{
  this->clear();
}

// Update the plot data.  Choice of fixed scale or variable scale
// If fixed = 0 then the plot is automatically scaled. If fixed is
// not zero then it is treated as the scale to be used. That is,
// the data is multiplied by fixed*graph_height_
void bgui_bargraph_clipon_tableau::update(vcl_vector<float> const& bars,
                                          const bool fixed,
                                          const float scale)
{
  //if not fixed then scale max to graph_height_
  int nbars = bars.size();
  bars_.resize(nbars);
  if(!nbars)
    {
      vcl_cout << "In bgui_bargraph_clipon_tableau::update(..) -"
               << " no data\n";
      return;
    }
  if(!fixed)
    {
      float max = bars[0];
      for (unsigned int i=1; i<nbars; ++i)
        if (max < bars[i]) max = bars[i];
      
      for (unsigned int i=0; i<nbars; ++i)
        bars_[i] = graph_height_ - (bars[i]/max)*graph_height_;
    }
  else
    for (unsigned int i=0; i<nbars; ++i)
      bars_[i] = graph_height_- bars[i]*graph_height_*fixed;

  //reconcile the number of plot bars
  float bar_width=nominal_bar_width_;
  bar_width = graph_width_/nbars;
  if(bar_width>5*nominal_bar_width_)
    bar_width = 5*nominal_bar_width_;
  if(bar_width<1)
    bar_width = 1;

  //The first n_plot_bars data values will be plotted the 
  //others are dropped
  this->clear();
  easy_->set_line_width(bar_width);
  for(unsigned int i = 0; i<nbars; i++)
    {
      float x0 = i*bar_width + left_offset_ + 1, x1 = x0;
      float y0 = top_offset_+graph_height_, y1 = top_offset_ + bars_[i];
      unsigned int color_index = 2;
      if(color_index_.size()>=nbars)
        {
          unsigned int temp = color_index_[i];
          if(temp<=7)
            color_index = temp;
        }
      vcl_vector<float> c = color_values_[color_index];
      easy_->set_foreground(c[0], c[1], c[2]);
      vgui_soview2D_lineseg* l = easy_->add_line(x0, y0, x1, y1);
      bar_plot_.push_back(l);
    }
  easy_->post_redraw();
}

void bgui_bargraph_clipon_tableau::set_color_vector(vcl_vector<unsigned char> const& colors)
{
  color_index_.clear();
  color_index_ = colors;
}

void bgui_bargraph_clipon_tableau::clear()
{
  for(vcl_vector<vgui_soview2D_lineseg*>::iterator sit = bar_plot_.begin();
      sit != bar_plot_.end();++sit)
    {
      easy_->remove(*sit);
      delete *sit;
    }
  bar_plot_.clear();
  easy_->post_redraw();
}
