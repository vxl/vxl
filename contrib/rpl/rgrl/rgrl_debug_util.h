#ifndef rgrl_debug_util_h_
#define rgrl_debug_util_h_
//:
// \file
// \brief Utility functions/classes to help debug registration
// \author Gehua Yang
// \date Aug 2004

#include <iostream>
#include <string>
#include <rgrl/rgrl_command.h>
#include <rgrl/rgrl_mask_sptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: observer to view transformations at each iteration of feature-based registration engine
class rgrl_debug_feature_iteration_print: public rgrl_command
{
 public:
  void execute(rgrl_object* caller, const rgrl_event & event ) override
  {
    execute( (const rgrl_object*) caller, event );
  }

  void execute(const rgrl_object* caller, const rgrl_event & event ) override;
};


//: observer to save matches at each iteration of feature-based registration engine
class rgrl_debug_feature_iteration_save_matches: public rgrl_command
{
 protected:
  std::string path_;
  std::string file_prefix_;
  rgrl_mask_sptr from_roi_sptr_;

 public:

  //: constructor
  rgrl_debug_feature_iteration_save_matches( std::string  path,
                                             std::string  prefix,
                                             const rgrl_mask_sptr& from_roi = nullptr );

  void execute(rgrl_object* caller, const rgrl_event & event ) override
  {
    execute( (const rgrl_object*) caller, event );
  }

  void execute(const rgrl_object* caller, const rgrl_event & event ) override;
};


#endif //rgrl_debug_util_h_
