//:
// \file
#include <bdgl/bdgl_curve_description.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vdgl/vdgl_digital_curve.h>
#include <bdgl/bdgl_curve_algs.h>

//: Constructor
bdgl_curve_description::bdgl_curve_description()
{
  center_.set(0.0, 0.0);
  curvature_ = 0.0;
  length_ = 0.0;
  gradient_mean_val_ = 0.0;
  gradient_mean_dir_ = 0.0;
  gradient_std_val_ = 0.0;
  gradient_std_dir_ = 0.0;
}


//: Constructor : compute description from a digital curve and a given order of description
void bdgl_curve_description::init(vdgl_edgel_chain_sptr const& ec)
{
  if (!ec)
  {
    vcl_cout<<"In bdgl_curve_description::bdgl_curve_description(..) - warning, null chain\n";
    return;
  }

  unsigned int N = ec->size();
  coarser_points_.clear();
  points_.clear(); angles_.clear(); grad_.clear();
  // means
  center_.set(0.0, 0.0);
  gradient_mean_val_=0.0;
  gradient_mean_dir_=0.0;
  int j=0;
  for (unsigned int i=0; i<N; ++i)
  {
    vdgl_edgel ed = ec->edgel(i);
    center_.set(center_.x() + ed.get_x(), center_.y() + ed.get_y());
    gradient_mean_val_ += ed.get_grad();
    gradient_mean_dir_ += ed.get_theta();
    points_.push_back(vgl_point_2d<double>(ed.get_x(),ed.get_y()));
    angles_.push_back(ed.get_theta());
    grad_.push_back(ed.get_grad());
    if (j%10==0)
      coarser_points_.push_back(vgl_point_2d<double>(ed.get_x(),ed.get_y()));
    j++;
    //vcl_cout<<"grad:("<<ed.get_grad()<<", "<<ed.get_theta()<<")\n";
  }
  center_.set( center_.x()/double(N) , center_.y()/double(N) );
  gradient_mean_val_ /= double(N);
  gradient_mean_dir_ /= double(N);
  bdgl_curve_algs::smooth_curve(points_,1);
  points_.erase(points_.begin());
  points_.erase(points_.begin());
  points_.erase(points_.begin());

  points_.erase(points_.end()-1);
  points_.erase(points_.end()-1);
  points_.erase(points_.end()-1);


  // standard deviations
  gradient_std_val_ = 0.0;
  gradient_std_dir_ = 0.0;
  for (unsigned int i=0; i<N; ++i)
  {
    vdgl_edgel ed = ec->edgel(i);
    gradient_std_val_ += vnl_math_sqr(ed.get_grad()-gradient_mean_val_);
    gradient_std_dir_ += vnl_math_sqr(ed.get_theta()-gradient_mean_dir_);
  }
  if (N>1)
  {
    gradient_std_val_ = vcl_sqrt(gradient_std_val_/double(N-1));
    gradient_std_dir_ = vcl_sqrt(gradient_std_dir_/double(N-1));
  }

  // length
  length_=0.0;
  for (unsigned int i=1; i<N; ++i)
  {
    vdgl_edgel ed = ec->edgel(i);
    vdgl_edgel ep = ec->edgel(i-1);
    length_ += vcl_sqrt( (ed.x() - ep.x())*(ed.x() - ep.x()) + (ed.y() - ep.y())*(ed.y() - ep.y()) );
  }
  // curvature estimate
  curvature_ = 0.0;
  for (unsigned int i=1; i+1<N; ++i)
  {
    vdgl_edgel ed = ec->edgel(i);
    vdgl_edgel ep = ec->edgel(i-1);
    vdgl_edgel en = ec->edgel(i+1);

    // sort of curvature function ... to be properly defined later
    double curv = ( 2.0*ed.x() - ep.x() - en.x() )
                 *( 2.0*ed.x() - ep.x() - en.x() )
                + ( 2.0*ed.y() - ep.y() - en.y() )
                 *( 2.0*ed.y() - ep.y() - en.y() );
    //vcl_cout<<curv<<'\n';
    curvature_ += vcl_sqrt(curv);
  }
  if (N>2)
    curvature_ /= double(N-2);

  return;
}

// text description
void bdgl_curve_description::info()
{
  vcl_cout<<"curve: c ("<<center_.x()<<", "<<center_.y()<<") | l "<<length_
          <<" | grad "<<gradient_mean_val_<<" th "<<gradient_mean_dir_
          <<" | dg "<<gradient_std_val_<<" dth "<<gradient_std_dir_
          <<" | curv "<<curvature_<<'\n';
  return;
}

void bdgl_curve_description::compute_bounding_box(vdgl_edgel_chain_sptr const& ec)
{
  vdgl_digital_curve_sptr dc = new vdgl_digital_curve(new vdgl_interpolator_linear(ec));
  dc->compute_bounding_box();
  box_=dc->get_bounding_box();
}
