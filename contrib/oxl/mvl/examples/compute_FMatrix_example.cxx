// Module: FMatrixCompute example
// Author: Andrew W. Fitzgibbon, Oxford RRG, July 96
// Adapted to mvl by Peter Vanroose, January 2002.

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <mvl/FMatrix.h>
#include <mvl/HomgOperator2D.h>
#include <mvl/FMatrixComputeLinear.h>
#include <mvl/FMatrixCompute7Point.h>
#include <mvl/FMatrixComputeMLESAC.h>
#include <mvl/FMatrixComputeRANSAC.h>
#include <mvl/FMatrixComputeLMedSq.h>

int main(int argc, char**argv)
{
  vcl_vector<HomgPoint2D> points1;
  vcl_vector<HomgPoint2D> points2;
  bool ishomg = (argc > 1 && argv[1][0] == 'h');

  // Read points
  while (vcl_cin.good()) {
    points1.push_back(HomgPoint2D::read(vcl_cin,ishomg)); // Read 2 reals
    points2.push_back(HomgPoint2D::read(vcl_cin,ishomg)); // Read 2 reals
    vcl_cin >> vcl_ws; // Eat whitespace
  }

  {
    // Perform the fit using the non-normalized linear computor.
    FMatrixComputeLinear computor(false);
    FMatrix f = computor.compute(points1, points2);

    vcl_cout << "FMatrixComputeLinear:\nF = " << f << vcl_endl;

    double d = 0;
    for(unsigned int i = 0; i < points1.size(); ++i)
      d += f.image1_epipolar_distance_squared(&points1[i], &points2[i]);
    vcl_cout << "Error = " << d/points1.size() << vcl_endl;
  }

  {
    // Perform the fit using the non-normalised 7-point estimator.
    FMatrixCompute7Point computor(false);
    FMatrix f;
    vcl_vector<FMatrix*> l; l.push_back(&f);
    computor.compute(points1, points2, l);

    vcl_cout << "FMatrixCompute7Point:\nF = " << f << vcl_endl;

    double d = 0;
    for(unsigned int i = 0; i < points1.size(); ++i)
      d += f.image1_epipolar_distance_squared(&points1[i], &points2[i]);
    vcl_cout << "Error = " << d/points1.size() << vcl_endl;
  }

  {
    // Perform the fit using Phil Torr's Maximum Likelyhood Estimation.
    FMatrixComputeMLESAC computor(true,9);
    FMatrix f = computor.compute(points1, points2);

    vcl_cout << "FMatrixComputeMLESAC with rank truncation:\nF = " << f << vcl_endl;

    double d = 0;
    for(unsigned int i = 0; i < points1.size(); ++i)
      d += f.image1_epipolar_distance_squared(&points1[i], &points2[i]);
    vcl_cout << "Error = " << d/points1.size() << vcl_endl;
  }

  {
    // Perform the fit using Phil Torr's Robust Sampling Concensus
    FMatrixComputeRANSAC computor(true,9);
    FMatrix f = computor.compute(points1, points2);

    vcl_cout << "FMatrixComputeRANSAC with rank truncation:\nF = " << f << vcl_endl;

    double d = 0;
    for(unsigned int i = 0; i < points1.size(); ++i)
      d += f.image1_epipolar_distance_squared(&points1[i], &points2[i]);
    vcl_cout << "Error = " << d/points1.size() << vcl_endl;
  }

  {
    // Perform the fit using the normalized linear computor.
    FMatrixComputeLMedSq computor(true,8);
    FMatrix f = computor.compute(points1, points2);

    vcl_cout << "FMatrixComputeLMedSq with rank truncation:\nF = " << f << vcl_endl;

    double d = 0;
    for(unsigned int i = 0; i < points1.size(); ++i)
      d += f.image1_epipolar_distance_squared(&points1[i], &points2[i]);
    vcl_cout << "Error = " << d/points1.size() << vcl_endl;
  }

  return 0;
}
