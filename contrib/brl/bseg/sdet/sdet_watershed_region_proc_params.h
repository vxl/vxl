#ifndef sdet_watershed_region_proc_params_h_
#define sdet_watershed_region_proc_params_h_
//:
// \file
// \brief parameter mixin for sdet_watershed_region_proc
//
// \author
//    Joseph L. Mundy - July 11, 2004
//    
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <brip/brip_watershed_params.h>
#include <vcl_iostream.h>

class sdet_watershed_region_proc_params : public gevd_param_mixin
{
 public:
  sdet_watershed_region_proc_params(
                          const brip_watershed_params& wp = brip_watershed_params(),
                          bool verbose = false,
                          bool debug = false,
                          int min_area = 5,
                          float merge_tol = 40,
                          int merge_priority = 0
                         );

 sdet_watershed_region_proc_params(const sdet_watershed_region_proc_params& old_params);
 ~sdet_watershed_region_proc_params(){}

  bool SanityCheck();
 friend
   vcl_ostream& operator<<(vcl_ostream& os, const sdet_watershed_region_proc_params& rpp);
 protected:
 void InitParams(
                 const brip_watershed_params& wp,
                 bool verbose,
                 bool debug,
                 int min_area,
                 float merge_tol,
                 int merge_priority
                );
 public:
  //
  // Parameter blocks and parameters
  //
  int min_area_;          //!< minimum region area
  float merge_tol_;       //!< region merge tolerance
  int merge_priority_;    //!< order for region merge
  bool debug_;               //!< Carry out debug processing
  bool verbose_;             //!< Print detailed output
  brip_watershed_params wp_;  //!< parameters associated with step and fold detection
};


#endif // sdet_watershed_region_proc_params_h_
