#ifndef sdet_vehicle_finder_params_h_
#define sdet_vehicle_finder_params_h_
//:
// \file
// \brief parameter mixin for sdet_vehicle_finder
//
// \author
//    Joseph L. Mundy - July 11, 2004
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <sdet/sdet_watershed_region_proc_params.h>
#include <brip/brip_para_cvrg_params.h>
#include <vcl_iosfwd.h>

class sdet_vehicle_finder_params : public gevd_param_mixin
{
 public:
  sdet_vehicle_finder_params(const sdet_watershed_region_proc_params& wrpp = sdet_watershed_region_proc_params(),
                             const brip_para_cvrg_params& pcp = brip_para_cvrg_params(),
                             bool verbose = false,
                             bool debug = false,
                             float search_radius = 50,
                             float shadow_thresh = 80,
                             float para_thresh = 100,
                             float distance_scale = 5);

  sdet_vehicle_finder_params(const sdet_vehicle_finder_params& old_params);
 ~sdet_vehicle_finder_params(){}

  bool SanityCheck();
  friend
    vcl_ostream& operator<<(vcl_ostream&,const sdet_vehicle_finder_params& vfp);
 protected:
  void InitParams(const sdet_watershed_region_proc_params& wrpp,
                  const brip_para_cvrg_params& pcp,
                  bool verbose,
                  bool debug,
                  float search_radius,
                  float shadow_thresh,
                  float para_thresh,
                  float distance_scale);
 public:
  //
  // Parameter blocks and parameters
  //
  float search_radius_;          //!< defines vehicle search box
  float shadow_thresh_;          //!< defines shadow threshold
  float para_thresh_;          //!< defines parallel coverage threshold
  bool debug_;               //!< Carry out debug processing
  float distance_scale_;     //!< Distance relative to region diameter
  bool verbose_;             //!< Print detailed output
  sdet_watershed_region_proc_params wrpp_;  //!< watershed region proc parameters
  brip_para_cvrg_params pcp_;  //!< parameters associated with step and fold detection
};

#endif // sdet_vehicle_finder_params_h_
