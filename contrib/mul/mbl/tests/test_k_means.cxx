// This is mul/mbl/tests/test_k_means.cxx
#include <iostream>
#include <cmath>
#include <vector>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vnl/vnl_random.h>
#include <mbl/mbl_k_means.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <vbl/vbl_array_3d.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_bounding_box.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <testlib/testlib_test.h>

void test_k_means()
{
  std::cout << "\n*********************\n"
           <<   " Testing mbl_k_means\n"
           <<   "*********************\n";

  vnl_random rng;
  rng.reseed(123456);

  constexpr unsigned nCentres = 64;
  constexpr unsigned nSamples = 1000;
  constexpr unsigned nDims = 3;

  std::cout << "Find " << nCentres << " clusters from " << nSamples
           << ' ' << nDims << "-D points.\n";

  std::vector<vnl_vector<double> >
    data(nSamples, vnl_vector<double>(nDims, 0.0));

  // sample data uniformly from unit cube
  unsigned i, j;
  for (i=0; i<nSamples; ++i)
    for (j=0; j<nDims; ++j) {
      data[i](j) = rng.drand32(0.0, 1.0);
      assert ( data[i](j) >= 0 && data[i](j) < 1);
    }

  std::vector<vnl_vector<double> > centres;
  std::vector<unsigned> clusters;
  mbl_data_array_wrapper<vnl_vector<double> > data_array(data);

  unsigned nIts =
    mbl_k_means(data_array, nCentres, &centres, &clusters);
  std::cout << "Took " << nIts << " iterations.\n"
              "Found " << centres.size() << " clusters.\n";
  TEST ("Found as many clusters as asked for", centres.size(), nCentres);

  vbl_bounding_box<double, nDims> bbox;
  bbox.update(&((vnl_vector<double>(nDims, 0.0))[0]));
  bbox.update(&((vnl_vector<double>(nDims, 1.0))[0]));

  i=0;
  while ( i<nCentres && bbox.inside(centres[i].data_block()) ) i++;

  TEST ("All cluster centres are inside bounding box", i, nCentres);

  std::cout << "\n\n======Test spread of centres\n";

  if (nDims == 2)
  {
    vbl_array_2d<unsigned> m(10, 10, 0u);
    for (i=0; i<nCentres; ++i)
    {
      m((unsigned)(centres[i](0) * 10.0),
        (unsigned)(centres[i](1) * 10.0)) ++;
    }
    std::cout << "Distribution of centres in a 10x10 histogram:\n"
             << m << std::endl;
  }
  else if (nDims == 3)
  {
    vbl_array_3d<unsigned> m(4, 4, 4, 0u);
    for (i=0; i<nCentres; ++i)
    {
      // Note that this will fail if centres[i](j) equals 1 ...
      m((unsigned)(centres[i](0) * 4.0),
        (unsigned)(centres[i](1) * 4.0),
        (unsigned)(centres[i](2) * 4.0)) ++;
    }
    std::cout << "Distribution of centres in a 4x4x4 histogram:\n"
             << m << std::endl;
  }

  double bestDistSum=0.0, bestDistSumSq=0.0;
  for (i=0; i<nCentres; ++i)
  {
    double bestDist = nDims ;
    for (j=0; j<i; ++j)
      bestDist = std::min(bestDist,
                              vnl_vector_ssd(centres[i], centres[j]));
    for (j=i+1; j<nCentres; ++j)
      bestDist = std::min(bestDist,
                              vnl_vector_ssd(centres[i], centres[j]));
    bestDistSum += std::sqrt(bestDist);
    bestDistSumSq += bestDist;
  }

  double bestDistMean = bestDistSum/nCentres;
  double bestDistSD = (bestDistSumSq - vnl_math::sqr(bestDistSum)/nCentres)/(nCentres-1);
  std::cout << "\nClosest cluster centres are on average " << bestDistMean
           << " apart\n\twith a s.d. of " << bestDistSD << std::endl;

  TEST_FAR("Cluster centres are further than 0.2 apart", bestDistMean, bestDistSD, 0.2);

  std::vector<unsigned> clusters2;

  mbl_k_means(data_array, nCentres, &centres, &clusters2);


  std::cout << "\n\n======Test Stability of solution\n";
  TEST("Cluster partitions do not change when restarting with the found centres",
       clusters2, clusters);

  centres.resize(0);
  mbl_k_means(data_array, nCentres, &centres, &clusters2);
  TEST("Cluster partitions do not change when restarting with the found partition",
       clusters2, clusters);

  std::cout << "\n\n======Test mbl_k_means_weighted\n";
  std::vector<double> weights(nSamples, 1.0);
  clusters2.resize(0);
  centres.resize(0);
  mbl_k_means_weighted(data_array, nCentres, weights, &centres, &clusters2);
  TEST("Cluster partitions do not change when rerunning with equal weights",
       clusters2 == clusters && centres.size() ==nCentres, true);

  std::cout << "\n\n=============With biased weighting:\n";

  //  Set the weightings so that we expect the centres at x_0 > 0.5
  for (i=0; i<nSamples; ++i)
  {
    if (data[i](0) < 0.5) weights[i] = 0.0;
    else if (data[i](0) < 0.75) weights[i] = 0.3;
    else weights[i] = 1.0;
  }

  clusters2.resize(0);
  centres.resize(0);
  mbl_k_means_weighted(data_array, nCentres, weights, &centres, &clusters2);
  std::cout << "Found " << centres.size() << " clusters.\n";

  i=0;
  while ( i<centres.size() && bbox.inside(centres[i].data_block()) ) i++;

  TEST ("All cluster centres are inside bounding box", i, centres.size());

  std::cout << "\n\n======Test spread of centres\n";

  if (nDims == 2)
  {
    vbl_array_2d<unsigned> m(10, 10, 0u);
    for (i=0; i<centres.size(); ++i)
    {
      m((unsigned)(centres[i](0) * 10.0),
        (unsigned)(centres[i](1) * 10.0)) ++;
    }
    std::cout << "Distribution of centres in a 10x10 histogram:\n"
             << m << std::endl;
  }
  else if (nDims == 3)
  {
    vbl_array_3d<unsigned> m(4, 4, 4, 0u);
    for (i=0; i<centres.size(); ++i)
    {
      m((unsigned)(centres[i](0) * 4.0),
        (unsigned)(centres[i](1) * 4.0),
        (unsigned)(centres[i](2) * 4.0)) ++;
    }
    std::cout << "Distribution of centres in a 4x4x4 histogram:\n"
             << m << std::endl;
  }

  i=0;
  while ( i<centres.size() && centres[i](0) > 0.5 ) i++;
  TEST("All cluster centres are on correct side of bias decision line",
       i, centres.size());

  std::cout << "\n\n";
}

TESTMAIN(test_k_means);
