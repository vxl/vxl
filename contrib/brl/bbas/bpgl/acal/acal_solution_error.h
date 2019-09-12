// This is//external/acal/acal_solution_error.h
#ifndef acal_solution_error_h
#define acal_solution_error_h

//:
// \file
// \brief A set of error values for geo_correction
// \author J.L. Mundy
// \date May 6, 2019
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>

class acal_solution_error
{
 public:
  acal_solution_error():
    min_eps_u_(0.0), min_eps_v_(0.0),
    max_eps_u_(0.0), max_eps_v_(0.0),
    rms_err_u_(0.0),rms_err_v_(0.0) {}

  acal_solution_error(double min_eps_u, double min_eps_v,
                      double max_eps_u, double max_eps_v,
                      double rms_err_u,  double rms_err_v):
    min_eps_u_(min_eps_u), min_eps_v_(min_eps_v),
    max_eps_u_(max_eps_u), max_eps_v_(max_eps_v),
    rms_err_u_(rms_err_u),rms_err_v_(rms_err_v) {}

  double min_eps_u_;
  double min_eps_v_;
  double max_eps_u_;
  double max_eps_v_;
  double rms_err_u_;
  double rms_err_v_;

  void print() {
    std::cout << "min (" << min_eps_u_ << ' ' << min_eps_v_ << ")" << std::endl;
    std::cout << "max (" << max_eps_u_ << ' ' << max_eps_v_ << ")" << std::endl;
    std::cout << "rms (" << rms_err_u_ << ' ' << rms_err_v_ << ")" << std::endl;
  }

  double max_err() {return max_eps_u_ > max_eps_v_ ? max_eps_u_ : max_eps_v_;}
  double min_err() {return min_eps_u_ < min_eps_v_ ? min_eps_u_ : min_eps_v_;}
  double total_rms() {return rms_err_u_ + rms_err_v_;}
};

#endif
