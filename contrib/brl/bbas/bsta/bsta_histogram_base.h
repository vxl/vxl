// This is brl/bbas/bsta/bsta_histogram_base.h
#ifndef bsta_histogram_base_h_
#define bsta_histogram_base_h_
//:
// \file
// \brief Abstract histogram base class
// \author Joseph Mundy
// \date March 1, 2008
//
// \verbatim
// \endverbatim

#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class bsta_histogram_base : public vbl_ref_count
{
 public:
  enum bsta_hist_type
    {
      HIST_TYPE_UNKNOWN = 0,
      HIST_TYPE_FLOAT = 1,
      HIST_TYPE_DOUBLE = 2,
      HIST_TYPE_END = 3
    };
  //: Default constructor
  bsta_histogram_base(): type_(HIST_TYPE_UNKNOWN){}

  ~bsta_histogram_base() override = default;

  bsta_hist_type type_;
};

template <class T>
class bsta_histogram_traits
{
 public:
  static bsta_histogram_base::bsta_hist_type type(){
    return bsta_histogram_base::HIST_TYPE_UNKNOWN;}
};

template <>
class bsta_histogram_traits<float>
{
 public:
  static bsta_histogram_base::bsta_hist_type type(){
    return bsta_histogram_base::HIST_TYPE_FLOAT;}
};

template <>
class bsta_histogram_traits<double>
{
 public:
  static bsta_histogram_base::bsta_hist_type type(){
    return bsta_histogram_base::HIST_TYPE_DOUBLE;}
};


#endif // bsta_histogram_base_h_
