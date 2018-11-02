// Module: FMatrixCompute example
// Author: Andrew W. Fitzgibbon, Oxford RRG, July 96
// Adapted to mvl by Peter Vanroose, January 2002.

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_homg_point_2d.h>
#include <mvl/FMatrix.h>
#include <mvl/FMatrixComputeLinear.h>
#include <mvl/FMatrixCompute7Point.h>
#include <mvl/FMatrixComputeMLESAC.h>
#include <mvl/FMatrixComputeRANSAC.h>
#include <mvl/FMatrixComputeLMedSq.h>

int main(int argc, char**argv)
{
  std::vector<vgl_homg_point_2d<double> > points1;
  std::vector<vgl_homg_point_2d<double> > points2;
  bool ishomg = (argc > 1 && argv[1][0] == 'h');

  // Read points
  while (std::cin.good()) {
    double x,y,w=1.0; std::cin >> x >> y; if (ishomg) std::cin >> w; // Read 2 or 3 reals
    points1.emplace_back(x,y,w);
    w=1.0; std::cin >> x >> y; if (ishomg) std::cin >> w; // Read 2 or 3 reals
    points2.emplace_back(x,y,w);
    std::cin >> std::ws; // Eat whitespace
  }

  {
    // Perform the fit using the non-normalized linear computor.
    FMatrixComputeLinear computor(false);
    FMatrix f = computor.compute(points1, points2);
    f.set_rank2_using_svd();

    std::cout << "FMatrixComputeLinear:\nF = " << f << std::endl;

    double d = 0;
    for (unsigned int i = 0; i < points1.size(); ++i)
      d += f.image1_epipolar_distance_squared(points1[i], points2[i]);
    std::cout << "Error = " << d/points1.size() << std::endl;
  }

  {
    // Perform the fit using the non-normalised 7-point estimator.
    FMatrixCompute7Point computor(false);
    FMatrix f;
    std::vector<FMatrix*> l; l.push_back(&f);
    computor.compute(points1, points2, l);
    f.set_rank2_using_svd();

    std::cout << "FMatrixCompute7Point:\nF = " << f << std::endl;

    double d = 0;
    for (unsigned int i = 0; i < points1.size(); ++i)
      d += f.image1_epipolar_distance_squared(points1[i], points2[i]);
    std::cout << "Error = " << d/points1.size() << std::endl;
  }

  {
    // Perform the fit using Phil Torr's Maximum Likelihood Estimation.
    FMatrixComputeMLESAC computor(true,2);
    FMatrix f = computor.compute(points1, points2);
    f.set_rank2_using_svd();

    std::cout << "FMatrixComputeMLESAC with rank truncation:\nF = " << f << std::endl;

    double d = 0;
    for (unsigned int i = 0; i < points1.size(); ++i)
      d += f.image1_epipolar_distance_squared(points1[i], points2[i]);
    std::cout << "Error = " << d/points1.size() << std::endl;
  }

  {
    // Perform the fit using Phil Torr's Robust Sampling Consensus
    FMatrixComputeRANSAC computor(true,2);
    FMatrix f = computor.compute(points1, points2);
    f.set_rank2_using_svd();

    std::cout << "FMatrixComputeRANSAC with rank truncation:\nF = " << f << std::endl;

    double d = 0;
    for (unsigned int i = 0; i < points1.size(); ++i)
      d += f.image1_epipolar_distance_squared(points1[i], points2[i]);
    std::cout << "Error = " << d/points1.size() << std::endl;
  }

  {
    // Perform the fit using the normalized linear computor.
    FMatrixComputeLMedSq computor(true,8);
    FMatrix f = computor.compute(points1, points2);
    f.set_rank2_using_svd();

    std::cout << "FMatrixComputeLMedSq with rank truncation:\nF = " << f << std::endl;

    double d = 0;
    for (unsigned int i = 0; i < points1.size(); ++i)
      d += f.image1_epipolar_distance_squared(points1[i], points2[i]);
    std::cout << "Error = " << d/points1.size() << std::endl;
  }

  return 0;
}
