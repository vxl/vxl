// This is mul/mbl/tests/test_k_means.cxx
#include <vcl_iostream.h>
#include <vcl_cmath.h> 
#include <vcl_cassert.h>
#include <vcl_vector.h>

#include <mbl/mbl_lda.h>

#include <vpdfl/vpdfl_axis_gaussian_sampler.h>
#include <vpdfl/vpdfl_axis_gaussian.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matlab_print.h>

#include <testlib/testlib_test.h>

void arr2vec(vnl_vector<double>& vector,
                          int nx, 
                          const double *double_array)
{  
  vector.resize(nx);
  for (int x=0;x<nx;++x)
    vector(x) = double_array[x];
  
}

void vec2mat(vnl_matrix<double>& matrix,
                const vcl_vector< vnl_vector<double> >& list_vec)
{
  int n_rows= list_vec.size();
  int n_cols= list_vec[0].size();

  matrix.resize( n_rows, n_cols );
  for (int i=0; i< n_rows; ++i)
  {
    assert ( list_vec[i].size() == n_cols ); 
    for (int j=0; j< n_cols; ++j)
      matrix(i,j)=list_vec[i](j);
    
  }
}

void add_data( vcl_vector< vnl_vector<double> >& d_vec,
                vcl_vector< int >& l_vec,
                vcl_vector< vnl_vector<double> >& new_data,
                int label)

{
  int n=new_data.size();
  for (int i=0; i<n; ++i)
  {
    d_vec.push_back( new_data[i] );
    l_vec.push_back ( label ); 
  }

}
                


void test_lda()
{
  vcl_cout << "\n*********************\n"
           <<   " Testing mbl_lda \n"
           <<   "*********************\n";


  //create 2 gaussian distributions
  vnl_vector<double> m0,v0,m1,v1;
  vpdfl_axis_gaussian g0,g1;

   // Create g0
  double m0_array[2] = {8, 6};
  arr2vec(m0,2,m0_array);
  double v0_array[2] = {3, 2};
  arr2vec(v0,2,v0_array);
  g0.set( m0, v0 );

   // Create g1
  double m1_array[2] = {1, 2};
  arr2vec(m1,2,m1_array);
  double v1_array[2] = {20, 30};
  arr2vec(v1,2,v1_array);
  g1.set( m1, v1 );

 
  //sample some data
  vpdfl_axis_gaussian_sampler s0,s1;
  s0.set_model(g0);
  s1.set_model(g1);
  
  int n_data=50;
  vcl_vector< vnl_vector<double> >d0(n_data),d1(n_data);
  for (int i=0;i<n_data;++i)
  {
    s0.sample( d0[i] );
    s1.sample( d1[i] );
  }

  vnl_matrix<double> mat0;
  vec2mat( mat0, d0 );
  vcl_ofstream of0( "c:\\temp\\mat0.txt" );
  vnl_matlab_print(of0,mat0);

  vnl_matrix<double> mat1;
  vec2mat( mat1, d1 );
  vcl_ofstream of1( "c:\\temp\\mat1.txt" );
  vnl_matlab_print(of1,mat1);

  //concatenate data + make labels
  vcl_vector< vnl_vector<double> > d_vec(0);
  vcl_vector< int > l_vec(0);
  add_data ( d_vec, l_vec, d0, 0);
  add_data ( d_vec, l_vec, d1, 1);
  
  mbl_lda lda;
  lda.build( d_vec, l_vec);
  
  int c_m0= lda.classify( m0 );
  vcl_cout<<"c_m0= "<<c_m0<<vcl_endl;
  TEST("Test classifying mean 0", c_m0==0 , true);
  
  int c_m1= lda.classify( m1 );
  vcl_cout<<"c_m1= "<<c_m1<<vcl_endl;
  TEST("Test classifying mean 1", c_m1==1 , true);

  // Create some test data
  vcl_vector< vnl_vector<double> >test_d0(n_data),test_d1(n_data);
  for (int i=0;i<n_data;++i)
  {
    s0.sample( test_d0[i] );
    s1.sample( test_d1[i] );
  }


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

  
  vcl_cout<<"c0 %correct= "<< prop0<<vcl_endl;
  vcl_cout<<"c1 %correct= "<< prop1<<vcl_endl;

  TEST("Test prop correct prop0>0.9", prop0>0.9 , true);
  TEST("Test prop correct prop1>0.8", prop1>0.8 , true);

}

TESTLIB_DEFINE_MAIN(test_lda);
