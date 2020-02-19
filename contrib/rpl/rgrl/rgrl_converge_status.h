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

  enum converge_type { conv_on_going, converged, stagnated };

  enum status_type { status_on_going, good_enough, failed, good_and_terminate };

  //: default ctor
  rgrl_converge_status( );

  //: default ctor
  rgrl_converge_status( converge_type conv,
                        status_type stat,
                        double error,
                        unsigned osc_count,
                        double error_diff );


  //:
  rgrl_converge_status( bool     has_converged,
                        bool     has_stagnated,
                        bool     is_good_enough,
                        bool     in_is_failed,
                        double   error,
                        unsigned oscillation_count,
                        double   error_diff );

  ~rgrl_converge_status() override;


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

  //:  Return true if the current estimate should be considered "a failure" that would not solve the problem.
  //
  bool is_failed() const;

  //: return true is the current estimate is so good that registration can be terminated
  //
  bool is_good_and_should_terminate() const;

  //: return converge enum
  converge_type current_converge() const;

  //: return current status
  status_type  current_status() const;

  //: set current coverage
  void set_current_converge( converge_type c );

  //: set current coverage
  void set_current_status( status_type s );

  //: set objective value
  void set_objective_value( double obj );

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

protected:
  converge_type conv_{conv_on_going};
  status_type status_{status_on_going};
  double error_{-1.0};
  unsigned oscillation_count_{0};
  double error_diff_{-1.0};
};

#endif // rgrl_converge_status_h_
