// This is brl/bmvl/brct/brct_volume_processor_params.h
#ifndef brct_volume_processor_params_h_
#define brct_volume_processor_params_h_
//:
// \file
// \brief parameter mixin for brct_volume_processor
//
// \author
//    Joseph L. Mundy - March 1, 2004
//    Brown University
//
//-----------------------------------------------------------------------------
#include <sdet/sdet_harris_detector_params.h>
#include <gevd/gevd_param_mixin.h>
#include <vcl_iosfwd.h>

class brct_volume_processor_params : public gevd_param_mixin
{
 public:
  brct_volume_processor_params(const float xmin = -600.0,
                               const float xmax = 0.0,
                               const float ymin = 0.0,
                               const float ymax = 1000.0,
                               const float zmin = -20,
                               const float zmax = 1000.0,
                               const int cube_edge_length=50,
                               const float cell_thresh =1,
                               const float dummy2 =0,
                               const float dummy3 =0);

  brct_volume_processor_params(const brct_volume_processor_params& old_params);
 ~brct_volume_processor_params(){}

  bool SanityCheck();
  friend
    vcl_ostream& operator<<(vcl_ostream&, const brct_volume_processor_params&);
 protected:
  void InitParams(float xmin,
                  float xmax,
                  float ymin,
                  float ymax,
                  float zmin,
                  float zmax,
                  int cube_edge_length,
                  float cell_thresh,
                  float dummy2,
                  float dummy3);
 public:
  //
  // Parameter blocks and parameters
  //
  float xmin_;
  float xmax_;
  float ymin_;
  float ymax_;
  float zmin_;
  float zmax_;
  int cube_edge_length_;
  float cell_thresh_;
  float dummy2_;
  float dummy3_;
};

#endif // brct_volume_processor_params_h_
