// This is gel/vifa/vifa_group_pgram_params.h
#ifndef VIFA_GROUP_PGRAM_PARAMS_H
#define VIFA_GROUP_PGRAM_PARAMS_H

//-----------------------------------------------------------------------------
//:
// \file
// \brief Parameter mixin for grouping parallel line sets.
//
// \author Joesph L. Mundy
//
// \date December 1998
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


//: Container holding parameters for grouping polygons
class vifa_group_pgram_params : public gevd_param_mixin,
                                public vul_timestamp,
                                public vbl_ref_count
{
 public:
  //: Granularity in the angle bins.
  float  _angle_increment;

  //: Radius of maximum suppression kernel.
  int    _max_suppress_radius;

  //: Relative length of sorter side of parallelogram.
  float  _length_ratio;

 public:
  //: Default constructor.
  vifa_group_pgram_params(float  angle_increment = 5.0,
                          int    max_suppress_radius = 2,
                          float  length_ratio = .1
                         );

  //: Copy constructor.
  vifa_group_pgram_params(const vifa_group_pgram_params&  old_params);

  //: Check that parameters are within acceptable bounds.
  bool  SanityCheck();

  //: Output contents
  void  print_info(void);

 protected:
  //: Assign internal parameter blocks.
  void  init_params(float  angle_increment,
                    int    max_supress_radius,
                    float  length_ratio
                   );
};

typedef vbl_smart_ptr<vifa_group_pgram_params>  vifa_group_pgram_params_sptr;


#endif  // VIFA_GROUP_PGRAM_PARAMS_H
