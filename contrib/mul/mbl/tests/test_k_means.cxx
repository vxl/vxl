#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>
#include <vcl_cmath.h> // for vcl_sqrt()

#include <vnl/vnl_test.h>
#include <mbl/mbl_mz_random.h>
#include <mbl/mbl_k_means.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <vbl/vbl_array_3d.h>
#include <vbl/vbl_array_2d.h>
#include <vnl/vnl_math.h>
#include <vbl/vbl_bounding_box.h>

void test_k_means()
{
  vcl_cout << "\n*********************" << vcl_endl;
  vcl_cout << " Testing mbl_k_means " << vcl_endl;
  vcl_cout << "*********************" << vcl_endl;

  mbl_mz_random rng;
  rng.reseed(123456);
  
  const unsigned nCentres = 64;
  const unsigned nSamples = 1000;
  const unsigned nDims = 3;

  vcl_cout << "Find " << nCentres << " clusters from " << nSamples
    << " " << nDims << "-D points." << vcl_endl;

  vcl_vector<vnl_vector<double> >
    data(nSamples, vnl_vector<double>(nDims, 0.0));

  // sample data uniformly from unit cube
  unsigned i, j;
  for (i=0; i<nSamples; ++i)
    for (j=0; j<nDims; ++j)
      data[i](j) = rng.drand32(0.0, 1.0);

  vcl_vector<vnl_vector<double> > centres;
  vcl_vector<unsigned> clusters;
  mbl_data_array_wrapper<vnl_vector<double> > data_array1(data);

  unsigned nIts =
    mbl_k_means(data_array1, nCentres, &centres, &clusters);
  vcl_cout << "Took " << nIts << " iterations." << vcl_endl;

  vbl_bounding_box<double, nDims> bbox;
  bbox.update(&((vnl_vector<double>(nDims, 0.0))[0]));
  bbox.update(&((vnl_vector<double>(nDims, 1.0))[0]));

  i=0;
  while ( i<nCentres && bbox.inside(centres[i].data_block()) ) i++;
  
  TEST ("All cluster centres are inside bounding box", i, nCentres);

  vcl_cout << "\n\n======Test spread of centres" << vcl_endl;

  if(nDims == 2)
  {
    vbl_array_2d<unsigned> m(10, 10, 0u);
    for (i=0; i<nCentres; ++i)
    {
      m((unsigned)(centres[i](0) * 10.0),
        (unsigned)(centres[i](1) * 10.0)) ++;
    }
    vcl_cout << "Distribution of centres in a 10x10 histogram:\n"
             << m << vcl_endl;
  }
  else if (nDims == 3)
  {
    vbl_array_3d<unsigned> m(4, 4, 4, 0u);
    for (i=0; i<nCentres; ++i)
    {
      m((unsigned)(centres[i](0) * 4.0),
        (unsigned)(centres[i](1) * 4.0),
        (unsigned)(centres[i](2) * 4.0)) ++;
    }
    vcl_cout << "Distribution of centres in a 4x4x4 histogram:\n"
             << m << vcl_endl;
  }

  
  double bestDistSum=0.0, bestDistSumSq=0.0;
  for (i=0; i<nCentres; ++i)
  {
    double bestDist = nDims ;
    for (j=0; j<i; ++j)
      bestDist = vnl_math_min(bestDist,
                              vnl_vector_ssd(centres[i], centres[j]));
    for (j=i+1; j<nCentres; ++j)
      bestDist = vnl_math_min(bestDist,
                              vnl_vector_ssd(centres[i], centres[j]));
    bestDistSum += sqrt(bestDist);
    bestDistSumSq += bestDist;
  }

  double bestDistMean = bestDistSum/nCentres;
  double bestDistSD = (bestDistSumSq - vnl_math_sqr(bestDistSum)/nCentres)/(nCentres-1);
  vcl_cout << "\nClosest cluster centres are on average " << bestDistMean << "  apart" << vcl_endl;
  vcl_cout << "\twith a s.d. of " << bestDistSD << vcl_endl;

  TEST("Cluster centres are further than 0.2 apart",
    bestDistMean - bestDistSD > 0.2, true);

  vcl_vector<unsigned> clusters2;    
 
  mbl_data_array_wrapper<vnl_vector<double> > data_array2(data);
  mbl_k_means(data_array2, nCentres, &centres, &clusters2);


  vcl_cout << "\n\n======Test Stability of solution" << vcl_endl;
  TEST("Cluster partitions do not change when restarting with the found centres", clusters2, clusters);

  centres.resize(0);
  mbl_k_means(data_array2, nCentres, &centres, &clusters2);
  TEST("Cluster partitions do not change when restarting with the found partition", clusters2, clusters);
  vcl_cout << vcl_endl << vcl_endl;

}

TESTMAIN(test_mz_random);
