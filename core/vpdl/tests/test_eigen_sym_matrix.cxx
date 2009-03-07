#include <testlib/testlib_test.h>
#include <vpdl/vpdt/vpdt_eigen_sym_matrix.h>
#include <vcl_iostream.h>
#include <vnl/vnl_matrix_fixed.h>

#include <vnl/vnl_diag_matrix.h>

template <class T>
void test_matrix_type(T epsilon, const vcl_string& type_name)
{
  // generate a test matrix
  vnl_matrix_fixed<T,3,3> M(T(0));
  M(0,0) = (T)0.5; M(0,1) = (T)0.2; M(0,2) = (T)0.1;
  M(1,0) = (T)0.2; M(1,1) = (T)0.3; M(1,2) = (T)0.1;
  M(2,0) = (T)0.1; M(2,1) = (T)0.1; M(2,2) = (T)0.4;
  
  T det = (T)0.04;
  
  // the inverse of M
  vnl_matrix_fixed<T,3,3> invM(T(0));
  invM(0,0) = (T)2.75;  invM(0,1) = (T)-1.75; invM(0,2) = (T)-0.25;
  invM(1,0) = (T)-1.75; invM(1,1) = (T)4.75;  invM(1,2) = (T)-0.75;
  invM(2,0) = (T)-0.25; invM(2,1) = (T)-0.75; invM(2,2) = (T)2.75;
  
  vnl_vector_fixed<T,3> x(T(1), T(2), T(-1));
  
  {
    vcl_cout <<"=======================================\n";
    vpdt_eigen_sym_matrix<T,3> sym(M);
    vnl_matrix_fixed<T,3,3> M2;
    sym.form_matrix(M2);
    TEST_NEAR(("reform matrix <"+type_name+"> fixed").c_str(), 
              (M-M2).array_inf_norm(), 0, epsilon);
    sym.form_inverse(M2);
    TEST_NEAR(("inverse matrix <"+type_name+"> fixed").c_str(), 
              (invM-M2).array_inf_norm(), 0, epsilon);
    TEST_NEAR(("determinant <"+type_name+"> fixed").c_str(), 
              sym.determinant(), det, epsilon);
    
    TEST_NEAR(("quad form <"+type_name+"> fixed").c_str(), 
              sym.quad_form(x), dot_product(x,M*x), epsilon);
    TEST_NEAR(("inverse quad form <"+type_name+"> fixed").c_str(), 
              sym.inverse_quad_form(x), dot_product(x,invM*x), epsilon);
  }
  
  {
    vcl_cout <<"=======================================\n";
    vpdt_eigen_sym_matrix<T> sym(M);
    vnl_matrix<T> M2;
    sym.form_matrix(M2);
    TEST_NEAR(("reform matrix <"+type_name+"> variable").c_str(), 
              (M-M2).array_inf_norm(), 0, epsilon);
    sym.form_inverse(M2);
    TEST_NEAR(("inverse matrix <"+type_name+"> variable").c_str(), 
              (invM-M2).array_inf_norm(), 0, epsilon);
    TEST_NEAR(("determinant <"+type_name+"> variable").c_str(), 
              sym.determinant(), det, epsilon);
    
    TEST_NEAR(("quad form <"+type_name+"> variable").c_str(), 
              sym.quad_form(x), dot_product(x,M*x), epsilon);
    TEST_NEAR(("inverse quad form <"+type_name+"> variable").c_str(), 
              sym.inverse_quad_form(x), dot_product(x,invM*x), epsilon);
  }
  
  {
    vcl_cout <<"=======================================\n";
    vpdt_eigen_sym_matrix<T,1> sym(T(2));
    T M2;
    sym.form_matrix(M2);
    TEST_NEAR(("reform matrix <"+type_name+"> scalar").c_str(), 
              M2, T(2), epsilon);
    sym.form_inverse(M2);
    TEST_NEAR(("inverse matrix <"+type_name+"> scalar").c_str(), 
              M2, T(0.5), epsilon);
    TEST_NEAR(("determinant <"+type_name+"> scalar").c_str(), 
              sym.determinant(), 2, epsilon);
    
    TEST_NEAR(("quad form <"+type_name+"> scalar").c_str(), 
              sym.quad_form(3), 18, epsilon);
    TEST_NEAR(("inverse quad form <"+type_name+"> scalar").c_str(), 
              sym.inverse_quad_form(3), 4.5, epsilon);
    
    TEST("implicit cast to scalar", sym, T(2));
    sym = T(3);
    TEST("assignment from scalar", sym, T(3));
  }

}

MAIN( test_eigen_sym_matrix )
{
  START ("Eigen-decomposed Symmetric Matrix");
  test_matrix_type(float(1e-5),"float");
  test_matrix_type(double(1e-13),"double");
  SUMMARY();
}

