// This is brl/bmvl/btom/btom_slice_simulator_params.h
#ifndef btom_slice_simulator_params_h_
#define btom_slice_simulator_params_h_
//:
// \file
// \brief parameter mixin for btom_slice_simulator
//
// \author
//    Joseph L. Mundy - March 4, 2003
//    Brown University
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iosfwd.h>

class btom_slice_simulator_params : public gevd_param_mixin
{
 public:
  btom_slice_simulator_params(const int ncyl=10,
                              const float min_xy_sigma = 2.0,
                              const float max_xy_sigma = 10.0);

  btom_slice_simulator_params(const btom_slice_simulator_params& old_params);
 ~btom_slice_simulator_params(){}

  bool SanityCheck();
  friend
    vcl_ostream& operator<<(vcl_ostream&,const btom_slice_simulator_params& sp);
 protected:
  void InitParams(int ncyl,
                  float min_xy_sigma,
                  float max_xy_sigma);
 public:
  //
  // Parameter blocks and parameters
  //
  int ncyl_;                 //!< number of cylinders in each direction
  float min_xy_sigma_;       //!< min,max cylinder sigmas
  float max_xy_sigma_;
};


#endif // btom_slice_simulator_params_h_
