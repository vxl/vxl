#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vnl/vnl_math.h>

#include <rrel/rrel_util.h>
#include <testlib/testlib_test.h>

bool close(double,double);

bool is_sorted( vcl_vector<double>::iterator first, vcl_vector<double>::iterator last )
{
  bool ok=true;
  vcl_vector<double>::iterator i, j;
  for ( i=first, j=first+1; ok && j!=last; ++i,++j)
    ok = ! ( *j < *i );
  return ok;
}

MAIN( test_robust_util )
{
  START( "rrel_util" );

  //
  //  rrel_util_median_abs_dev_scale
  //
  double test_arr[] = { -1.0, -2.0, 3.0, -4.0, -1.5, -11.0, 14.1,
                        -56.0, -2.4, 3.1, 1.1, 2.15, -2.1, -0.8 };
  vcl_vector<double> test_vec( test_arr, test_arr+14 );
  const double target=3.0*1.4826*(1+5.0/11);
  testlib_test_begin( "rrel_util_median_abs_dev_scale" );
  double res = rrel_util_median_abs_dev_scale( test_vec.begin(), test_vec.end(), 3 );
  testlib_test_perform( close( res, target ) );

  //
  //  rrel_util_weighted_scale
  //
  double wgt_arr[] = { 0.5, 0.75, 0.8,  0,  0.25, 0.4, 0.6, 0.9, 0.8, 0.75 };
  double rs_arr[] =  { 2,   4,    0.5,  7,  6,    5,   3,   2,   5,   2    };
  double sum_wr = 0;
  double sum_w = 0;
  for ( int i=0; i<10; ++i ) {
    sum_wr += wgt_arr[i] * vnl_math_sqr(rs_arr[i]);
    sum_w += wgt_arr[i];
  }
  int num = 10;
  int dof = 1;
  testlib_test_begin( "rrel_util_weighted_scale" );
  double corr_wgted_scale = vcl_sqrt( sum_wr / ( sum_w * (num-dof) / num ) );
  double est_wgted_scale = rrel_util_weighted_scale( (double*)rs_arr, rs_arr+10, (double*)wgt_arr, dof );
  testlib_test_perform( close(corr_wgted_scale, est_wgted_scale) );

  //
  //  rrel_util_median_and_scale
  //
  double test_arr4[] = { -1.0, -2.0, 1.2, -4.0, -1.5, -11.0, 14.1, 2.2,
                         56.0, -2.4, 3.1,  1.1,  2.1,  -2.1, -0.8, 0.6,
                         13.1,  6.2, 55.8 };
  vcl_vector<double> test_vect4( test_arr4, test_arr4+18 );
  double median, scale;
  const double corr_median = 1.1;
  dof = 1;
  const double corr_scale = 3.2 * (1.0 + 5.0/(18.0-dof)) * 1.4826;
  testlib_test_begin( "rrel_util_median_and_scale_copy" );
  rrel_util_median_and_scale_copy( test_vect4.begin(), test_vect4.end(), median, scale, dof);
  testlib_test_perform( close(median,corr_median) && close(scale,corr_scale) );
  testlib_test_begin( "rrel_util_median_and_scale" );
  rrel_util_median_and_scale( test_vect4.begin(), test_vect4.end(),
                              median, scale, 1 );
  testlib_test_perform( close(median,corr_median) && close(scale,corr_scale) );

  //
  //  rrel_util_intercept_adjustment
  //
  double test_arr2[] = { -1.0, -2.0, 1.2, -4.0, -1.5, -11.0, 14.1, 2.2,
                         -56.0, -2.4, 3.1,  1.1,  2.1,  -2.1, -0.8, 0.6 };
  vcl_vector<double> test_vect2( test_arr2, test_arr2+16 );
  double center, half_width;
  const double corr_center=-0.45, corr_half_width=1.65;
  testlib_test_begin( "rrel_util_intercept_adjustment_copy --- correct?");
  rrel_util_intercept_adjustment( test_vect2.begin(), test_vect2.end(), center, half_width, 1 );
  testlib_test_perform( close(center,corr_center) && close(half_width, corr_half_width) );
  testlib_test_begin( "rrel_util_intercept_adjustment(pointer) --- correct?");
  rrel_util_intercept_adjustment( test_vect2.begin(), test_vect2.end(), center, half_width, 1 );
  testlib_test_perform( close(center,corr_center) && close(half_width, corr_half_width) );
  testlib_test_begin( "rrel_util_intercept_adjustment (pointer) --- sorted?");
  testlib_test_perform( is_sorted( test_vect2.begin(), test_vect2.end() ) );

  //
  //  rrel_util_intercept_adjust_stats
  //
  double mean, std, frac;
  double test_arr3[] = { -1.0, -2.0, 1.2, -4.0, -1.5, -11.0, 14.1, 2.2,
                         -56.0, -2.4, 3.1,  1.1,  2.1,  -2.1, -0.8, 0.6 };
  vcl_vector<double> test_vec3( test_arr3, test_arr3+16 );
  const double corr_mean=-0.2692307, corr_std=2.140632, corr_frac = 0.8125;
  testlib_test_begin( "rrel_util_intercept_adjust_stats_copy " );
  rrel_util_intercept_adjust_stats_copy( test_vec3.begin(), test_vec3.end(), mean, std, frac, 1 );
  testlib_test_perform( close(mean,corr_mean) && close(std,corr_std) && close(frac,corr_frac) );
  testlib_test_begin( "rrel_util_intercept_adjust_stats (pointer) " );
  rrel_util_intercept_adjust_stats( test_vec3.begin(), test_vec3.end(),
                                    mean, std, frac, 1 );
  testlib_test_perform( close(mean,corr_mean) && close(std,corr_std) && close(frac,corr_frac) );

  SUMMARY();
}
