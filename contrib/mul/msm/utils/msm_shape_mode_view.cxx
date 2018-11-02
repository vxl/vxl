
//:
// \file
// \brief Generates shapes displaying modes of a shape model.
// \author Tim Cootes

#include <cmath>
#include <iostream>
#include <algorithm>
#include "msm_shape_mode_view.h"
#include <msm/msm_ref_shape_model.h>
#include <vsl/vsl_indent.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================
// Dflt ctor
//=======================================================================

msm_shape_mode_view::msm_shape_mode_view():
  shape_model_(nullptr),
  points_(3),
  n_per_mode_(3),
  mode_(0),
  sd_range_(3.0),
  rel_scale_(0.95),
  overlap_shapes_(false),
  display_win_(0,255,0,128)
{}


//: When true, overlap all the shapes to highlight changes
void msm_shape_mode_view::set_overlap_shapes(bool b)
{
  msm_shape_mode_view::overlap_shapes_=b;

  if (shape_model_!=nullptr)
    compute_shapes();
}

//=======================================================================
//: Define shape model
//=======================================================================

void msm_shape_mode_view::set_shape_model(const msm_ref_shape_model& sm)
{
  shape_model_ = &sm;
  sm_inst_.set_shape_model(*shape_model_);

  b_sd_ = sm.mode_var();
  for (double & i : b_sd_) i=std::sqrt(i);

  if (b_sd_.size()>0)
    set_mode(1);
  else
    set_mode(0);

  compute_shapes();
}

//: Define width of display region for a single shape
//  Height estimated from aspect ratio of mean shape
//  Width of display window is then a multiple of this,
//  depending on the number of shapes along the row.
void msm_shape_mode_view::set_display_width(double width)
{
  // Estimate aspect ratio
  vgl_box_2d<double> bounds = shape_model().mean_points().bounds();
  double asp=bounds.height()/(1e-3+bounds.width());

  int win_width;
  if (overlap_shapes_)
    win_width=int(width);
  else
    win_width=int(width*n_per_mode_);

  int win_height=int(asp*width);
  set_display_window(vgl_box_2d<int>(0,win_width, 0,win_height));
}


//: Current shape model
const msm_ref_shape_model& msm_shape_mode_view::shape_model() const
{
  assert(shape_model_!=nullptr);
  return *shape_model_;
}

//: Compute the shapes so they fit into current window
void msm_shape_mode_view::compute_shapes()
{
  compute_shapes(n_per_mode_,sd_range_,false);
}

//: Compute the shapes so they fit into current window
void msm_shape_mode_view::compute_shapes(unsigned n_shapes,
                                         double range, bool vary_one)
{
  if (mode_<=0) return;
  if (mode_>b_sd_.size()) return;

  double min_b,db;

  if (n_shapes>1)
  {
    min_b = -1.0 * range * b_sd_(mode_-1);
    db = -2.0 * min_b/(n_shapes-1);
  }
  else
  {
    if (vary_one)
    {
      min_b = 1.0 * range * b_sd_(mode_-1);
      db = 0;
    }
    else
    {
      min_b = 0.0;
      db    = 0.0;
    }
  }
  points_.resize(n_shapes);

  // First compute model shapes and largest bounding box
  vgl_box_2d<double> bbox;
  for (unsigned i=0;i<n_shapes;++i)
  {
    b_(mode_-1) = min_b + db*i;
    sm_inst_.set_params(b_);
    points_[i]=sm_inst_.points();
    if (i==0)
      bbox=points_[i].bounds();
    else
    {
      bbox.add(points_[i].bounds());
    }
  }

  double w_width;
  if (overlap_shapes_)
    w_width = display_win_.width();
  else
    w_width = display_win_.width()/n_per_mode_;
  double w_height = display_win_.height();

  // Estimate scaling required to fit into window

  // Assumes shapes have origin at CoG
//  double sw = 0.5*w_width/std::max(-bbox.min_x(),bbox.max_x());
//  double sh = 0.5*w_height/std::max(-bbox.min_y(),bbox.max_y());
  double sw = w_width/(bbox.max_x()-bbox.min_x());
  double sh = w_height/(bbox.max_y()-bbox.min_y());
  double s = std::min(sw,sh)*rel_scale_;

  // Scale each example
  for (unsigned i=0;i<n_shapes;++i)
    points_[i].scale_by(s);

  // Centre of scaled bounding box
  vgl_point_2d<double > bbox_c(bbox.centroid().x()*s,
                               bbox.centroid().y()*s);

  // Find translation to centre the box
  vgl_point_2d<double> view_centre(0.5*w_width,0.5*w_height);
  vgl_vector_2d<double> t=view_centre-bbox_c;

  // Translate each example
  // Assume each model centred on origin
  for (unsigned i=0;i<n_shapes;++i)
  {
    if (overlap_shapes_)
    {
      // Centre every example at field centre
      points_[i].translate_by(t.x(),t.y());
    }
    else
    {
      // Centre each example along a row
      points_[i].translate_by(t.x()+i*w_width,t.y());
    }
  }
}

  //: Define current display window size
void msm_shape_mode_view::set_display_window(const vgl_box_2d<int> & win)
{
  display_win_ = win;

  if (shape_model_!=nullptr)
    compute_shapes();
}

    //: Define current mode to use
void msm_shape_mode_view::set_mode(unsigned m)
{
  if (shape_model_==nullptr) return;

  if (m>=shape_model().n_modes()) m = shape_model().n_modes();
  mode_ = m;

  if (b_.size()!=mode_)
  {
    b_.set_size(mode_);
    for (unsigned i=0; i<mode_; ++i) b_(i) = 0.0;
  }

  compute_shapes();
}

  //: Define number of shapes per mode
void msm_shape_mode_view::set_n_per_mode(unsigned n)
{
  if (n_per_mode_ == n) return;
  if (n<1) return;

  n_per_mode_ = n;

  compute_shapes();
}

//: Maximum number of shape modes available
unsigned msm_shape_mode_view::max_modes() const
{
  if (shape_model_==nullptr)
    return 0;
  else
    return shape_model_->n_modes();
}

//: Set range (in units of SD)
void  msm_shape_mode_view::set_range(double r)
{
  sd_range_ = r;
  compute_shapes();
}
