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
  sdet_vehicle_finder_params(sdet_watershed_region_proc_params const& wrpp = sdet_watershed_region_proc_params(),
                             brip_para_cvrg_params const& pcp = brip_para_cvrg_params(),
                             bool verbose = false,
                             bool debug = false,
                             float search_radius = 50,
                             float shadow_thresh = 80,
                             float para_thresh = 100,
                             float distance_scale = 5)
    : search_radius_(search_radius), shadow_thresh_(shadow_thresh),
      para_thresh_(para_thresh), debug_(debug), distance_scale_(distance_scale),
      verbose_(verbose), wrpp_(wrpp), pcp_(pcp) {}

  sdet_vehicle_finder_params(sdet_vehicle_finder_params const& p)
    : gevd_param_mixin(), search_radius_(p.search_radius_),
      shadow_thresh_(p.shadow_thresh_), para_thresh_(p.para_thresh_),
      debug_(p.debug_), distance_scale_(p.distance_scale_),
      verbose_(p.verbose_), wrpp_(p.wrpp_), pcp_(p.pcp_) {}

 ~sdet_vehicle_finder_params() {}

  bool SanityCheck();
 protected:
  void InitParams(sdet_watershed_region_proc_params const& wrpp,
                  brip_para_cvrg_params const& pcp,
                  bool verbose,
                  bool debug,
                  float search_radius,
                  float shadow_thresh,
                  float para_thresh,
                  float distance_scale)
  { search_radius_ = search_radius; shadow_thresh_ = shadow_thresh;
    para_thresh_ = para_thresh; distance_scale_ = distance_scale;
    debug_ = debug; verbose_ = verbose; wrpp_ = wrpp; pcp_ = pcp; }

 public:
  //
  // Parameter blocks and parameters
  //
  float search_radius_;      //!< defines vehicle search box
  float shadow_thresh_;      //!< defines shadow threshold
  float para_thresh_;        //!< defines parallel coverage threshold
  bool debug_;               //!< Carry out debug processing
  float distance_scale_;     //!< Distance relative to region diameter
  bool verbose_;             //!< Print detailed output
  sdet_watershed_region_proc_params wrpp_; //!< watershed region proc parameters
  brip_para_cvrg_params pcp_;//!< params associated with step and fold detection
};

vcl_ostream& operator<<(vcl_ostream&, sdet_vehicle_finder_params const&);

#endif // sdet_vehicle_finder_params_h_
