// This is gel/vifa/vifa_parallel_params.h
#ifndef VIFA_PARALLEL_PARAMS_H
#define VIFA_PARALLEL_PARAMS_H

//-----------------------------------------------------------------------------
//:
// \file
// \brief Parameter mixin for parallel line calculations.
//
// \author Roddy Collins, from DDB in TargetJr
//
// \verbatim
//  Modifications
//   MPP Mar 2003, Ported to VXL
// \endverbatim
//-----------------------------------------------------------------------------

#include <gevd/gevd_param_mixin.h>
#include <vul/vul_timestamp.h>
#include <vbl/vbl_ref_count.h>

#define DEFAULT_MIN_ANGLE  0.0f
#define DEFAULT_MAX_ANGLE  179.9999f
#define DEFAULT_NBUCKETS  60


class vifa_parallel_params : public gevd_param_mixin,
               public vul_timestamp,
               public vbl_ref_count
{
 public:
  float  min_angle;
  float  max_angle;
  int    nbuckets;

  //: Default constructor
  vifa_parallel_params(float  angle_min = DEFAULT_MIN_ANGLE,
                       float  angle_max = DEFAULT_MAX_ANGLE,
                       int  buckets = DEFAULT_NBUCKETS
                      );

  //: Copy constructors.
  vifa_parallel_params(const vifa_parallel_params&  np);
  vifa_parallel_params(vifa_parallel_params*  np);

  //: Output contents
  void  print_info(void);
};


#endif  // VIFA_PARALLEL_PARAMS_H
