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
class bsta_histogram_base : public vbl_ref_count
{
 public:
  //: Default constructor
  bsta_histogram_base(){}

  virtual ~bsta_histogram_base() {}
};

#endif // bsta_histogram_base_h_
