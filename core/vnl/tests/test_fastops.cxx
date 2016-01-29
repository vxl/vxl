// This is core/vnl/tests/test_fastops.cxx
#include <testlib/testlib_test.h>
// :
// \file
// \author Peter Vanroose
// \date 17 June 2004
#include <vnl/vnl_fastops.h>

void test_fastops()
{
  // The data to work with
  vnl_matrix<double> result_m, X; vnl_vector<double> result_v, Y;
  vnl_matrix<double> id1x1(1, 1); id1x1.set_identity();
  vnl_matrix<double> id2x2(2, 2); id2x2.set_identity();
  vnl_matrix<double> id3x3(3, 3); id3x3.set_identity();
  vnl_matrix<double> id10x10(10, 10); id10x10.set_identity();
  vnl_matrix<double> id99x99(99, 99); id99x99.set_identity();
  double             data[10] = { -7.5, 1.5, 0., -107.25, 511., -509.75, 1.25, -1., 0., 1. };
  vnl_matrix<double> m1x1(data, 1, 1); // the number -7.5
  vnl_matrix<double> m2x2(data, 2, 2);
  vnl_matrix<double> m2x2t = m2x2.transpose();
  vnl_matrix<double> m3x3(data, 3, 3);
  vnl_matrix<double> m3x3t = m3x3.transpose();
  vnl_vector<double> v1(data, 1); // the number -7.5
  vnl_vector<double> v2(data, 2);
  vnl_vector<double> v3(data, 3);
  vnl_vector<double> v10(data, 10);
  vnl_matrix<double> m10x2(10, 2), m2x10(2, 10);
  for( unsigned int i = 0; i < 10; ++i )
    {
    for( unsigned int j = 0; j < 2; ++j )
      {
      m10x2[i][j] = m2x10[j][i] = (i + 1) * 1.5 + (j + 1) * (j + i);
      }
    }

  // First test: $I \times I$
  result_m.set_size(1, 1);
  vnl_fastops::AtA(result_m, id1x1);
  TEST("vnl_fastops::AtA(id1x1)", result_m, id1x1);
  vnl_fastops::AB(result_m, id1x1, id1x1);
  TEST("vnl_fastops::AB(id1x1,id1x1)", result_m, id1x1);
  vnl_fastops::AtB(result_m, id1x1, id1x1);
  TEST("vnl_fastops::AtB(id1x1,id1x1)", result_m, id1x1);
  vnl_fastops::ABt(result_m, id1x1, id1x1);
  TEST("vnl_fastops::ABt(id1x1,id1x1)", result_m, id1x1);
  result_m.set_size(2, 2);
  vnl_fastops::AtA(result_m, id2x2);
  TEST("vnl_fastops::AtA(id2x2)", result_m, id2x2);
  vnl_fastops::AB(result_m, id2x2, id2x2);
  TEST("vnl_fastops::AB(id2x2,id2x2)", result_m, id2x2);
  vnl_fastops::AtB(result_m, id2x2, id2x2);
  TEST("vnl_fastops::AtB(id2x2,id2x2)", result_m, id2x2);
  vnl_fastops::ABt(result_m, id2x2, id2x2);
  TEST("vnl_fastops::ABt(id2x2,id2x2)", result_m, id2x2);
  result_m.set_size(99, 99);
  vnl_fastops::AtA(result_m, id99x99);
  TEST("vnl_fastops::AtA(id99x99)", result_m, id99x99);
  vnl_fastops::AB(result_m, id99x99, id99x99);
  TEST("vnl_fastops::AB(id99x99,id99x99)", result_m, id99x99);
  vnl_fastops::AtB(result_m, id99x99, id99x99);
  TEST("vnl_fastops::AtB(id99x99,id99x99)", result_m, id99x99);
  vnl_fastops::ABt(result_m, id99x99, id99x99);
  TEST("vnl_fastops::ABt(id99x99,id99x99)", result_m, id99x99);

  // Second test: $I \times M$ and $M \times I$ and $M^\top \times I$
  result_m.set_size(1, 1); result_v.set_size(1);
  vnl_fastops::AB(result_m, id1x1, m1x1);
  TEST("vnl_fastops::AB(id1x1,m1x1)", result_m, m1x1);
  vnl_fastops::AB(result_m, m1x1, id1x1);
  TEST("vnl_fastops::AB(m1x1,id1x1)", result_m, m1x1);
  vnl_fastops::AtB(result_v, id1x1, v1);
  TEST("vnl_fastops::AtB(id1x1,v1)", result_v, v1);
  vnl_fastops::AtB(result_m, id1x1, m1x1);
  TEST("vnl_fastops::AtB(id1x1,m1x1)", result_m, m1x1);
  vnl_fastops::ABt(result_m, m1x1, id1x1);
  TEST("vnl_fastops::ABt(m1x1,id1x1)", result_m, m1x1);
  vnl_fastops::AtB(result_m, m1x1, id1x1);
  TEST("vnl_fastops::AtB(m1x1,id1x1)", result_m, m1x1);
  vnl_fastops::ABt(result_m, id1x1, m1x1);
  TEST("vnl_fastops::ABt(id1x1,m1x1)", result_m, m1x1);
  result_m.set_size(2, 2); result_v.set_size(2);
  vnl_fastops::AB(result_m, id2x2, m2x2);
  TEST("vnl_fastops::AB(id2x2,m2x2)", result_m, m2x2);
  vnl_fastops::AB(result_m, m2x2, id2x2);
  TEST("vnl_fastops::AB(m2x2,id2x2)", result_m, m2x2);
  vnl_fastops::AtB(result_v, id2x2, v2);
  TEST("vnl_fastops::AtB(id2x2,v2)", result_v, v2);
  vnl_fastops::AtB(result_m, id2x2, m2x2);
  TEST("vnl_fastops::AtB(id2x2,m2x2)", result_m, m2x2);
  vnl_fastops::ABt(result_m, m2x2, id2x2);
  TEST("vnl_fastops::ABt(m2x2,id2x2)", result_m, m2x2);
  vnl_fastops::AtB(result_m, m2x2, id2x2);
  TEST("vnl_fastops::AtB(m2x2,id2x2)", result_m, m2x2t);
  vnl_fastops::ABt(result_m, id2x2, m2x2);
  TEST("vnl_fastops::ABt(id2x2,m2x2)", result_m, m2x2t);
  result_m.set_size(3, 3); result_v.set_size(3);
  vnl_fastops::AB(result_m, id3x3, m3x3);
  TEST("vnl_fastops::AB(id3x3,m3x3)", result_m, m3x3);
  vnl_fastops::AB(result_m, m3x3, id3x3);
  TEST("vnl_fastops::AB(m3x3,id3x3)", result_m, m3x3);
  vnl_fastops::AtB(result_v, id3x3, v3);
  TEST("vnl_fastops::AtB(id3x3,v3)", result_v, v3);
  vnl_fastops::AtB(result_m, id3x3, m3x3);
  TEST("vnl_fastops::AtB(id3x3,m3x3)", result_m, m3x3);
  vnl_fastops::ABt(result_m, m3x3, id3x3);
  TEST("vnl_fastops::ABt(m3x3,id3x3)", result_m, m3x3);
  vnl_fastops::AtB(result_m, m3x3, id3x3);
  TEST("vnl_fastops::AtB(m3x3,id3x3)", result_m, m3x3t);
  vnl_fastops::ABt(result_m, id3x3, m3x3);
  TEST("vnl_fastops::ABt(id3x3,m3x3)", result_m, m3x3t);
  result_v.set_size(10);
  vnl_fastops::AtB(result_v, id10x10, v10);
  TEST("vnl_fastops::AtB(id10x10,v10)", result_v, v10);

  // Third test: $M \times M$ and $M^\top \times M$ and $M \times M^\top$
  result_m.set_size(1, 1); result_v.set_size(1);
  vnl_fastops::AtA(result_m, m1x1);
  TEST("vnl_fastops::AtA(m1x1)", result_m, m1x1 * m1x1);
  vnl_fastops::AB(result_m, m1x1, m1x1);
  TEST("vnl_fastops::AB(m1x1,m1x1)", result_m, m1x1 * m1x1);
  vnl_fastops::AtB(result_v, m1x1, v1);
  TEST("vnl_fastops::AtB(m1x1,v1)", result_v, m1x1 * v1);
  vnl_fastops::AtB(result_m, m1x1, m1x1);
  TEST("vnl_fastops::AtB(m1x1,m1x1)", result_m, m1x1 * m1x1);
  vnl_fastops::ABt(result_m, m1x1, m1x1);
  TEST("vnl_fastops::ABt(m1x1,m1x1)", result_m, m1x1 * m1x1);
  result_m.set_size(2, 2); result_v.set_size(2);
  vnl_fastops::AtA(result_m, m2x2);
  TEST("vnl_fastops::AtA(m2x2)", result_m, m2x2t * m2x2);
  vnl_fastops::AB(result_m, m2x2, m2x2);
  TEST("vnl_fastops::AB(m2x2,m2x2)", result_m, m2x2 * m2x2);
  vnl_fastops::AtB(result_v, m2x2, v2);
  TEST("vnl_fastops::AtB(m2x2,v2)", result_v, m2x2t * v2);
  vnl_fastops::AtB(result_m, m2x2, m2x2);
  TEST("vnl_fastops::AtB(m2x2,m2x2)", result_m, m2x2t * m2x2);
  vnl_fastops::ABt(result_m, m2x2, m2x2);
  TEST("vnl_fastops::ABt(m2x2,m2x2)", result_m, m2x2 * m2x2t);
  result_m.set_size(3, 3); result_v.set_size(3);
  vnl_fastops::AtA(result_m, m3x3);
  TEST("vnl_fastops::AtA(m3x3)", result_m, m3x3t * m3x3);
  vnl_fastops::AB(result_m, m3x3, m3x3);
  TEST("vnl_fastops::AB(m3x3,m3x3)", result_m, m3x3 * m3x3);
  vnl_fastops::AtB(result_v, m3x3, v3);
  TEST("vnl_fastops::AtB(m3x3,v3)", result_v, m3x3t * v3);
  vnl_fastops::AtB(result_m, m3x3, m3x3);
  TEST("vnl_fastops::AtB(m3x3,m3x3)", result_m, m3x3t * m3x3);
  vnl_fastops::ABt(result_m, m3x3, m3x3);
  TEST("vnl_fastops::ABt(m3x3,m3x3)", result_m, m3x3 * m3x3t);
  result_m.set_size(2, 2);
  vnl_fastops::AB(result_m, m2x10, m10x2);
  TEST("vnl_fastops::AB(m2x10,m10x2)", result_m, m2x10 * m10x2);
  vnl_fastops::AtB(result_m, m10x2, m10x2);
  TEST("vnl_fastops::AtB(m10x2,m10x2)", result_m, m2x10 * m10x2);
  vnl_fastops::ABt(result_m, m2x10, m2x10);
  TEST("vnl_fastops::ABt(m2x10,m2x10)", result_m, m2x10 * m10x2);
  result_m.set_size(10, 10);
  vnl_fastops::AB(result_m, m10x2, m2x10);
  TEST("vnl_fastops::AB(m10x2,m2x10)", result_m, m10x2 * m2x10);
  vnl_fastops::AtB(result_m, m2x10, m2x10);
  TEST("vnl_fastops::AtB(m2x10,m2x10)", result_m, m10x2 * m2x10);
  vnl_fastops::ABt(result_m, m10x2, m10x2);
  TEST("vnl_fastops::ABt(m10x2,m10x2)", result_m, m10x2 * m2x10);
  result_v.set_size(2);
  vnl_fastops::AtB(result_v, m10x2, v10);
  TEST("vnl_fastops::AtB(m10x2,v10)", result_v, m2x10 * v10);
  result_v.set_size(10);
  vnl_fastops::AtB(result_v, m2x10, v2);
  TEST("vnl_fastops::AtB(m2x10,v2)", result_v, m10x2 * v2);

  // Fourth test: increments and decrements
  X = m1x1; Y = v1;
  vnl_fastops::inc_X_by_AtA(X, m1x1);
  TEST("vnl_fastops::inc_X_by_AtA(X, m1x1)", X, m1x1 * m1x1 + m1x1);
  vnl_fastops::dec_X_by_AtA(X, m1x1);
  TEST("vnl_fastops::dec_X_by_AtA(X, m1x1)", X, m1x1);
  vnl_fastops::inc_X_by_AB(X, m1x1, m1x1);
  TEST("vnl_fastops::inc_X_by_AB(X, m1x1,m1x1)", X, m1x1 * m1x1 + m1x1);
  vnl_fastops::dec_X_by_AB(X, m1x1, m1x1);
  TEST("vnl_fastops::dec_X_by_AB(X, m1x1,m1x1)", X, m1x1);
  vnl_fastops::inc_X_by_AtB(Y, m1x1, v1);
  TEST("vnl_fastops::inc_X_by_AtB(X, m1x1,v1)", Y, m1x1 * v1 + v1);
  vnl_fastops::dec_X_by_AtB(Y, m1x1, v1);
  TEST("vnl_fastops::dec_X_by_AtB(X, m1x1,v1)", Y, v1);
  vnl_fastops::inc_X_by_AtB(X, m1x1, m1x1);
  TEST("vnl_fastops::inc_X_by_AtB(X, m1x1,m1x1)", X, m1x1 * m1x1 + m1x1);
  vnl_fastops::dec_X_by_AtB(X, m1x1, m1x1);
  TEST("vnl_fastops::dec_X_by_AtB(X, m1x1,m1x1)", X, m1x1);
  vnl_fastops::inc_X_by_ABt(X, m1x1, m1x1);
  TEST("vnl_fastops::inc_X_by_ABt(X, m1x1,m1x1)", X, m1x1 * m1x1 + m1x1);
  vnl_fastops::dec_X_by_ABt(X, m1x1, m1x1);
  TEST("vnl_fastops::dec_X_by_ABt(X, m1x1,m1x1)", X, m1x1);
  X = m2x2; Y = v2;
  vnl_fastops::inc_X_by_AtA(X, m2x2);
  TEST("vnl_fastops::inc_X_by_AtA(X, m2x2)", X, m2x2t * m2x2 + m2x2);
  vnl_fastops::dec_X_by_AtA(X, m2x2);
  TEST("vnl_fastops::dec_X_by_AtA(X, m2x2)", X, m2x2);
  vnl_fastops::inc_X_by_AB(X, m2x2, m2x2);
  TEST("vnl_fastops::inc_X_by_AB(X, m2x2,m2x2)", X, m2x2 * m2x2 + m2x2);
  vnl_fastops::dec_X_by_AB(X, m2x2, m2x2);
  TEST("vnl_fastops::dec_X_by_AB(X, m2x2,m2x2)", X, m2x2);
  vnl_fastops::inc_X_by_AtB(Y, m2x2, v2);
  TEST("vnl_fastops::inc_X_by_AtB(X, m2x2,v2)", Y, m2x2t * v2 + v2);
  vnl_fastops::dec_X_by_AtB(Y, m2x2, v2);
  TEST("vnl_fastops::dec_X_by_AtB(X, m2x2,v2)", Y, v2);
  vnl_fastops::inc_X_by_AtB(X, m2x2, m2x2);
  TEST("vnl_fastops::inc_X_by_AtB(X, m2x2,m2x2)", X, m2x2t * m2x2 + m2x2);
  vnl_fastops::dec_X_by_AtB(X, m2x2, m2x2);
  TEST("vnl_fastops::dec_X_by_AtB(X, m2x2,m2x2)", X, m2x2);
  vnl_fastops::inc_X_by_ABt(X, m2x2, m2x2);
  TEST("vnl_fastops::inc_X_by_ABt(X, m2x2,m2x2)", X, m2x2 * m2x2t + m2x2);
  vnl_fastops::dec_X_by_ABt(X, m2x2, m2x2);
  TEST("vnl_fastops::dec_X_by_ABt(X, m2x2,m2x2)", X, m2x2);
  X = m3x3; Y = v3;
  vnl_fastops::inc_X_by_AtA(X, m3x3);
  TEST("vnl_fastops::inc_X_by_AtA(X, m3x3)", X, m3x3t * m3x3 + m3x3);
  vnl_fastops::dec_X_by_AtA(X, m3x3);
  TEST("vnl_fastops::dec_X_by_AtA(X, m3x3)", X, m3x3);
  vnl_fastops::inc_X_by_AB(X, m3x3, m3x3);
  TEST("vnl_fastops::inc_X_by_AB(X, m3x3,m3x3)", X, m3x3 * m3x3 + m3x3);
  vnl_fastops::dec_X_by_AB(X, m3x3, m3x3);
  TEST("vnl_fastops::dec_X_by_AB(X, m3x3,m3x3)", X, m3x3);
  vnl_fastops::inc_X_by_AtB(Y, m3x3, v3);
  TEST("vnl_fastops::inc_X_by_AtB(X, m3x3,v3)", Y, m3x3t * v3 + v3);
  vnl_fastops::dec_X_by_AtB(Y, m3x3, v3);
  TEST("vnl_fastops::dec_X_by_AtB(X, m3x3,v3)", Y, v3);
  vnl_fastops::inc_X_by_AtB(X, m3x3, m3x3);
  TEST("vnl_fastops::inc_X_by_AtB(X, m3x3,m3x3)", X, m3x3t * m3x3 + m3x3);
  vnl_fastops::dec_X_by_AtB(X, m3x3, m3x3);
  TEST("vnl_fastops::dec_X_by_AtB(X, m3x3,m3x3)", X, m3x3);
  vnl_fastops::inc_X_by_ABt(X, m3x3, m3x3);
  TEST("vnl_fastops::inc_X_by_ABt(X, m3x3,m3x3)", X, m3x3 * m3x3t + m3x3);
  vnl_fastops::dec_X_by_ABt(X, m3x3, m3x3);
  TEST("vnl_fastops::dec_X_by_ABt(X, m3x3,m3x3)", X, m3x3);
  X = m2x2;
  vnl_fastops::inc_X_by_AB(X, m2x10, m10x2);
  TEST("vnl_fastops::inc_X_by_AB(X, m2x10,m10x2)", X, m2x10 * m10x2 + m2x2);
  vnl_fastops::dec_X_by_AB(X, m2x10, m10x2);
  TEST("vnl_fastops::dec_X_by_AB(X, m2x10,m10x2)", X, m2x2);
  vnl_fastops::inc_X_by_AtB(X, m10x2, m10x2);
  TEST("vnl_fastops::inc_X_by_AtB(X, m10x2,m10x2)", X, m2x10 * m10x2 + m2x2);
  vnl_fastops::dec_X_by_AtB(X, m10x2, m10x2);
  TEST("vnl_fastops::dec_X_by_AtB(X, m10x2,m10x2)", X, m2x2);
  vnl_fastops::inc_X_by_ABt(X, m2x10, m2x10);
  TEST("vnl_fastops::inc_X_by_ABt(X, m2x10,m2x10)", X, m2x10 * m10x2 + m2x2);
  vnl_fastops::dec_X_by_ABt(X, m2x10, m2x10);
  TEST("vnl_fastops::dec_X_by_ABt(X, m2x10,m2x10)", X, m2x2);
  X = m10x2 * m2x10;
  vnl_fastops::inc_X_by_AB(X, m10x2, m2x10);
  TEST("vnl_fastops::inc_X_by_AB(X, m10x2,m2x10)", X, m10x2 * m2x10 * 2);
  vnl_fastops::dec_X_by_AB(X, m10x2, m2x10);
  TEST("vnl_fastops::dec_X_by_AB(X, m10x2,m2x10)", X, m10x2 * m2x10);
  vnl_fastops::inc_X_by_AtB(X, m2x10, m2x10);
  TEST("vnl_fastops::inc_X_by_AtB(X, m2x10,m2x10)", X, m10x2 * m2x10 * 2);
  vnl_fastops::dec_X_by_AtB(X, m2x10, m2x10);
  TEST("vnl_fastops::dec_X_by_AtB(X, m2x10,m2x10)", X, m10x2 * m2x10);
  vnl_fastops::inc_X_by_ABt(X, m10x2, m10x2);
  TEST("vnl_fastops::inc_X_by_ABt(X, m10x2,m10x2)", X, m10x2 * m2x10 * 2);
  vnl_fastops::dec_X_by_ABt(X, m10x2, m10x2);
  TEST("vnl_fastops::dec_X_by_ABt(X, m10x2,m10x2)", X, m10x2 * m2x10);
  Y = v2;
  vnl_fastops::inc_X_by_AtB(Y, m10x2, v10);
  TEST("vnl_fastops::inc_X_by_AtB(X, m10x2,v10)", Y, m2x10 * v10 + v2);
  vnl_fastops::dec_X_by_AtB(Y, m10x2, v10);
  TEST("vnl_fastops::dec_X_by_AtB(X, m10x2,v10)", Y, v2);
  Y = v10;
  vnl_fastops::inc_X_by_AtB(Y, m2x10, v2);
  TEST("vnl_fastops::inc_X_by_AtB(X, m2x10,v2)", Y, m10x2 * v2 + v10);
  vnl_fastops::dec_X_by_AtB(Y, m2x10, v2);
  TEST("vnl_fastops::dec_X_by_AtB(X, m2x10,v2)", Y, v10);
}

TESTMAIN(test_fastops);
