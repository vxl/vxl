#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <testlib/testlib_test.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>

#include "similarity_from_matches.h"

#include <rrel/rrel_m_est_obj.h>

bool close(double,double);

class null_m_est : public rrel_m_est_obj
{
 public:
  double rho( double u ) const override { return u; }
  double wgt( double u ) const override { return u; }
  void wgt( vect_const_iter b, vect_const_iter e, vect_const_iter s, vect_iter w) const override { rrel_m_est_obj::wgt(b,e,s,w); }
  void wgt( vect_const_iter b, vect_const_iter e, double s, vect_iter w) const override { rrel_m_est_obj::wgt(b,e,s,w); }
};

static void test_similarity_from_matches()
{
  vnl_matrix_fixed<double,2,2> A(0.0);
  vnl_vector_fixed<double,2> t;
  vnl_vector_fixed<double,4> params;
  double a = 1.2, b=0.3, tx=15, ty=-4;
  params[0] = a; params[1] = b;  params[2] = tx;  params[3] = ty;
  A(0,0) = A(1,1) = a;
  A(0,1) = -b;  A(1,0) = b;
  t[0] = tx; t[1] = ty;

  vnl_vector_fixed<double,2> from_loc;
  vnl_vector_fixed<double,2> to_loc;
  std::vector< image_point_match > matches;

  from_loc[0] = 10;  from_loc[1] = 20;
  to_loc = A*from_loc + t;
  int id = 0;
  matches.emplace_back( from_loc, to_loc, id );  // 0 - id 0 - good

  to_loc[0] += 3;  to_loc[1] += -4;   // dist = 5
  id = 0;
  matches.emplace_back( from_loc, to_loc, id );  // 1 - id 0 - bad

  from_loc[0] = -15;  from_loc[1] = 6;
  to_loc = A*from_loc + t;
  to_loc[0] += -30;  to_loc[1] += 40;  // dist = 50
  id = 1;
  matches.emplace_back( from_loc, to_loc, id );  // 2 - id 1 - bad

  to_loc = A*from_loc + t;
  id = 1;
  matches.emplace_back( from_loc, to_loc, id );  // 3 - id 1 - good

  id = 1;
  to_loc[0] += -6; to_loc[1] += 8;   // dist = 10
  matches.emplace_back( from_loc, to_loc, id );  // 4 - id 1 - bad

  id = 2;
  from_loc[0] += 5; from_loc[1] += -3;
  to_loc = A*from_loc + t;
  to_loc[0] += 0.3;  to_loc[1] += -0.4;
  matches.emplace_back( from_loc, to_loc, id );  // 5 - id 2 - good (small error)

  testlib_test_begin( "ctor" );
  similarity_from_matches sim( matches );
  testlib_test_perform( true );

  TEST("num to instantiate", sim.num_samples_to_instantiate(), 2);

  TEST("num_unique_samples" , sim.num_unique_samples(), 3);

  std::cout << " num = " << sim.num_samples() << std::endl;
  TEST("num_samples" , sim.num_samples(), 6);

  vnl_vector<double> est_params(4);
  std::vector<int> indices(2);  indices[0] = 4;  indices[1] = 3;
  TEST("fit_from_minimal_sample -- degenerate" , !sim.fit_from_minimal_set( indices, est_params ), true);

  indices[0] = 0;  indices[1] = 3;
  TEST("fit_from_minimal_sample -- exact",
       sim.fit_from_minimal_set( indices, est_params )
       && close( est_params[0], params[0] )
       && close( est_params[1], params[1] )
       && close( est_params[2], params[2] )
       && close( est_params[3], params[3] ), true);

  std::vector<double> residuals;
  sim.compute_residuals( params.as_ref(), residuals );
  TEST("compute_residuals",
       residuals.size() == 6
       && close( residuals[0],  0 )
       && close( residuals[1],  5 )
       && close( residuals[2], 50 )
       && close( residuals[3],  0 )
       && close( residuals[4], 10 )
       && close( residuals[5],  0.5 ), true);

  std::vector<double> temp_res(6);
  std::vector<double> weights;
  null_m_est obj;
  temp_res[0] = 1;  temp_res[1] = 0.1;
  temp_res[2] = 0.02; temp_res[3] = 1; temp_res[4] = 0.12;
  temp_res[5] = 0.9;

  sim.compute_weights( temp_res, &obj, 1.0, weights );
  TEST("compute_weights",
       close( weights[0], 1.0 * 1.0 / 1.1 ) &&
       close( weights[1], 0.1 * 0.1 / 1.1 ) &&
       close( weights[2], 0.02 * 0.02 / 1.14 ) &&
       close( weights[3], 1.0 * 1.0 / 1.14 ) &&
       close( weights[4], 0.12 * 0.12 / 1.14 ) &&
       close( weights[5], 0.9 ), true);
}

TESTMAIN(test_similarity_from_matches);
