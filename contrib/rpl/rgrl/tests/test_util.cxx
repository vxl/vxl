#include "test_util.h"

#include <vcl_cmath.h>

#include <vnl/algo/vnl_determinant.h>

#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_feature_trace_pt.h>
#include <rgrl/rgrl_feature_face_pt.h>

bool
close( vnl_matrix<double> const& a,
       vnl_matrix<double> const& b,
       double tol )
{
  double m1 = a.absolute_value_max();
  double m2 = b.absolute_value_max();
  double m = m1>m2 ? m1 : m2;
  vnl_matrix<double> d = a-b;
  return d.absolute_value_max() / m < tol;
}

bool
close( vnl_vector<double> const& a,
       vnl_vector<double> const& b,
       double tol )
{
  vnl_vector<double> d = a-b;
  return d.inf_norm() < tol;
}


bool
close_det( vnl_matrix<double> const& a,
           vnl_matrix<double> const& b,
           double tol )
{
  return vcl_abs( vnl_determinant(a) - vnl_determinant(b) ) < tol;
}


vnl_vector<double>
vec1d( double x )
{
  vnl_vector<double> v( 1 );
  v[0] = x;
  return v;
}

vnl_vector<double>
vec2d( double x, double y )
{
  vnl_vector<double> v( 2 );
  v[0] = x;
  v[1] = y;
  return v;
}

vnl_vector<double>
vec3d( double x, double y, double z )
{
  vnl_vector<double> v( 3 );
  v[0] = x;
  v[1] = y;
  v[2] = z;
  return v;
}

rgrl_feature_sptr
pf( vnl_vector<double> const& v )
{
  return new rgrl_feature_point( v );
}

rgrl_feature_sptr
tf( vnl_vector<double> const& v, vnl_vector<double> const& t )
{
  return new rgrl_feature_trace_pt( v, t );
}

rgrl_feature_sptr
ff( vnl_vector<double> const& v, vnl_vector<double> const& normal )
{
  return new rgrl_feature_face_pt( v, normal );
}
