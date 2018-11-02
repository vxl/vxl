#include <iostream>
#include <testlib/testlib_test.h>

#ifdef HAS_NO_COOL

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
void test_CoolMatrix_conversions()
{
  std::cerr << "test not run since you do not have the TargetJr COOL package\n";
}

#else
#include <math/CoolMatrix_to_vnl_matrix.h>
#include <math/CoolVector_to_vnl_vector.h>
#include <math/vnl_matrix_to_CoolMatrix.h>
#include <math/vnl_vector_to_CoolVector.h>

void test_CoolMatrix_conversions()
{
  int data[] = { 1, 2, 3, 2, 4, 6, 3, 6, 9 };
  CoolMatrix<int> m1(data,3,3);
  vnl_matrix<int> m2 = CoolMatrix_to_vnl_matrix(m1);
  TEST("CoolMatrix_to_vnl_matrix", m2.rows(), 3);
  TEST("CoolMatrix_to_vnl_matrix", m2.columns(), 3);
  TEST("CoolMatrix_to_vnl_matrix", m2[1][2], 6);
  CoolMatrix<int> m3 = vnl_matrix_to_CoolMatrix(m2);
  TEST("vnl_matrix_to_CoolMatrix", m3(1,1), 4);
  TEST("vnl_matrix_to_CoolMatrix", m3, m1);

  CoolVector<int> v1(data,9);
  vnl_vector<int> v2 = CoolVector_to_vnl_vector(v1);
  TEST("CoolVector_to_vnl_vector", v2.size(), 9);
  TEST("CoolVector_to_vnl_vector", v2[1], 2);
  CoolVector<int> v3 = vnl_vector_to_CoolVector(v2);
  TEST("vnl_vector_to_CoolVector", v3(2), 3);
  TEST("vnl_vector_to_CoolVector", v3, v1);
}

#endif // HAS_NO_COOL

TESTMAIN(test_CoolMatrix_conversions);
