#ifndef sdet_region_proc_params_h_
#define sdet_region_proc_params_h_
//:
// \file
// \brief parameter mixin for vdgl_region_proc
//
// \author
//    Joseph L. Mundy - Apr. 11, 2001
//    GE Corporate Research and Development
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <sdet/sdet_detector_params.h>
#include <vcl_iosfwd.h>

class sdet_region_proc_params : public gevd_param_mixin
{
 public:
  sdet_region_proc_params(
                          const sdet_detector_params& dp = sdet_detector_params(),
                          bool verbose = true,
                          bool debug = false,
                          int array_scale = 2);

  sdet_region_proc_params(const sdet_region_proc_params& old_params);
 ~sdet_region_proc_params(){}

  bool SanityCheck();
  friend
    vcl_ostream& operator<<(vcl_ostream&, const sdet_region_proc_params& rpp);
 protected:
  void InitParams(
                  const sdet_detector_params& dp,
                  bool verbose,
                  bool debug,
                  int array_scale);
 public:
  //
  // Parameter blocks and parameters
  //
  int array_scale_;          //!< resolution of label arrays
  bool debug_;               //!< Carry out debug processing
  bool verbose_;             //!< Print detailed output
  sdet_detector_params dp_;  //!< parameters associated with step and fold detection
};

#endif // sdet_region_proc_params_h_
