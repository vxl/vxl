//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include "rgrl_converge_status.h"

#include <vcl_cassert.h>
 
rgrl_converge_status::
rgrl_converge_status( bool     in_has_converged,
                      bool     in_has_stagnated,
                      bool     in_is_good_enough,
                      double   in_error,
                      unsigned in_oscillation_count,
                      double   in_error_diff )
  : has_converged_( in_has_converged ),
    has_stagnated_( in_has_stagnated ),
    is_good_enough_( in_is_good_enough ),
    error_( in_error ),
    oscillation_count_( in_oscillation_count ),
    error_diff_( in_error_diff )
{
  assert( ! ( in_has_converged && in_has_stagnated ) );
}

rgrl_converge_status::
~rgrl_converge_status()
{
}

bool
rgrl_converge_status::
has_converged() const
{
  return has_converged_;
}


bool
rgrl_converge_status::
has_stagnated() const
{ 
  return has_stagnated_;
}


bool
rgrl_converge_status::
is_good_enough() const
{
  return is_good_enough_;
}


double
rgrl_converge_status::
objective_value() const
{
  return error_;
}


double
rgrl_converge_status::
error() const
{
  return error_;
}


unsigned int
rgrl_converge_status::
oscillation_count() const
{
  return oscillation_count_;
}


double
rgrl_converge_status::
error_diff() const
{
  return error_diff_;
}
