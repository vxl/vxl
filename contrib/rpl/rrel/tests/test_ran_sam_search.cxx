// This is rpl/rrel/tests/test_ran_sam_search.cxx
#include <vcl_iostream.h>

#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_math.h>

#include <rrel/rrel_linear_regression.h>
#include <rrel/rrel_lms_obj.h>
#include <rrel/rrel_ran_sam_search.h>

#include <rrel/tests/similarity_from_matches.h>

#include <testlib/testlib_test.h>

double noise( double sigma );

inline bool
sample_ok( const vcl_vector<int>& indices, int num_pts )
{
  bool ok=true;
  for ( unsigned int i=0; i<indices.size() && ok; ++i ) {
    ok = 0<=indices[i] && indices[i]<num_pts;
    for ( unsigned int j=i+1; j<indices.size() && ok; ++ j )
      ok = (indices[i] != indices[j]);
  }
  return ok;
}


struct null_problem : public rrel_estimation_problem
{
  null_problem( unsigned int num_samples, unsigned int dof, unsigned int min_samples )
    : rrel_estimation_problem( dof, min_samples ), ns(num_samples) { }
  unsigned int num_samples() const { return ns; }
  void compute_residuals( const vnl_vector<double>& /*params*/,
                          vcl_vector<double>& /*residuals*/ ) const { }
  unsigned int ns;
};


static void test_ran_sam_search()
{
  vnl_double_3 true_params(10.0, 0.02, -0.1);
  const int num_pts=12;

  //  Build LinearRegression objects.
  vcl_vector< vnl_vector<double> > pts(num_pts);
  vcl_vector< double > error(num_pts);

  double x = 1.0, y=-0.5; error[0]=-0.001;
  double z= a[0] + a[1]*x + a[2]*y + error[0];
  pts[0]=vnl_double_3(x,y,z).as_vector();

  x = 2.0;  y=4.0;  error[1]=0; z = a[0] + a[1]*x + a[2]*y + error[1];
  pts[1]=vnl_double_3(x,y,z).as_vector();

  x = 3.0;  y=1.0;  error[2]=0; z = a[0] + a[1]*x + a[2]*y + error[2];
  pts[2]=vnl_double_3(x,y,z).as_vector();

  x = -2.0;  y=3.0; error[3]=-0.0025;  z = a[0] + a[1]*x + a[2]*y + error[3];
  pts[3]=vnl_double_3(x,y,z).as_vector();

  x = 2.0;  y=4.0;  error[4]=0.9;  z = a[0] + a[1]*x + a[2]*y + error[4];
  pts[4]=vnl_double_3(x,y,z).as_vector();

  x = 5.0;  y=-4.0;  error[5]=0; z = a[0] + a[1]*x + a[2]*y + error[5];
  pts[5]=vnl_double_3(x,y,z).as_vector();

  x = 3.0;  y=-2.0;  error[6]=-0.004; z = a[0] + a[1]*x + a[2]*y + error[6];
  pts[6]=vnl_double_3(x,y,z).as_vector();

  x = 2.0;  y=-2.0;  error[7]=-0.8; z = a[0] + a[1]*x + a[2]*y + error[7];
  pts[7]=vnl_double_3(x,y,z).as_vector();

  x = 3.0;  y=0.0;  error[8]=0.008; z = a[0] + a[1]*x + a[2]*y + error[8];
  pts[8]=vnl_double_3(x,y,z).as_vector();

  x = -1.0;  y=-2.0;  error[9]=0.003; z = a[0] + a[1]*x + a[2]*y + error[9];
  pts[9]=vnl_double_3(x,y,z).as_vector();

  x = 0.0;  y=0.0;  error[10]=0.3; z = a[0] + a[1]*x + a[2]*y + error[10];
  pts[10]=vnl_double_3(x,y,z).as_vector();

  x = -1.0;  y=2.0;  error[11]=-0.0012; z = a[0] + a[1]*x + a[2]*y + error[11];
  pts[11]=vnl_double_3(x,y,z).as_vector();

  //
  //  Create the linear regression problem and an lms objective
  //  function.
  //
  bool use_intercept=true;
  rrel_estimation_problem * lr = new rrel_linear_regression( pts, use_intercept );
  int dof = lr->num_samples_to_instantiate();
  rrel_objective* lms = new rrel_lms_obj( dof );
  testlib_test_begin( "ctor");
  rrel_ran_sam_search * ransam = new rrel_ran_sam_search();
  testlib_test_perform( ransam != 0 );
#if 0
  //  Test sampling by generating all parameters
  //
  testlib_test_begin( "generation of all samples" );
  ransam->set_gen_all_samples();
  //  ransam->print_params();

  int num_points=5;
  vcl_vector<int> indices(dof);
  ransam->calc_num_samples( num_points, dof );
  testlib_test_perform( ransam->samples_tested() == 10 );

  testlib_test_begin( "samples generated in order" );
  ransam->next_sample( 0, num_points, indices, dof );
  bool ok = indices[0]==0 && indices[1]==1 && indices[2]==2;
  ransam->next_sample( 1, num_points, indices, dof );
  ok = ok && (indices[0]==0 && indices[1]==1 && indices[2]==3);
  ransam->next_sample( 2, num_points, indices, dof );
  ok = ok && (indices[0]==0 && indices[1]==1 && indices[2]==4);
  ransam->next_sample( 3, num_points, indices, dof );
  ok = ok && (indices[0]==0 && indices[1]==2 && indices[2]==3);
  ransam->next_sample( 4, num_points, indices, dof );
  ok = ok && (indices[0]==0 && indices[1]==2 && indices[2]==4);
  ransam->next_sample( 5, num_points, indices, dof );
  ok = ok && (indices[0]==0 && indices[1]==3 && indices[2]==4);
  ransam->next_sample( 6, num_points, indices, dof );
  ok = ok && (indices[0]==1 && indices[1]==2 && indices[2]==3);
  ransam->next_sample( 7, num_points, indices, dof );
  ok = ok && (indices[0]==1 && indices[1]==2 && indices[2]==4);
  ransam->next_sample( 8, num_points, indices, dof );
  ok = ok && (indices[0]==1 && indices[1]==3 && indices[2]==4);
  ransam->next_sample( 9, num_points, indices, dof );
  ok = ok && (indices[0]==2 && indices[1]==3 && indices[2]==4);
  testlib_test_perform( ok );
  //  vcl_cout << "Test 2: " << ( ok ? "yes" : "NO" ) << vcl_endl;

  double max_outlier_frac = 0.4;
  double desired_prob_good = 0.99;
  int max_pops = 2;
  //  vcl_cout << "\nNow testing less than complete sampling:\n";
  testlib_test_begin( "1st probabilistic sampling" );
  ransam->set_sampling_params( max_outlier_frac, desired_prob_good,
                               max_pops);
  //  vcl_cout << "Parameters:\n";
  // ransam->print_params();
  ransam->calc_num_samples( num_pts, dof );
  testlib_test_perform(  ransam->samples_tested() == 83 );

  testlib_test_begin( "2nd probabilistic sampling" );
  max_outlier_frac=0.5;
  ransam->set_sampling_params( max_outlier_frac, desired_prob_good );
  // vcl_cout << "Parameters:\n";
  // ransam->print_params();
  ransam->calc_num_samples( num_pts, dof );
  testlib_test_perform( ransam->samples_tested() == 35 );

  testlib_test_begin( "generating samples" );
  ok = true;
  for ( int i=0; i<10 && ok; ++i ) {
    ransam->next_sample( i, num_pts, indices, dof );
    ok = sample_ok( indices, num_pts );
  }
  testlib_test_perform( ok );
#endif
  //
  //  Actually running the random sampler ...
  //
  bool ok;
//int trace_level=0;
  testlib_test_begin( "estimate succeed" );
  testlib_test_perform( ransam->estimate( lr, lms ) );
  vnl_vector<double> est_params = ransam->params();
#ifdef DEBUG
  vcl_cout << "estimate = " << est_params
           << ", true model = " << true_params << vcl_endl
           << "scale = " << ransam->get_scale() << vcl_endl;
#endif // DEBUG
  ok = vnl_math_abs( est_params[0] - true_params[0] ) < 0.2
    && vnl_math_abs( est_params[1] - true_params[1] ) < 0.025
    && vnl_math_abs( est_params[2] - true_params[2] ) < 0.025;
  testlib_test_begin( "accurate estimate" );
  testlib_test_perform( ok );

  delete lr;

  double sigma = 0.1;
  vcl_vector<image_point_match> matches;
  vnl_double_4 sim_params(1.4,-0.2,20.0,-18.0);
  generate_similarity_matches( sim_params.as_vector(), sigma, matches );  // 20 matches, 13 points
  rrel_estimation_problem* match_prob = new similarity_from_matches( matches );
#if 0
  ransam->set_sampling_params( 0.5, 0.999, 1 );
  ransam->calc_num_samples( match_prob->num_data_points(), match_prob->num_correspondences_all(),
                            match_prob->num_points_to_instantiate() );
  testlib_test_begin( "num samples for matching problem" );
  testlib_test_perform( ransam->samples_tested() == 12 );
#endif
//trace_level=0;
  testlib_test_begin( "non-unique estimate succeed" );
  testlib_test_perform( ransam->estimate( match_prob, lms ) );
  est_params = ransam->params();
  vcl_cout << "similarity estimate = " << est_params
           << ", true similarity model = " << sim_params << vcl_endl
           << "scale = " << ransam->scale() << vcl_endl;
  ok = vnl_math_abs( est_params[0] - sim_params[0] ) < 0.025
    && vnl_math_abs( est_params[1] - sim_params[1] ) < 0.025
    && vnl_math_abs( est_params[2] - sim_params[2] ) < 1.0
    && vnl_math_abs( est_params[3] - sim_params[3] ) < 1.0;
  testlib_test_begin( "non-unique estimate accurate" );
  testlib_test_perform( ok );

  delete ransam;
  delete lms;
  delete match_prob;
}

TESTMAIN(test_ran_sam_search);
