#ifndef rgrl_test_util_h_
#define rgrl_test_util_h_

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <rgrl/rgrl_feature_sptr.h>

bool
close( vnl_matrix<double> const& a, vnl_matrix<double> const& b, double tol = 1e-6 );

bool
close( vnl_vector<double> const& a, vnl_vector<double> const& b, double tol = 1e-6 );

bool
close_det( vnl_matrix<double> const& a, vnl_matrix<double> const& b, double tol = 1e-6 );

vnl_vector<double>
vec1d( double x );

vnl_vector<double>
vec2d( double x, double y );

vnl_vector<double>
vec3d( double x, double y, double z );

// point feature
rgrl_feature_sptr
pf( vnl_vector<double> const& v );

// trace point feature
rgrl_feature_sptr
tf( vnl_vector<double> const& v, vnl_vector<double> const& t );

// face point feature
rgrl_feature_sptr
ff( vnl_vector<double> const& v, vnl_vector<double> const& normal );

#endif // rgrl_test_util_h_
