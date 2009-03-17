// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef rgtl_sqt_space_txx
#define rgtl_sqt_space_txx

#include "rgtl_sqt_space.hxx"

#include <vcl_cassert.h>
#include <vcl_cmath.h>

//----------------------------------------------------------------------------
template <unsigned int D>
unsigned int
rgtl_sqt_space_base<D>::direction_to_face(double const d[D])
{
  // The face axis corresponds to the component of the direction that
  // has the largest magnitude.
  double max_d = vcl_fabs(d[0]);
  unsigned int max_axis = 0;
  unsigned int max_side = (d[0] >= 0)? 1:0;
  for(unsigned int a=1; a < D; ++a)
    {
    double cur_d = vcl_fabs(d[a]);
    if(cur_d > max_d)
      {
      max_d = cur_d;
      max_axis = a;
      max_side = ((d[a] >= 0)? 1:0);
      }
    }
  return (max_axis<<1) | max_side;
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
void
rgtl_sqt_space<D, Face>
::parameters_to_direction(double const u[D-1], double d[D])
{
  for(unsigned int a=0; a < face_axis; ++a)
    {
    d[a] = vcl_tan(u[a]);
    }
  d[face_axis] = (face_side? +1:-1);
  for(unsigned int a=face_axis+1; a < D; ++a)
    {
    d[a] = vcl_tan(u[a-1]);
    }
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
void
rgtl_sqt_space<D, Face>
::direction_to_parameters(double const d[D], double u[D-1])
{
  double da = (face_side?+1:-1)*d[face_axis];
  assert(da > 0);
  for(unsigned int a=0; a < face_axis; ++a)
    {
    u[a] = vcl_atan(d[a]/da);
    }
  for(unsigned int a=face_axis+1; a < D; ++a)
    {
    u[a-1] = vcl_atan(d[a]/da);
    }
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
void
rgtl_sqt_space<D, Face>
::isoplane_normal(unsigned int j, double u, double n[D])
{
  for(unsigned int a=0; a < face_axis; ++a)
    {
    if(a == j)
      {
      n[a] = vcl_cos(u);
      }
    else
      {
      n[a] = 0;
      }
    }
  n[face_axis] = (face_side?-1:+1)*vcl_sin(u);
  for(unsigned int a=face_axis+1; a < D; ++a)
    {
    if(a-1 == j)
      {
      n[a] = vcl_cos(u);
      }
    else
      {
      n[a] = 0;
      }
    }
}

//----------------------------------------------------------------------------
#define RGTL_SQT_SPACE_BASE_INSTANTIATE(D) \
  template class rgtl_sqt_space_base< D >
#define RGTL_SQT_SPACE_INSTANTIATE(D, Face) \
  template class rgtl_sqt_space< D , Face >

#endif
