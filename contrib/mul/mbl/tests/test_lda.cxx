// This is mul/mbl/tests/test_lda.cxx
#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h> // vpl_unlink()

#include <mbl/mbl_lda.h>
#include <mbl/mbl_log.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matlab_print.h>
#include <vnl/vnl_random.h>

#include <testlib/testlib_test.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

void arr2vec(vnl_vector<double>& vector,
             int nx,
             const double *double_array)
{
  vector.set_size(nx);
  for (int x=0;x<nx;++x)
    vector(x) = double_array[x];
}

void vec2mat(vnl_matrix<double>& matrix,
             const std::vector< vnl_vector<double> >& list_vec)
{
  unsigned int n_rows= list_vec.size();
  unsigned int n_cols= list_vec[0].size();

  matrix.set_size( n_rows, n_cols );
  for (unsigned int i=0; i< n_rows; ++i)
  {
    assert ( list_vec[i].size() == n_cols );
    for (unsigned int j=0; j< n_cols; ++j)
      matrix(i,j)=list_vec[i](j);
  }
}

void add_data(std::vector< vnl_vector<double> >& d_vec,
              std::vector< int >& l_vec,
              std::vector< vnl_vector<double> >& new_data,
              int label)
{
  int n=new_data.size();
  for (int i=0; i<n; ++i)
  {
    d_vec.push_back( new_data[i] );
    l_vec.push_back ( label );
  }
}

void sample_axis_gaussian(std::vector< vnl_vector<double> >& sample,
                          vnl_vector<double>& mean,
                          vnl_vector<double>& var,
                          int ns)
{
  vnl_random mzr(3);
  sample.resize(0);
  int d= mean.size();
  assert ( mean.size() == var.size() );
  for (int i=0; i<ns; ++i)
  {
    vnl_vector<double> s(d);
    for (int j=0; j<d; ++j)
      s(j)=mzr.normal()*std::sqrt( var(j) )+mean(j);
    sample.push_back(s);
  }
}


void test_lda()
{
  std::cout << "\n*****************\n"
           <<   " Testing mbl_lda\n"
           <<   "*****************\n";

  mbl_logger::root().load_log_config_file();

  //create 2 gaussian distributions
  vnl_vector<double> m0,v0,m1,v1;

   // Create g0
  double m0_array[2] = {8, 6};
  arr2vec(m0,2,m0_array);
  double v0_array[2] = {3, 2};
  arr2vec(v0,2,v0_array);

   // Create g1
  double m1_array[2] = {1, 2};
  arr2vec(m1,2,m1_array);
  double v1_array[2] = {20, 30};
  arr2vec(v1,2,v1_array);

  //sample some data
  int n_data=50;
  std::vector< vnl_vector<double> >d0,d1;
  sample_axis_gaussian( d0, m0, v0, n_data);
  sample_axis_gaussian( d1, m1, v1, n_data);

  vnl_matrix<double> mat0;
  vec2mat( mat0, d0 );
  std::ofstream of0( "mat0.txt" );
  vnl_matlab_print(of0,mat0);
  of0.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink("mat0.txt");
#endif

  vnl_matrix<double> mat1;
  vec2mat( mat1, d1 );
  std::ofstream of1( "mat1.txt" );
  vnl_matlab_print(of1,mat1);
  of1.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink("mat1.txt");
#endif

  //concatenate data + make labels
  std::vector< vnl_vector<double> > d_vec(0);
  std::vector< int > l_vec(0);
  add_data ( d_vec, l_vec, d0, 0);
  add_data ( d_vec, l_vec, d1, 1);

  mbl_lda lda;
  lda.build( d_vec, l_vec);

  int c_m0= lda.classify( m0 );
  std::cout<<"c_m0= "<<c_m0<<std::endl;
  TEST("Test classifying mean 0", c_m0==0 , true);

  int c_m1= lda.classify( m1 );
  std::cout<<"c_m1= "<<c_m1<<std::endl;
  TEST("Test classifying mean 1", c_m1==1 , true);

  // Project means into discriminant ("d") space and back to original ("x") space
  vnl_vector<double> m0_d, m1_d;
  lda.x_to_d(m0_d, m0);
  lda.x_to_d(m1_d, m1);
  vnl_vector<double> m0_x, m1_x;
  lda.d_to_x(m0_x, m0_d);
  lda.d_to_x(m1_x, m1_d);
  TEST_NEAR("Mean 0 projects from discriminant to original space", (m0_x-m0).magnitude(), 0.0, 2.0);
  TEST_NEAR("Mean 1 projects from discriminant to original space", (m1_x-m1).magnitude(), 0.0, 2.0);
  std::cout << "m1: " << m1 << std::endl;
  std::cout << "m1_x: " << m1_x << std::endl;

  //sample some test data
  std::vector< vnl_vector<double> > test_d0,test_d1;
  sample_axis_gaussian( test_d0, m0, v0, n_data);
  sample_axis_gaussian( test_d1, m1, v1, n_data);

  // Test classfication error is reasonable
  int c0_count=0;
  int c1_count=0;
  for (int i=0;i<n_data;++i)
  {
    int c0=lda.classify( test_d0[i] );
    if (c0 == 0) c0_count++;

    int c1=lda.classify( test_d1[i] );
    if (c1 == 1) c1_count++;
  }

  double prop0=c0_count*1.0/n_data;
  double prop1=c1_count*1.0/n_data;

  std::cout<<"c0 %correct= "<< prop0<<std::endl
          <<"c1 %correct= "<< prop1<<std::endl;


  // extract principle eigenvector + use for classification
  vnl_matrix<double> b= lda.basis();
  vnl_vector<double> eig_vec= b.get_column(0);
  std::cout<<"eig_vec= "<<eig_vec<<std::endl;

   // Test classfication error is reasonable, just using one eigenvector.
  //actually method above only uses one eigenvector anyway, so results are
  //the same!
  double mean0=dot_product( lda.class_mean(0), eig_vec );
  double mean1=dot_product( lda.class_mean(1), eig_vec );
  c0_count=0;
  c1_count=0;
  for (int i=0;i<n_data;++i)
  {
    double dp0=dot_product( test_d0[i], eig_vec );
    //std::cout<<"dp0= "<<dp0<<std::endl;

    double dm0, dm1;
    dm0= (dp0-mean0)*(dp0-mean0);
    dm1= (dp0-mean1)*(dp0-mean1);
    int c0;
    if ( dm0< dm1)
      c0=0;
    else
      c0=1;

    if (c0 == 0) c0_count++;

    double dp1=dot_product( test_d1[i], eig_vec );
    //std::cout<<"dp1= "<<dp1<<std::endl;

    dm0= (dp1-mean0)*(dp1-mean0);
    dm1= (dp1-mean1)*(dp1-mean1);

    int c1;
    if ( dm0< dm1)
      c1=0;
    else
      c1=1;

    if (c1 == 1) c1_count++;
  }

  prop0=c0_count*1.0/n_data;
  prop1=c1_count*1.0/n_data;

  std::cout<<"c0 %correct= "<< prop0<<std::endl
          <<"c1 %correct= "<< prop1<<std::endl;

  TEST_NEAR("Test prop correct prop0>0.9", prop0, 1.0, 0.1);
  TEST_NEAR("Test prop correct prop1>0.8", prop1, 1.0, 0.2);
}

TESTMAIN(test_lda);
