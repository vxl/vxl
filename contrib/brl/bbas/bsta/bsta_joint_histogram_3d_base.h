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
#include <vcl_compiler.h>
class bsta_joint_histogram_3d_base : public vbl_ref_count
{
 public:
  enum bsta_joint_hist_3d_type
    {
      HIST_TYPE_UNKNOWN = 0,
      HIST_TYPE_FLOAT = 1,
      HIST_TYPE_DOUBLE = 2,
      HIST_TYPE_END = 3
    };
  bsta_joint_histogram_3d_base(): type_(HIST_TYPE_UNKNOWN){}

  virtual  ~bsta_joint_histogram_3d_base() {}

  bsta_joint_hist_3d_type type_;
};

template <class T>
class bsta_joint_histogram_3d_traits
{
 public:
  static bsta_joint_histogram_3d_base::bsta_joint_hist_3d_type type(){
    return bsta_joint_histogram_3d_base::HIST_TYPE_UNKNOWN;}
};

VCL_DEFINE_SPECIALIZATION
class bsta_joint_histogram_3d_traits<float>
{
 public:
  static bsta_joint_histogram_3d_base::bsta_joint_hist_3d_type type(){
    return bsta_joint_histogram_3d_base::HIST_TYPE_FLOAT;}
};

VCL_DEFINE_SPECIALIZATION
class bsta_joint_histogram_3d_traits<double>
{
 public:
  static bsta_joint_histogram_3d_base::bsta_joint_hist_3d_type type(){
    return bsta_joint_histogram_3d_base::HIST_TYPE_DOUBLE;}
};

#endif // bsta_joint_histogram_3d_base_h_
