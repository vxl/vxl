// This is brl/bbas/bsta/bsta_joint_histogram_3d_base.h
#ifndef bsta_joint_histogram_3d_base_h_
#define bsta_joint_histogram_3d_base_h_
//:
// \file
// \brief Abstract base class for joint_histogram_3d
// \author Joseph L. Mundy
// \date June 29, 2011
// \verbatim
//  Modifications
// \endverbatim
#include <vbl/vbl_ref_count.h>
class bsta_joint_histogram_3d_base : public vbl_ref_count
{
 public:

 bsta_joint_histogram_3d_base() {}
 virtual  ~bsta_joint_histogram_3d_base() {}
};

#endif // bsta_joint_histogram_3d_base_h_
