// This is brl/bbas/bgui/bgui_histogram_tableau.cxx
#include <bgui/bgui_histogram_tableau.h>
//:
// \file
// \author  Matt Leotta

#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_vil.h>
#include <vil/vil_image_resource.h>
#include <vil/algo/vil_histogram.h>
#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_easy2D_tableau.h>

//========================================================================
//: Constructor
bgui_histogram_tableau::bgui_histogram_tableau() 
  : left_offset_(10), top_offset_(10),
    graph_width_(256), graph_height_(200), plot_(0)
{
  easy_ = vgui_easy2D_tableau_new();

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
  easy_->add_polygon(4, &x_corners[0], &y_corners[0]);
}

//========================================================================
//: Destructor.
bgui_histogram_tableau::~bgui_histogram_tableau()
{
}


void bgui_histogram_tableau::update(vil1_memory_image_of< vil1_rgb<unsigned char> >& img)
{
  vil_image_resource_sptr image = vil1_to_vil_image_resource(img);
  vil_image_view_base_sptr temp = image->get_view( 0 , image->ni() , 0 , image->nj() );
  vil_image_view<vxl_byte> img_view( temp );
  data_.clear();
  vil_histogram_byte( img_view , data_ );

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
    plot_ = easy_->add_linestrip(xscaled.size(), &xscaled[0], &yscaled[0]);

  post_redraw();
}

void bgui_histogram_tableau::update(vil_image_view< vxl_byte >& img_view)
{
  data_.clear();
  vil_histogram_byte( img_view , data_ );

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
    plot_ = easy_->add_linestrip(xscaled.size(), &xscaled[0], &yscaled[0]);

  post_redraw();
}

//========================================================================
//: Handles all events for this tableau.
bool bgui_histogram_tableau::handle(const vgui_event& event)
{
  // Pass events on down to the child tableaux:
  easy_->handle(event);
  return true;
}

void bgui_histogram_tableau::clear()
{
  easy_->remove(plot_);
  delete plot_;
  plot_ = 0;

  post_redraw();
}
