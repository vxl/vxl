// This is algo/bapl/bapl_mi_matcher.cxx
//:
// \file

#include "bapl_mi_matcher.h"
#include <bapl/bapl_affine_roi.h>
#include <bapl/bapl_affine_transform.h>
#include <vcl_algorithm.h>
#include <vcl_cmath.h>
#include <brip/brip_histogram.h>
#include <brip/brip_mutual_info.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_2x2.h>
#include <vnl/vnl_math.h>


//: Constructor
bapl_mi_matcher::bapl_mi_matcher(vil_image_view<vxl_byte> src_image,
                                 vil_image_view<vxl_byte> target_patch,
                                 bapl_affine_transform init_xform,
                                 bapl_mi_matcher_params params)
  :  src_img_(src_image), tgt_img_(target_patch), tgt_entropy_(0.0),
     init_xform_(init_xform), params_(params)
{
  // Calculate the entropy of the target image
  vcl_vector<double> histogram;
  double sum = brip_histogram(tgt_img_, histogram, 0, 255, 16);
  tgt_entropy_ = brip_hist_entropy(histogram, sum);
}


  
//: Constructor
bapl_mi_matcher::bapl_mi_matcher(vil_image_view<vxl_byte> src_image,
                                 bapl_affine_roi& roi,
                                 bapl_mi_matcher_params params)
  :  src_img_(src_image), tgt_img_(roi.rectified_image()), tgt_entropy_(0.0),
     init_xform_(roi.xform()), params_(params)
{
  // Calculate the entropy of the target image
  vcl_vector<double> histogram;
  double sum = brip_histogram(tgt_img_, histogram, 0, 255, 16);
  tgt_entropy_ = brip_hist_entropy(histogram, sum);
}


//: Generate random candidate matches
void
bapl_mi_matcher::generate()
{
  double mi;
  vcl_vector<bapl_match> hypotheses;
  // if this is the first iteration, initialize with the initial transformation
  if(matches_.empty()){
    mi = mutual_info(init_xform_);
    matches_.push_back(bapl_mi_matcher::bapl_match(mi,init_xform_));
  }

  for( vcl_vector<bapl_match>::iterator m_itr = matches_.begin();
       m_itr != matches_.end(); ++m_itr){
    // hypothesize that the transformation remains unchanged
    hypotheses.push_back(*m_itr);
    // make other random hypotheses
    for(unsigned i=0; i<params_.num_samples_; ++i){
      bapl_affine_transform T = this->rand_transform();
      double mi = mutual_info(T);
      hypotheses.push_back(bapl_mi_matcher::bapl_match(mi,T));
    }
  }
  // sort in order of decreasing mutual info
  vcl_sort(hypotheses.begin(), hypotheses.end());

  matches_.clear();
  for(unsigned i=0; i<params_.num_samples_; ++i){
    matches_.push_back(hypotheses[i]);
    vcl_cout << "MI" << i << ": " << hypotheses[i].mut_info << " - T: "
             << hypotheses[i].xform.t() << " - A: "<< hypotheses[i].xform.A() << vcl_endl; 
  }
}


//: Return the transformation of the best match
bapl_affine_transform
bapl_mi_matcher::best_xform()
{
  return matches_.front().xform;
}


// ---------------------------------------------------------------------------------
// Private Functions
// ---------------------------------------------------------------------------------

//: Generate random doubles between -1.0 and 1.0
inline double rand_double() { return 2.0*rand()/(double(RAND_MAX)) -1.0; }

//: Generate a random transform (close to init_xform_)
bapl_affine_transform 
bapl_mi_matcher::rand_transform()
{
  double angle = params_.max_rotation_ang_*rand_double()*vnl_math::pi/180.0;
  double sin_ang = vcl_sin(angle);
  double cos_ang = vcl_cos(angle);
  double shear = vcl_tan(params_.max_shear_ang_*rand_double()*vnl_math::pi/180.0);
  double scale_x = vcl_pow(params_.max_sx_,rand_double());
  double scale_y = vcl_pow(params_.max_sy_,rand_double());

  vnl_double_2x2 R;
  R(0,0) = (cos_ang + shear*sin_ang)*scale_x;  R(0,1) = (-sin_ang + shear*cos_ang)*scale_x;
  R(1,0) = sin_ang*scale_y;  R(1,1) = cos_ang*scale_y;

  vnl_double_2 offset(params_.max_tx_*rand_double(), params_.max_ty_*rand_double());

  offset = 5.0 - offset;
  return bapl_affine_transform(init_xform_.A()*R, init_xform_.t()+offset);
}


//: Calculate the mutual information for the image at the transformed location
double 
bapl_mi_matcher::mutual_info(const bapl_affine_transform& T)
{
  bapl_affine_roi roi(src_img_, T, tgt_img_.ni(), tgt_img_.nj());

  vcl_vector<double> histogram;
  double match_sum = brip_histogram(roi.rectified_image(), histogram, 0, 255, 16);
  double match_entropy = brip_hist_entropy(histogram, match_sum);

  vcl_vector<vcl_vector<double> > joint_histogram;
  double joint_sum = brip_joint_histogram(tgt_img_, roi.rectified_image(), 
                                            joint_histogram, 0, 255, 16);
  double joint_entropy = brip_hist_entropy(joint_histogram, joint_sum);

  return tgt_entropy_ + match_entropy - joint_entropy;
}


