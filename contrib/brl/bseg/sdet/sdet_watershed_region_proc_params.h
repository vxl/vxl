#ifndef sdet_watershed_region_proc_params_h_
#define sdet_watershed_region_proc_params_h_
//:
// \file
// \brief parameter mixin for sdet_watershed_region_proc
//
// \author
//    Joseph L. Mundy - July 11, 2004
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <brip/brip_watershed_params.h>
#include <vcl_iosfwd.h>

class sdet_watershed_region_proc_params : public gevd_param_mixin
{
 public:
  sdet_watershed_region_proc_params(
                      brip_watershed_params const& wp = brip_watershed_params(),
                      bool verbose = false,
                      bool debug = false,
                      int min_area = 5,
                      float merge_tol = 40,
                      int merge_priority = 0)
  : min_area_(min_area), merge_tol_(merge_tol), merge_priority_(merge_priority),
    debug_(debug), verbose_(verbose), wp_(wp) {}

  sdet_watershed_region_proc_params(sdet_watershed_region_proc_params const& p)
  : gevd_param_mixin(), min_area_(p.min_area_), merge_tol_(p.merge_tol_),
    merge_priority_(p.merge_priority_), debug_(p.debug_), verbose_(p.verbose_),
    wp_(p.wp_) {}

 ~sdet_watershed_region_proc_params() {}

  bool SanityCheck();
 protected:
  void InitParams(brip_watershed_params const& wp, bool verbose,
                  bool debug, int min_area, float merge_tol, int merge_priority)
  { min_area_ = min_area; merge_tol_ = merge_tol; merge_priority_ =
    merge_priority; debug_ = debug; verbose_ = verbose; wp_ = wp; }

 public:
  //
  // Parameter blocks and parameters
  //
  unsigned int min_area_; //!< minimum region area
  float merge_tol_;       //!< region merge tolerance
  int merge_priority_;    //!< order for region merge
  bool debug_;            //!< Carry out debug processing
  bool verbose_;          //!< Print detailed output
  brip_watershed_params wp_; //!< params associated with step and fold detection
};

vcl_ostream& operator<<(vcl_ostream&, sdet_watershed_region_proc_params const&);

#endif // sdet_watershed_region_proc_params_h_
