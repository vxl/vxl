// This is gel/vmal/vmal_homog2d.cxx
#include "vmal_homog2d.h"
#include <vnl/algo/vnl_svd.h>

vmal_homog2d::vmal_homog2d() = default;

vmal_homog2d::~vmal_homog2d() = default;

void vmal_homog2d::compute_homo(const std::vector<vnl_double_3 > &pima1,
                                const std::vector<vnl_double_3 > &pima2,
                                vnl_double_3x3 &hmatrix)
{
  int numpoints=pima1.size();
  vnl_matrix<double> A(2*numpoints,9);
  A.fill(0.0);
  for (int row=0,i=0; i<numpoints; ++i)
  {
    A(row, 0) = pima1[i][0] * pima2[i][2];
    A(row, 1) = pima1[i][1] * pima2[i][2];
    A(row, 2) = pima1[i][2] * pima2[i][2];
    A(row, 3) = 0;
    A(row, 4) = 0;
    A(row, 5) = 0;
    A(row, 6) = -pima1[i][0] * pima2[i][0];
    A(row, 7) = -pima1[i][1] * pima2[i][0];
    A(row, 8) = -pima1[i][2] * pima2[i][0];
    ++row;

    A(row, 0) = 0;
    A(row, 1) = 0;
    A(row, 2) = 0;
    A(row, 3) = pima1[i][0] * pima2[i][2];
    A(row, 4) = pima1[i][1] * pima2[i][2];
    A(row, 5) = pima1[i][2] * pima2[i][2];
    A(row, 6) = -pima1[i][0] * pima2[i][1];
    A(row, 7) = -pima1[i][1] * pima2[i][1];
    A(row, 8) = -pima1[i][2] * pima2[i][1];
    ++row;
  }

  A.normalize_rows();
  vnl_svd<double> SVD(A);
  vnl_vector<double> x=SVD.nullvector();
    hmatrix[0][0]=x[0]; hmatrix[0][1]=x[1]; hmatrix[0][2]=x[2];
  hmatrix[1][0]=x[3]; hmatrix[1][1]=x[4]; hmatrix[1][2]=x[5];
  hmatrix[2][0]=x[6]; hmatrix[2][1]=x[7]; hmatrix[2][2]=x[8];
}
