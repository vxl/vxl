#include "mvl_linear_backproject.h"

vnl_vector<double> mvl_linear_backproject(
          // camera matrices:
          vcl_vector<PMatrix> Ps,
          // image coordinates
          HomgPoint2D const *imgcoords,
          // world point:
          HomgPoint3D &X,
          // if true (default false), use current value of X to commute weights.
          bool re_weighted,
          // if 'e' (default) then use eigenvalue method if 'l' use least-squares (see Hartley & Sturm 'Triangulation' paper)
          char method)
{
  int numviews = Ps.size();
  vnl_vector<double> weights(numviews, 0);
  vnl_matrix<double> A(numviews*2, 4);

  // there are the two rows of the least squares problem for each view:
  vnl_vector<double> row0(4), row1(4);

  // i is the view index
  for (int i=0, j=0; i < numviews; i++, j+=2)
  {
    vnl_matrix<double> const &P = Ps[i].get_matrix();

    row0[0] = imgcoords[i].x()*P(2,0) - P(0,0);
    row0[1] = imgcoords[i].x()*P(2,1) - P(0,1);
    row0[2] = imgcoords[i].x()*P(2,2) - P(0,2);
    row0[3] = imgcoords[i].x()*P(2,3) - P(0,3);

    row1[0] = imgcoords[i].y()*P(2,0) - P(1,0);
    row1[1] = imgcoords[i].y()*P(2,1) - P(1,1);
    row1[2] = imgcoords[i].y()*P(2,2) - P(1,2);
    row1[3] = imgcoords[i].y()*P(2,3) - P(1,3);

    // normalisation :
    if (re_weighted) // calculate weight as p3*X
    {
      //weights[i] = P(2,0)*X.x() + P(2,1)*X.y() + P(2,2)*X.z() + P(2,3)*X.w();
      weights[i] = dot_product(P.get_row(2), X.get_vector()); // same as above
      row0 /= weights[i];
      row1 /= weights[i];
    }
    else
    {
      row0.normalize(); // normalise rows to provide better conditioning
      row1.normalize();
    }

    A.set_row(j, row0);
    A.set_row(j+1, row1);
  }


  if (method == 'e')
  {
    vnl_svd<double> svd(A);
    X.set(svd.nullvector());
  }
  else if (method == 'l')
  {
    vnl_vector<double> b =  -A.get_column(3);
    vnl_matrix<double> newA = A.extract(4, 3);
    vnl_svd<double> svd(newA);
    vnl_vector<double> b2 = svd.U().transpose() * b;

    vnl_vector<double> y(3);
    vnl_vector<double> D = svd.W().diagonal();

    for (int i = 0; i < 3; i++)
      y[i] = b2[i]/D[i];

    vnl_vector<double> tempX = svd.V()*y;
    X.set(tempX[0], tempX[1], tempX[2], 1.0);
  }
  else
  {
    vcl_cerr << "\nError: mvl_linear_backproject method must be 'e' or 'l'\n";
    vcl_exit(0);
  }

  return weights;
}
