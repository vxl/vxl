#ifndef gevd_region_proc_params_h_
#define gevd_region_proc_params_h_
//:
// \file
// \brief parameter mixin for vdgl_region_proc
//
// \author
//             Joseph L. Mundy - Apr. 11, 2001
//             GE Corporate Research and Development
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <gevd/gevd_detector_params.h>
#include <vcl_iosfwd.h>

class gevd_region_proc_params;
vcl_ostream& operator<<(vcl_ostream& os, const gevd_region_proc_params& rpp);

class gevd_region_proc_params : public gevd_param_mixin
{
 public :
  gevd_region_proc_params(float expand_scale = 2.0f,
                          float burt_adelson_factor = .4f,
                          bool debug = false,
                          bool verbose = false,
                          gevd_detector_params const& = gevd_detector_params());

  gevd_region_proc_params(const gevd_region_proc_params& old_params);
  ~gevd_region_proc_params(){}

  bool SanityCheck();
  friend
    vcl_ostream& operator<<(vcl_ostream&, const gevd_region_proc_params& rpp);
 protected:
  void InitParams(float expand_scale,
                  float burt_adelson_factor,
                  bool debug,
                  bool verbose,
                  const gevd_detector_params& dp);
 public:
  //
  // Parameter blocks and parameters
  //
  float expand_scale_;       //!< the scale factor for image expansion
  float burt_adelson_factor_;//!< the "sigma" of the interpolating kernel
  bool debug_;               //!< Carry out debug processing
  bool verbose_;             //!< Print detailed output
  gevd_detector_params dp_;  //!< parameters associated with step and fold detection
};

#endif // gevd_region_proc_params_h_
