#ifndef rgrl_converge_status_h_
#define rgrl_converge_status_h_

//:
// \file
// \brief Class to represent the registration status.
// \author Chuck Stewart
// \date 27 Dec 2002
//
// \verbatim
// modifications:
// Feb 2003 Amitha Perera, impl the functions
// \endverbatim

#include "rgrl_object.h"

//: Represent the registration status
//
class rgrl_converge_status
  : public rgrl_object
{
public:
  //:
  rgrl_converge_status( bool     has_converged,
                        bool     has_stagnated,
                        bool     is_good_enough,
                        double   error,
                        unsigned oscillation_count,
                        double   error_diff );

  virtual ~rgrl_converge_status();

  //:  Return true if the estimation has converged to a viable estimate.
  //
  bool has_converged() const;

  //:  Return true if the estimation has stagnated without reaching a viable estimate.
  //
  //   Only one of has_converged() and has_stagnated() can be true.
  bool has_stagnated() const;

  //:  Return true if the current estimate should be considered "good enough" to solve the problem.
  //
  bool is_good_enough() const;

  //: Current value of the objective function
  //
  //  Lower is better.
  //
  double objective_value() const;

  //: Same as the objective value
  //
  double error() const;

  //: A signed difference between the error of the current status and the error of the previous. 
  //
  double error_diff() const;

  //: Return the number of times that the error_diff changes the sign
  //
  unsigned int oscillation_count() const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_converge_status, rgrl_object );

private:
  bool     has_converged_;
  bool     has_stagnated_;
  bool     is_good_enough_;
  double   error_;
  unsigned oscillation_count_;
  double   error_diff_;
};
  
#endif // rgrl_converge_status_h_
