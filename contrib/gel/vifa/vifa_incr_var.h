// This is gel/vifa/vifa_incr_var.h
#ifndef _VIFA_INCR_VAR_H_
#define _VIFA_INCR_VAR_H_

//-----------------------------------------------------------------------------
//:
// \file
// \brief Incremental variance measures.
//
// The vifa_incr_var class is used to accumulate the mean & variance of a data
// set incrementally, one data sample at a time.
//
// \author Anthony Hoogs, from DDB in TargetJr
//
// \verbatim
//  Modifications:
//   MPP Mar 2003, Ported to VXL
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_bounding_box.h>
#include <vbl/vbl_smart_ptr.h>
#include <vul/vul_timestamp.h>

//: Compute the mean & variance measures of a data set.
//
// The vifa_incr_var class is used to accumulate the mean & variance of a data
// set incrementally, one data sample at a time.

class vifa_incr_var : public vul_timestamp, public vbl_ref_count
{
 protected:
  //: The current mean of the data set
  double  data_mean_;

  //: The current variance of the data set
  double  data_var_;

  //: The number of samples in the data set
  int     n_;

  //: The minimum-value and maximum-value sample of the data set
  vbl_bounding_box<double,1>  min_max_;

 public:
  //: Default constructor
  vifa_incr_var() : data_mean_(0.0), data_var_(0.0), n_(0) {}

  //: Fetch the current mean
  double  get_mean() const { return data_mean_; }

  //: Fetch the current variance
  double  get_var() const { return data_var_; }

  //: Fetch the number of samples received so far
  int     get_n() const { return n_; }

  //: Fetch the minimum-value sample of the data set
  double  get_min() const { return min_max_.min()[0]; }

  //: Fetch the maximum-value sample of the data set
  double  get_max() const { return min_max_.max()[0]; }

  //: Update the mean & variance measures with a new sample
  // This method is a convenience front-end for add_sample() (see below)
  void  add_sample(double  data_point); // !< The new data sample

  //: Update the mean & variance measures with a new sample
  void  add_sample(double  data_point,  // !< The new data sample
                   double  prev_factor, // !< Ratio of current n_ & new n_
                   double  curr_factor  // !< Inverse of new n_
                  );
};

typedef vbl_smart_ptr<vifa_incr_var>   vifa_incr_var_sptr;
typedef vcl_vector<vifa_incr_var_sptr> incr_var_list;

#endif  // _VIFA_INCR_VAR_H_
