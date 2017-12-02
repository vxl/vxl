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
#include <vcl_compiler.h>
class bsta_joint_histogram_base : public vbl_ref_count
{
 public:
  enum bsta_joint_hist_type
    {
      HIST_TYPE_UNKNOWN = 0,
      HIST_TYPE_FLOAT = 1,
      HIST_TYPE_DOUBLE = 2,
      HIST_TYPE_END = 3
    };
  //: Default constructor
  bsta_joint_histogram_base() : type_(HIST_TYPE_UNKNOWN){}
  virtual  ~bsta_joint_histogram_base() {}
  bsta_joint_hist_type type_;
};

template <class T>
class bsta_joint_histogram_traits
{
 public:
  static bsta_joint_histogram_base::bsta_joint_hist_type type(){
    return bsta_joint_histogram_base::HIST_TYPE_UNKNOWN;}
};

template <>
class bsta_joint_histogram_traits<float>
{
 public:
  static bsta_joint_histogram_base::bsta_joint_hist_type type(){
    return bsta_joint_histogram_base::HIST_TYPE_FLOAT;}
};

template <>
class bsta_joint_histogram_traits<double>
{
 public:
  static bsta_joint_histogram_base::bsta_joint_hist_type type(){
    return bsta_joint_histogram_base::HIST_TYPE_DOUBLE;}
};
#endif // bsta_joint_histogram_base_h_
