// This is gel/vifa/vifa_coll_lines_params.h
#ifndef VIFA_COLL_LINES_PARAMS_H
#define VIFA_COLL_LINES_PARAMS_H

//-----------------------------------------------------------------------------
//:
// \file
// \brief Parameter mixin for collinearity tests.
//
// \author Roddy Collins, from DDB in TargetJr
//
// \date May 2001
//
// \verbatim
//  Modifications:
//   MPP Mar 2003, Ported to VXL
// \endverbatim
//-----------------------------------------------------------------------------

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vul/vul_timestamp.h>
#include <gevd/gevd_param_mixin.h>


//: Container holding parameters for collinearizing lines
class vifa_coll_lines_params : public gevd_param_mixin,
                               public vul_timestamp,
                               public vbl_ref_count
{
  float midpt_distance_;
  float angle_tolerance_;
  float discard_threshold_;
  float endpt_distance_;

 public:
  //: Distance of midpoint of qualified edge segment to projected line.
  float midpt_distance() const { return midpt_distance_; }

  //: Maximum angular difference (in degrees).
  float angle_tolerance() const { return angle_tolerance_; }

  //: Minimum of supported to spanning length.
  float discard_threshold() const { return discard_threshold_; }

  //: Endpoints within this distance are accepted no matter what the angular difference is.
  float endpt_distance() const { return endpt_distance_; }

  //: Default constructor
  vifa_coll_lines_params(float midpt_distance = 1.0f,
                         float angle_tolerance = 5.0f,
                         float discard_threshold = 0.3f,
                         float endpt_distance = 3.0f
                        );

  //: Copy constructor
  vifa_coll_lines_params(const vifa_coll_lines_params& old_params);

  //: Output contents
  void  print_info(void);
};


typedef vbl_smart_ptr<vifa_coll_lines_params>  vifa_coll_lines_params_sptr;


#endif  // VIFA_COLL_LINES_PARAMS_H
