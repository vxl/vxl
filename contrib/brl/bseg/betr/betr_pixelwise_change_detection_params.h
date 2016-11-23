#ifndef betr_pixelwise_change_detection_params_h_
#define betr_pixelwise_change_detection_params_h_
//:
// \file
// \brief Parameters for the edgel change_detection algorithm
// \author Thomas Pollard
// \date November 18, 2016
//
// method - an integer specifying the algorithm used, defined below and in
//          baml_detect_change.h
//
//  0 Census       
//  1 Birchfield-Tomasi      
//  2 Gradient difference
//  3 Non-parametric transfer function learning
//                                          
#include <string>
#include <iostream>
#include <vcl_compiler.h>
#include "betr_params.h"
#include <baml/baml_detect_change.h>
//#include <sdet/sdet_detector_params.h>

class betr_pixelwise_change_detection_params : public betr_params
{
 public:

  betr_pixelwise_change_detection_params(){
    method_list_.push_back( CENSUS );
    method_list_.push_back( BIRCHFIELD_TOMASI );
    method_list_.push_back( GRADIENT_DIFF );
    method_list_.push_back( NON_PARAMETRIC );
  }

  //: check values of parameters to flag illegal values
  virtual bool sanity_check(std::string& errors) const;

  virtual void serialize( Json::Value& root ) const{
    int method_idx = 0;
    for( int m = 0; m < method_list_.size(); m++ )
      if( pw_params_.method == method_list_[m] ) method_idx = m;
    root["method"] = method_idx;
    root["registration_rad"] = pw_params_.registration_refinement_rad;
    root["change_prior"] = pw_params_.prior_change_prob;
  }

  virtual void deserialize( Json::Value& root){
    int method_idx = std::min( 
      (int)method_list_.size(), std::max( 0, (int)root["method"].asInt() ) );
    pw_params_.method = method_list_[method_idx];
    pw_params_.registration_refinement_rad = root["registration_rad"].asInt();
    pw_params_.prior_change_prob = root["change_prior"].asFloat();
  }

  //: parameter block
  baml_change_detection_params pw_params_;

protected:

  std::vector< baml_change_detection_method > method_list_;
};

std::ostream&  operator<<(std::ostream& s, betr_pixelwise_change_detection_params const& ecdp);
std::istream&  operator>>(std::istream& s, betr_pixelwise_change_detection_params& ecdp);

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_pixelwise_change_detection_params.

