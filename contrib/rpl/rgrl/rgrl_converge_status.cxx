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
                      bool     in_is_failed,
                      double   in_error,
                      unsigned in_oscillation_count,
                      double   in_error_diff )
  : conv_( conv_on_going ),
    status_( status_on_going ),
    error_( in_error ),
    oscillation_count_( in_oscillation_count ),
    error_diff_( in_error_diff )
{
  if( in_has_converged )
    conv_ = converged;

  if( in_has_stagnated )
    conv_ = stagnated;

  if( in_is_good_enough )
    status_ = good_enough;

  if( in_is_failed )
    status_ = failed;
        
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
  return conv_ == converged;
}


bool
rgrl_converge_status::
has_stagnated() const
{ 
  return conv_ == stagnated;
}


bool
rgrl_converge_status::
is_good_enough() const
{
  return status_ == good_enough;
}

bool
rgrl_converge_status::
is_failed() const
{
  return status_ == failed;
}

rgrl_converge_status::converge_type
rgrl_converge_status::
current_converge() const
{
  return conv_;
}

rgrl_converge_status::status_type
rgrl_converge_status::
current_status() const
{
  return status_;
}

void 
rgrl_converge_status::
set_current_converge( converge_type c )
{
  conv_ = c;
}

void 
rgrl_converge_status::
set_current_status( status_type s )
{
  status_ = s;
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
