//-*- c++ -*-------------------------------------------------------------------
// Module: Hyperplane fit using orthogonal regression
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 31 Aug 96
// Converted to vxl by Peter Vanroose, February 2000
//-----------------------------------------------------------------------------

#include <vcl/vcl_iostream.h>
#include <vnl/vnl_matops.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

int main()
{
  // Read points from stdin
  vnl_matrix<double> pts;
  cin >> pts;

  // Build design matrix D
  int npts = pts.rows();
  int dim = pts.columns();
  vnl_matrix<double> D(npts, dim+1);
  for(int i = 0; i < npts; ++i) {
    for(int j = 0; j < dim; ++j) D(i,j) = pts(i,j);
    D(i,dim) = 1;
  }

  // 1. Compute using SVD
  {
    vnl_svd<double> svd(D);
    vnl_vector<double> a = svd.nullvector();
    cout << "SVD residual = " << (D * a).magnitude() << endl;
  }

  // 2. Compute using eigensystem of D'*D
  {
    vnl_symmetric_eigensystem<double> eig(D.transpose() * D);
    vnl_vector<double> a = eig.get_eigenvector(0);
    cout << "Eig residual = " << (D * a).magnitude() << endl;
  }

  return 0;
}
