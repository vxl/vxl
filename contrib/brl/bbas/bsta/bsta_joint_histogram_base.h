// This is brl/bbas/bsta/bsta_joint_histogram_base.h
#ifndef bsta_joint_histogram_base_h_
#define bsta_joint_histogram_base_h_
//:
// \file
// \brief Abstract base class for joint histogram
// \author Joseph L. Mundy
// \date March 1, 2008
// \verbatim
//  Modifications
// \endverbatim
#include <vbl/vbl_ref_count.h>
class bsta_joint_histogram_base : public vbl_ref_count
{
 public:

 bsta_joint_histogram_base() {}
 virtual  ~bsta_joint_histogram_base() {}
};

#endif // bsta_joint_histogram_base_h_
