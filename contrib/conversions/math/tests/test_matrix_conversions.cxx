#include <math/vbl_array_to_vnl_matrix.h>
#include <math/vbl_array_to_vnl_vector.h>
#include <math/vnl_matrix_to_vbl_array.h>
#include <math/vnl_vector_to_vbl_array.h>

#include <testlib/testlib_test.h>

void test_matrix_conversions()
{
  int data[] = { 1, 2, 3, 2, 4, 6, 3, 6, 9 };
  vbl_array_2d<int> m1(3,3);
  for (int i=0; i<9; ++i) m1[0][i] = data[i];

  vnl_matrix<int> m2 = vbl_array_to_vnl_matrix(m1);
  TEST("vbl_array_to_vnl_matrix", m2.rows(), 3);
  TEST("vbl_array_to_vnl_matrix", m2.columns(), 3);
  TEST("vbl_array_to_vnl_matrix", m2[1][2], 6);
  vbl_array_2d<int> m3 = vnl_matrix_to_vbl_array(m2);
  TEST("vnl_matrix_to_vbl_array", m3(1,1), 4);
  TEST("vnl_matrix_to_vbl_array", m3, m1);

  vbl_array_1d<int> v1(data,data+9);
  vnl_vector<int> v2 = vbl_array_to_vnl_vector(v1);
  TEST("vbl_array_to_vnl_vector", v2.size(), 9);
  TEST("vbl_array_to_vnl_vector", v2[1], 2);
  vbl_array_1d<int> v3 = vnl_vector_to_vbl_array(v2);
  TEST("vnl_vector_to_vbl_array", v3[2], 3);
  TEST("vnl_vector_to_vbl_array", v3, v1);
}

TESTMAIN(test_matrix_conversions);
