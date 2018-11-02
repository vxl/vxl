// This is mul/mbl/mbl_k_means.cxx
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include "mbl_k_means.h"
//:
//  \file

#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: Find k cluster centres
// Uses batch k-means clustering.
// If you provide parameter partition, it will return the
// cluster index for each data sample. The number of iterations
// performed is returned.
//
// \par Initial Cluster Centres
// If centres contain the correct number of centres, they will
// be used as the initial centres, If not, and if partition is
// given, and it is the correct size, then this will be used
// to find the initial centres.
//
// \par Degenerate Cases
// If at any point the one of the centres has no data points allocated to it
// the number of centres will be reduced below k. This is most likely to
// happen if you start the function with one or more centre identical, or
// if some of the centres start off outside the convex hull of the data set.
// In particular if you let the function initialise the centres, it will
// occur if any of the first k data samples are identical.
unsigned mbl_k_means(mbl_data_wrapper<vnl_vector<double> > &data, unsigned k,
                     std::vector<vnl_vector<double> >* cluster_centres,
                     std::vector<unsigned> * partition //=0
                    )
{
  std::vector<vnl_vector<double> > & centres = *cluster_centres;

  std::vector<unsigned> * p_partition;
  data.reset();
  unsigned  dims = data.current().size();
  std::vector<vnl_vector<double> > sums(k, vnl_vector<double>(dims, 0.0));
  std::vector<unsigned> nNearest(k,0);
  unsigned i;
  unsigned iterations =0;

  assert(data.size() >= k);

  bool initialise_from_clusters = false;

  // set up p_partition to point to something sensible
  if (partition)
  {
    p_partition = partition;
    if (p_partition->size() != data.size())
    {
      p_partition->resize(data.size());
      std::fill(p_partition->begin(), p_partition->end(), 0);
    }
    else initialise_from_clusters = true;
  }
  else
    p_partition = new std::vector<unsigned>(int(data.size()), 0u);


// Calculate initial centres

  if (centres.size() != k) // use first k data items as centres
  {
    centres.resize(k);
    for (i=0; i<k; ++i)
    {
      centres[i] = data.current();
      sums[i] += data.current();
      nNearest[i]++;
      data.next();
    }
  }
  else if (initialise_from_clusters)
  {                         // calculate centres fro existing
    do
    {
      sums[(*p_partition)[data.index()] ] += data.current();
      nNearest[(*p_partition)[data.index()] ]++;
    } while (data.next());
    // Calculate centres
    for (i=0; i<k; ++i)
      centres[i] = sums[i]/nNearest[i];
    data.reset();
    std::fill(sums.begin(), sums.end(), vnl_vector<double>(dims, 0.0));
    std::fill(nNearest.begin(), nNearest.end(), 0);
  }

  bool changed = true;
  while (changed)
  {
    changed = false;
    do
    {
      unsigned bestCentre = 0;
      double bestDist = vnl_vector_ssd(centres[0], data.current());
      for (i=1; i<k; ++i)
      {
        double dist = vnl_vector_ssd(centres[i], data.current());
        if (dist < bestDist)
        {
            bestDist = dist;
            bestCentre = i;
        }
      }
      sums[bestCentre] += data.current();
      nNearest[bestCentre] ++;
      if (bestCentre != (*p_partition)[data.index()])
      {
        changed = true;
        (*p_partition)[data.index()] = bestCentre;
      }
    } while (data.next());


    // reduce k if any centres have no data items assigned to its cluster.
    for (i=0; i<k; ++i)
    {
      if ( nNearest[i] == 0)
      {
        k--;
        centres.erase(centres.begin()+i);
        sums.erase(sums.begin()+i);
        nNearest.erase(nNearest.begin()+i);

        for (unsigned int & j : *p_partition)
        {
          assert (j = i);
          if (j > i) j--;
        }

        changed= true;
      }
    }

    // Calculate new centres
    for (i=0; i<k; ++i)
      centres[i] = sums[i]/nNearest[i];

    // and repeat
    data.reset();
    std::fill(sums.begin(), sums.end(), vnl_vector<double>(dims, 0.0));
    std::fill(nNearest.begin(), nNearest.end(), 0);
    iterations ++;
  }

  if (!partition)
    delete p_partition;

  return iterations;
}

static inline void incXbyYv(vnl_vector<double> *X, const vnl_vector<double> &Y, double v)
{
  assert(X->size() == Y.size());
  assert(X->size() > 0);
  int i = ((int)X->size()) - 1;
  double * const pX=X->data_block();
  while (i >= 0)
  {
    pX[i] += Y[i] * v;
    i--;
  }
}

//: Find k cluster centres with weighted data
// Uses batch k-means clustering.
// If you provide parameter partition, it will return the
// cluster index for each data sample. The number of iterations
// performed is returned.
//
// \par Initial Cluster Centres
// If centres contain the correct number of centres, they will
// be used as the initial centres, If not, and if partition is
// given, and it is the correct size, then this will be used
// to find the initial centres.
//
// \par Degenerate Cases
// If at any point the one of the centres has no data points allocated to it
// the number of centres will be reduced below k. This is most likely to
// happen if you start the function with one or more centre identical, or
// if some of the centres start off outside the convex hull of the data set.
// In particular if you let the function initialise the centres, it will
// occur if any of the first k data samples are identical.
//
// \par
// The algorithm has been optimised
unsigned mbl_k_means_weighted(mbl_data_wrapper<vnl_vector<double> > &data, unsigned k,
                              const std::vector<double>& wts,
                              std::vector<vnl_vector<double> >* cluster_centres,
                              std::vector<unsigned> * partition //=0
                             )
{
  std::vector<vnl_vector<double> > & centres = *cluster_centres;

  std::vector<unsigned> * p_partition;
  data.reset();
  unsigned  dims = data.current().size();
  std::vector<vnl_vector<double> > sums(k, vnl_vector<double>(dims, 0.0));
  std::vector<double> nNearest(k,0.0);
  unsigned i;
  unsigned iterations =0;

  bool initialise_from_clusters = false;

  assert(data.size() >= k);
  assert(data.size() == wts.size());

  // set up p_partition to point to something sensible
  if (partition)
  {
    p_partition = partition;
    if (p_partition->size() != data.size())
    {
      p_partition->resize(data.size());
      std::fill(p_partition->begin(), p_partition->end(), 0);
    }
    else initialise_from_clusters = true;
  }
  else
    p_partition = new std::vector<unsigned>(int(data.size()), 0u);

  const vnl_vector<double>  vector_double_dims_0(dims, 0.0);


// Calculate initial centres

  if (centres.size() != k) // use first k non-zero weighted data items as centres
  {
    centres.resize(k);
    for (i=0; i<k; ++i)
    {
      while (wts[data.index()] == 0.0) // skip zero weighted data
      {
#ifdef NDEBUG
        data.next();
#else
        if (!data.next())
        {
          std::cerr << "ERROR: mbl_k_means_weighted, while initialising centres from data\n"
                   << "Not enough non-zero-weighted data\n";
          std::abort();
        }
#endif //NDEBUG
      }
      centres[i] = data.current();
      incXbyYv(&sums[i], data.current(), wts[data.index()]);

      nNearest[i]+= wts[data.index()];
      data.next();
    }
  }
  else if (initialise_from_clusters)
  {                         // calculate centres fro existing

    do
    {
      incXbyYv(&sums[(*p_partition)[data.index()] ], data.current(), wts[data.index()]);
      nNearest[(*p_partition)[data.index()] ]+=wts[data.index()];
    } while (data.next());
    // Calculate centres
    for (i=0; i<k; ++i)
      centres[i] = sums[i]/nNearest[i];
    data.reset();
    std::fill(sums.begin(), sums.end(), vector_double_dims_0);
    std::fill(nNearest.begin(), nNearest.end(), 0.0);
  }

  bool changed = true;
  while (changed)
  {
    changed = false;
    do
    {
      const double w = wts[data.index()];
      if (w != 0.0)
      {
        unsigned bestCentre = 0;
        double bestDist = vnl_vector_ssd(centres[0], data.current());
        for (i=1; i<k; ++i)
        {
          double dist = vnl_vector_ssd(centres[i], data.current());
          if (dist < bestDist)
          {
              bestDist = dist;
              bestCentre = i;
          }
        }
        incXbyYv(&sums[bestCentre], data.current(), w);
        nNearest[bestCentre] += w;
        if (bestCentre != (*p_partition)[data.index()])
        {
          changed = true;
          (*p_partition)[data.index()] = bestCentre;
        }
      }
    } while (data.next());


    // reduce k if any centres have no data items assigned to its cluster.
    for (i=0; i<k; ++i)
    {
      if ( nNearest[i] == 0.0)
      {
        k--;
        centres.erase(centres.begin()+i);
        sums.erase(sums.begin()+i);
        nNearest.erase(nNearest.begin()+i);
        for (unsigned j=0; j<p_partition->size(); ++j)
        {
          if (wts[j] != 0.0)
          {
            assert ((*p_partition)[j]  != i);
            if ((*p_partition)[j] > i) (*p_partition)[j]--;
          }
        }
      }
    }

    // Calculate new centres
    for (i=0; i<k; ++i)
      centres[i] = sums[i]/nNearest[i];

    // and repeat
    data.reset();
    std::fill(sums.begin(), sums.end(), vector_double_dims_0);
    std::fill(nNearest.begin(), nNearest.end(), 0.0);
    iterations ++;
  }

  if (!partition)
    delete p_partition;
  else // assign all the zero weighted samples to their nearest centres.
  {
    data.reset();
    do
    {
      if (wts[data.index()] == 0.0)
      {
        unsigned bestCentre = 0;
        double bestDist = vnl_vector_ssd(centres[0], data.current());
        for (i=1; i<k; ++i)
        {
          double dist = vnl_vector_ssd(centres[i], data.current());
          if (dist < bestDist)
          {
            bestDist = dist;
            bestCentre = i;
          }
        }
        (*p_partition)[data.index()] = bestCentre;
      }
    } while (data.next());
  }

  return iterations;
}
