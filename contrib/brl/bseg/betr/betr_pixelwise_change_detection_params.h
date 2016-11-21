#ifndef betr_pixelwise_change_detection_params_h_
#define betr_pixelwise_change_detection_params_h_
//:
// \file
// \brief Parameters for the edgel change_detection algorithm
// \author Thomas Pollard
// \date November 18, 2016
//
// method - a text string specifying the algorithm used, defined below and in
//          baml_detect_change.h
//
//  "bt"           Birchfield-Tomasi
//  "census"       Census
//  "grad"         Gradient difference
//  "nonparam"     Non-parametric transfer function learning
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

 betr_pixelwise_change_detection_params(){}

  //: check values of parameters to flag illegal values
  virtual bool sanity_check(std::string& errors) const;

  virtual void serialize( Json::Value& root ) const{
    root["method"] = pw_params_.method_name();
  }
  virtual void deserialize( Json::Value& root){

    bool method_found = false;
    for( int m = 0; m < 4 && !method_found; m++ ){
      pw_params_.method;
      if( m == 0 ) pw_params_.method = NON_PARAMETRIC; 
      else if( m == 1 ) pw_params_.method = BIRCHFIELD_TOMASI; 
      else if( m == 2 ) pw_params_.method = GRADIENT_DIFF;
      else if( m == 3 ) pw_params_.method = CENSUS;
      std::string method_name = pw_params_.method_name();
      if( strcmp( root["method"].asCString(), method_name.c_str() )==0 ) 
        method_found = true;
    }

    if( !method_found ) std::cerr << 
      "ERROR: betr_pixelwise_change_detection_params::deserialize, invalid method string\n";
  }

  //: parameter block
  baml_change_detection_params pw_params_;

};

std::ostream&  operator<<(std::ostream& s, betr_pixelwise_change_detection_params const& ecdp);
std::istream&  operator>>(std::istream& s, betr_pixelwise_change_detection_params& ecdp);

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_pixelwise_change_detection_params.

