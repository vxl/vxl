// This is brl/bseg/strk/strk_info_model_tracker.cxx
#include "strk_info_model_tracker.h"
//:
// \file
#include <vcl_algorithm.h>
#include <vul/vul_timer.h>
#include <vil1/vil1_memory_image_of.h>
#include <vsol/vsol_point_2d.h>
#include <btol/btol_face_algs.h>
#include <brip/brip_vil1_float_ops.h>
#include <strk/strk_art_info_model.h>
#include <strk/strk_tracking_face_2d.h>


//Gives a sort on mutual information
static bool info_compare(strk_art_info_model_sptr const m1,
                         strk_art_info_model_sptr const m2)
{
  return m1->total_model_info() > m2->total_model_info();//JLM Switched
}


//---------------------------------------------------------------
// Constructors
//
//----------------------------------------------------------------

//: constructor from a parameter block (the only way)
//
strk_info_model_tracker::strk_info_model_tracker(strk_info_model_tracker_params& tp)
  : strk_info_model_tracker_params(tp)
{
}

//:Default Destructor
strk_info_model_tracker::~strk_info_model_tracker()
{
}

//-------------------------------------------------------------------------
//: Set the previous frame image
//
void strk_info_model_tracker::set_image_0(vil1_image& image)
{
  if (!image)
    {
      vcl_cout <<"In strk_info_model_tracker::set_image_i(.) - null input\n";
      return;
    }

  vil1_memory_image_of<float> flt=brip_vil1_float_ops::convert_to_float(image);

  image_0_= brip_vil1_float_ops::gaussian(flt, sigma_);

  int w = image_0_.width(), h = image_0_.height();
  Ix_0_.resize(w,h);
  Iy_0_.resize(w,h);
  brip_vil1_float_ops::gradient_3x3(image_0_, Ix_0_, Iy_0_);
}

//-------------------------------------------------------------------------
//: Set the next frame image
//
void strk_info_model_tracker::set_image_i(vil1_image& image)
{
  if (!image)
    {
      vcl_cout <<"In strk_info_model_tracker::set_image_i(.) - null input\n";
      return;
    }

  vil1_memory_image_of<float> flt=brip_vil1_float_ops::convert_to_float(image);

  image_i_ = brip_vil1_float_ops::gaussian(flt, sigma_);
  int w = image_i_.width(), h = image_i_.height();
  Ix_i_.resize(w,h);
  Iy_i_.resize(w,h);
  brip_vil1_float_ops::gradient_3x3(image_i_, Ix_i_, Iy_i_);
}


//--------------------------------------------------------------------------
//: Initialize the info_tracker
void strk_info_model_tracker::init()
{
  if (!image_0_)
    return;
  if (!initial_model_.size())
    return;
//for now use the centroid of the first face
  vsol_point_2d_sptr pivot = btol_face_algs::centroid(initial_model_[0]);
  strk_art_info_model_sptr mod;
  if (gradient_info_)
    mod = new strk_art_info_model(initial_model_,pivot, image_0_, Ix_0_, Iy_0_);
  else
    mod = new strk_art_info_model(initial_model_,pivot, image_0_);
  current_samples_.push_back(mod);
}

static float rand_val(float range)
{
  return float((2.0f*range)*(rand()/(RAND_MAX+1.0f)) - range);
}

//--------------------------------------------------------------------------
//: generate a randomly positioned articulated model
strk_art_info_model_sptr
strk_info_model_tracker::generate_model(strk_art_info_model_sptr const& seed)
{
  if (!seed)
    return 0;
  //random sample of uniform distribution
  float stem_tx = rand_val(stem_trans_radius_);
  float stem_ty = rand_val(stem_trans_radius_);
  float stem_angle = rand_val(stem_angle_range_);
  float long_arm_angle = rand_val(long_arm_angle_range_);
  float short_arm_angle = rand_val(short_arm_angle_range_);
  float long_arm_tip_angle = rand_val(long_arm_tip_angle_range_);
  float short_arm_tip_angle = rand_val(short_arm_tip_angle_range_);
  strk_art_info_model_sptr mod = new strk_art_info_model(seed);
  if (!mod->transform(stem_tx, stem_ty, stem_angle, long_arm_angle,
                      short_arm_angle, long_arm_tip_angle, short_arm_tip_angle))
    return (strk_art_info_model*)0;
  else
    return mod;
}

//--------------------------------------------------------------------------
//: generate a random set of new faces from the existing samples
void strk_info_model_tracker::generate_samples()
{
  vul_timer t;
  for (vcl_vector<strk_art_info_model_sptr>::iterator mit =
       current_samples_.begin(); mit != current_samples_.end(); mit++)
    for (int i = 0; i<n_samples_; i++)
    {
      strk_art_info_model_sptr mod =
        this->generate_model(*mit);
      if (!mod)
      {
        vcl_cout << "In strk_info_model_tracker::generate_samples() -"
                 << " generate_model failed\n";
        continue;
      }
      if (gradient_info_)
        mod->compute_mutual_information(image_i_, Ix_i_, Iy_i_);
      else
        mod->compute_mutual_information(image_i_);
      hypothesized_samples_.push_back(mod);
    }

  //sort the hypotheses
  vcl_sort(hypothesized_samples_.begin(),
           hypothesized_samples_.end(), info_compare);
}

//--------------------------------------------------------------------------
//: cull out the best N hypothesized samples to become the current samples
void strk_info_model_tracker::cull_samples()
{
  current_samples_.clear();
  for (int i =0; i<n_samples_; i++)
  {
    current_samples_.push_back(hypothesized_samples_[i]);
  }
  if (verbose_)
    vcl_cout << "Total Inf = " << hypothesized_samples_[0]->total_model_info()
             << ")\n";
  hypothesized_samples_.clear();
}

//--------------------------------------------------------------------------
//: because of sorting, the best sample will be the first current sample
strk_art_info_model_sptr strk_info_model_tracker::get_best_sample()
{
  if (!current_samples_.size())
    return 0;
  return current_samples_[0];
}

//--------------------------------------------------------------------------
//: because of sorting the samples will be in descending order of mutual info
void strk_info_model_tracker::get_samples(vcl_vector<strk_art_info_model_sptr>& samples)
{
  samples.clear();
  for (vcl_vector<strk_art_info_model_sptr>::iterator
       mit = current_samples_.begin(); mit != current_samples_.end(); mit++)
    samples.push_back(*mit);
}

//--------------------------------------------------------------------------
//: Main tracking method
void strk_info_model_tracker::track()
{
  vul_timer t;
  this->generate_samples();
  if (verbose_)
    vcl_cout << "Samples generated " << t.real() << " msecs.\n";
  this->cull_samples();
}

void strk_info_model_tracker::clear()
{
  current_samples_.clear();
  hypothesized_samples_.clear();
}
