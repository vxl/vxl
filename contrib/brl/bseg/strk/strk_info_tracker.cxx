// This is brl/bseg/strk/strk_info_tracker.cxx
#include "strk_info_tracker.h"
//:
// \file
#include <vcl_algorithm.h>
#include <vul/vul_timer.h>
#include <vil1/vil1_memory_image_of.h>
#include <brip/brip_float_ops.h>
#include <strk/strk_tracking_face_2d.h>

//Gives a sort on mutual information decreasing order
static bool info_compare(strk_tracking_face_2d_sptr const f1,
                         strk_tracking_face_2d_sptr const f2)


{
  return f1->total_info() > f2->total_info();//JLM Switched
}


//---------------------------------------------------------------
// Constructors
//
//----------------------------------------------------------------

//: constructor from a parameter block (the only way)
//
strk_info_tracker::strk_info_tracker(strk_info_tracker_params& tp)
  : strk_info_tracker_params(tp)
{
//  model_intensity_hist_=0;
  model_intensity_entropy_=0;
 // model_gradient_dir_hist_=0;
  model_gradient_dir_entropy_=0;
}

//:Default Destructor
strk_info_tracker::~strk_info_tracker()
{
//  if (model_intensity_hist_)
//    delete model_intensity_hist_;
 // if (model_gradient_dir_hist_)
 //   delete model_gradient_dir_hist_;
}

//-------------------------------------------------------------------------
//: Set the previous frame image
//
void strk_info_tracker::set_image_0(vil1_image& image)
{
  if (!image)
    {
      vcl_cout <<"In strk_info_tracker::set_image_i(.) - null input\n";
      return;
    }

  vil1_memory_image_of<float> flt=brip_float_ops::convert_to_float(image);

  image_0_= brip_float_ops::gaussian(flt, sigma_);

  int w = image_0_.width(), h = image_0_.height();
  Ix_0_.resize(w,h);
  Iy_0_.resize(w,h);
  brip_float_ops::gradient_3x3(image_0_, Ix_0_, Iy_0_);
}

//-------------------------------------------------------------------------
//: Set the next frame image
//
void strk_info_tracker::set_image_i(vil1_image& image)
{
  if (!image)
    {
      vcl_cout <<"In strk_info_tracker::set_image_i(.) - null input\n";
      return;
    }

  vil1_memory_image_of<float> flt=brip_float_ops::convert_to_float(image);

  image_i_ = brip_float_ops::gaussian(flt, sigma_);
  int w = image_i_.width(), h = image_i_.height();
  Ix_i_.resize(w,h);
  Iy_i_.resize(w,h);
  brip_float_ops::gradient_3x3(image_i_, Ix_i_, Iy_i_);
}

//--------------------------------------------------------------------------
//: Set the initial model position
void strk_info_tracker::set_initial_model(vtol_face_2d_sptr const& face)
{
  initial_model_ = face;
}

//--------------------------------------------------------------------------
//: Initialize the info_tracker
void strk_info_tracker::init()
{
  if (!image_0_)
    return;
  if (!initial_model_)
    return;
  strk_tracking_face_2d_sptr tf;
  if (gradient_info_)
    tf = new strk_tracking_face_2d(initial_model_, image_0_,
                                   Ix_0_, Iy_0_);
  else
    tf = new strk_tracking_face_2d(initial_model_, image_0_);
  current_samples_.push_back(tf);
}

//--------------------------------------------------------------------------
//: generate a randomly positioned augmented face
strk_tracking_face_2d_sptr strk_info_tracker::
generate_randomly_positioned_sample(strk_tracking_face_2d_sptr const& seed)
{
  if (!seed)
    return 0;
  //random sample of uniform distribution
  float tx = (2.0*search_radius_)*(rand()/(RAND_MAX+1.0)) - search_radius_;
  float ty = (2.0*search_radius_)*(rand()/(RAND_MAX+1.0)) - search_radius_;
  float theta = (2.0*angle_range_)*(rand()/(RAND_MAX+1.0)) - angle_range_;
  float s = (2.0*scale_range_)*(rand()/(RAND_MAX+1.0)) - scale_range_;
  float scale = 1+s;
  strk_tracking_face_2d* tf = new strk_tracking_face_2d(seed);
  tf->transform(tx, ty, theta, scale);
  return tf;
}

//--------------------------------------------------------------------------
//: generate a random set of new faces from the existing samples
void strk_info_tracker::generate_samples()
{
  vul_timer t;
  for (vcl_vector<strk_tracking_face_2d_sptr>::iterator fit =
       current_samples_.begin(); fit != current_samples_.end(); fit++)
    for (int i = 0; i<n_samples_; i++)
      {
        strk_tracking_face_2d_sptr tf;
        tf = this->generate_randomly_positioned_sample(*fit);
        if (!tf)
          continue;
        tf->compute_mutual_information(image_i_, Ix_i_, Iy_i_);
        hypothesized_samples_.push_back(tf);
      }

  //sort the hypotheses
  vcl_sort(hypothesized_samples_.begin(),
           hypothesized_samples_.end(), info_compare);
}

bool strk_info_tracker::refresh_sample()
{
  float t = rand()/(RAND_MAX+1.0);
  return t<=frac_time_samples_;
}

strk_tracking_face_2d_sptr strk_info_tracker::
clone_and_refresh_data(strk_tracking_face_2d_sptr const& sample)
{
  if (!sample)
    return (strk_tracking_face_2d*)0;
  vtol_face_2d_sptr f =
    sample->face()->cast_to_face_2d();
  strk_tracking_face_2d_sptr tf;
  if (gradient_info_)
    tf = new strk_tracking_face_2d(f, image_0_,
                                   Ix_0_, Iy_0_);
  else
    tf = new strk_tracking_face_2d(f, image_0_);
  return tf;
}

//--------------------------------------------------------------------------
//: cull out the best N hypothesized samples to become the current samples
void strk_info_tracker::cull_samples()
{
  current_samples_.clear();
  for (int i =0; i<n_samples_; i++)
    current_samples_.push_back(hypothesized_samples_[i]);
      //     vcl_cout << "I[" << i << "]= " << hypothesized_samples_[i]->total_info() << "\n";
      //     vcl_cout << vcl_flush;

  if (verbose_)
    vcl_cout << "Total Inf = " << hypothesized_samples_[0]->total_info()
             << " = IntInfo(" <<  hypothesized_samples_[0]->int_mutual_info()
             << ") + GradInfo(" <<  hypothesized_samples_[0]->grad_mutual_info()
             << ")\n";

  hypothesized_samples_.clear();
  //save track history
  strk_tracking_face_2d_sptr refreshed_best =
    clone_and_refresh_data(current_samples_[0]);
  track_history_.push_back(refreshed_best);
}

#if 0
//--------------------------------------------------------------------------
//: Use parabolic interpolation to refine the transform for the best sample
//
void strk_info_tracker::refine_best_sample()
{
  strk_tracking_face_2d_sptr btf = current_samples_[0];
  //translation first
  double idtx = 0, idty = 0, idth = 0, ids = 0;
  double idtx_max=0, idty_max=0, inf_max= btf->total_info();
  double inf = 0;
  double xrange = 0, yrange = 0;
  vcl_cout << "Initial Info " << inf_max << "\n";
  double range_fraction = 0.1;
  if (false)
    //  if (initial_model_)
    {
      vtol_face* f = (vtol_face_2d*)initial_model_->cast_to_face();
      vtol_topology_object* to = (vtol_topology_object*)f;
      vsol_spatial_object_2d* so = (vsol_spatial_object_2d*)to;
      vsol_box_2d_sptr bb = so->get_bounding_box();
      if (bb)
        {
          xrange = range_fraction*bb->width();
          yrange = range_fraction*bb->height();
        }
    }
  double dx = (2*xrange)/n_samples_, dy = (2*yrange)/n_samples_;
  vcl_cout << "X,Y range(" << xrange << " " << yrange << ")\n";
  strk_tracking_face_2d_sptr tf = new strk_tracking_face_2d(btf);
  for (double ty = -yrange; ty<=yrange; ty+=dy)
    for (double tx = -xrange; tx<=xrange;tx+=dx)
      {
        if (tx||ty)
          {
            tf->transform(tx, ty, 0, 1.0);
            this->mutual_info_face(tf);
            inf = tf->total_info();
            //            vcl_cout << "(" << tx << " " << ty  << ")=" << inf << "\n";
            tf->transform(-tx, -ty, 0, 1.0);
            if (inf>inf_max)
              {
                inf_max = inf;
                idtx_max = tx;
                idty_max = ty;
              }
          }
      }
  idtx = idtx_max;
  idty = idty_max;
//   float dtx = 0, dty = 0, dth = 0, ds = 0;
//   double bgtx=0, bgty=0, bgth=0, bgsc=1.0;
//   double gtx=0, gty=0, gth=0, gsc=1.0;
//   baf->global_transform(bgtx, bgty, bgth, bgsc);
//   strk_quadratic_interpolator qtrans;
//   // strk_parabolic_interpolator pith;
//   //   strk_parabolic_interpolator pisc;
//   for (int i = 0; i<search_pattern.size(); i++)
//     {
//       strk_tracking_face_2d_sptr af = search_pattern[i];
//       if (!af)
//         continue;
//       af->global_transform(gtx, gty, gth, gsc);
//       dtx = gtx-bgtx; dty = gty-bgty;
//       //       dth = gth-bgth;
//       //       ds = gsc/bgsc-1.0;
//       double inf = af->total_info();
//       qtrans.add_data_point(dtx,dty,inf);
//       //       pith.add_data_point(dth, inf);
//       //       pisc.add_data_point(ds, inf);
//     }

//   if (search_radius_&&qtrans.solve())
//     qtrans.extremum(idtx, idty);
  //limit the interpolated value
  //   if (vcl_fabs(idtx)>search_radius_)
  //     idtx = 0;
  //   if (vcl_fabs(idty)>search_radius_)
  //     idty = 0;
  //   if (vcl_fabs(idth)>angle_range_)
  //     idth = 0;
  //   if (vcl_fabs(ids)>scale_range_)
  //     ids = 0;
  btf->transform(idtx, idty, idth, 1.0+ids);
  this->mutual_info_face(btf);
  double info = btf->total_info();
  vcl_cout << "Final dtrans (" << idtx << " " << idty
           << " " << idth << " " << ids << ") = " << info
           <<  "\n" << vcl_flush;
}
#endif

//--------------------------------------------------------------------------
//: because of sorting, the best sample will be the first current sample
vtol_face_2d_sptr strk_info_tracker::get_best_sample()
{
  if (!current_samples_.size())
    return 0;
  //  this->refine_best_sample();
  return current_samples_[0]->face()->cast_to_face_2d();
}

//--------------------------------------------------------------------------
//: because of sorting the samples will be in descending order of mutual info
void strk_info_tracker::get_samples(vcl_vector<vtol_face_2d_sptr>& samples)
{
  samples.clear();
  for (vcl_vector<strk_tracking_face_2d_sptr>::iterator
       fit = current_samples_.begin(); fit != current_samples_.end(); fit++)
    samples.push_back((*fit)->face()->cast_to_face_2d());
}

//--------------------------------------------------------------------------
//: Main tracking method
void strk_info_tracker::track()
{
  vul_timer t;
  this->generate_samples();
  if (verbose_)
    vcl_cout << "Samples generated " << t.real() << " msecs.\n";
  this->cull_samples();
}

void strk_info_tracker::clear()
{
  current_samples_.clear();
  hypothesized_samples_.clear();
}
