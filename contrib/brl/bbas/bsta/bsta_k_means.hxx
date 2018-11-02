// This is bsta/bsta_k_means.hxx
#ifndef vgl_distance_hxx_
#define vgl_distance_hxx_

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include "bsta_k_means.h"
//:
//  \file

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
template <class T>
unsigned bsta_k_means(std::vector<vnl_vector<T> > &data, unsigned& k,
                std::vector<vnl_vector<T> >* cluster_centres,
                std::vector<unsigned> * partition //=0
                )
{
  unsigned n_data = data.size();
  if(n_data==0){
    std::cout << "no data to process in bsta_k_means\n";
    return 0;
  }
  std::vector<vnl_vector<T> > & centres = *cluster_centres;

  std::vector<unsigned> * p_partition;

  unsigned didx = 0;

  unsigned  dims = data[didx].size();

  std::vector<vnl_vector<T> > sums(k, vnl_vector<T>(dims, T(0)));
  std::vector<unsigned> nNearest(k, 0);
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
       centres[i] = data[didx];
       sums[i] += data[didx];
       nNearest[i]++;
       didx++;
     }
    }
  else if (initialise_from_clusters)
    {                         // calculate centres fro existing
      do
     {
       sums[(*p_partition)[didx] ] += data[didx];
       nNearest[(*p_partition)[didx] ]++;
     } while (didx++<n_data);
      // Calculate centres
      for (i=0; i<k; ++i)
     centres[i] = sums[i]/static_cast<T>(nNearest[i]);
      didx = 0;
      std::fill(sums.begin(), sums.end(), vnl_vector<T>(dims, T(0)));
      std::fill(nNearest.begin(), nNearest.end(), 0);
    }

  bool changed = true;
  while (changed)
    {
      changed = false;
      do
     {
       unsigned bestCentre = 0;
       T bestDist = vnl_vector_ssd(centres[0], data[didx]);
       for (i=1; i<k; ++i)
         {
           T dist = vnl_vector_ssd(centres[i], data[didx]);
           if (dist < bestDist)
          {
            bestDist = dist;
            bestCentre = i;
          }
         }
       sums[bestCentre] += data[didx];
       nNearest[bestCentre] ++;
       if (bestCentre != (*p_partition)[didx])
         {
           changed = true;
           (*p_partition)[didx] = bestCentre;
         }
     } while (++didx<n_data);


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
     centres[i] = sums[i]/static_cast<T>(nNearest[i]);

      // and repeat

      didx = 0;
      std::fill(sums.begin(), sums.end(), vnl_vector<T>(dims, T(0)));
      std::fill(nNearest.begin(), nNearest.end(), 0);
      iterations ++;
    }

  if (!partition)
    delete p_partition;

  return iterations;
}
template <class T, unsigned int n>
unsigned bsta_k_means(std::vector<vnl_vector_fixed<T, n> > &data, unsigned& k,
                std::vector<vnl_vector_fixed<T, n> >* cluster_centres,
                std::vector<unsigned> * partition){
  unsigned n_data = data.size();
  if(n_data==0){
    std::cout << "no data to process in bsta_k_means\n";
    return 0;
  }
  std::vector<vnl_vector_fixed<T, n> > & centres = *cluster_centres;

  std::vector<unsigned> * p_partition;

  unsigned didx = 0;

  unsigned  dims = data[didx].size();

  std::vector<vnl_vector_fixed<T, n> > sums(k, vnl_vector_fixed<T, n>(T(0)));
  std::vector<unsigned> nNearest(k, 0);
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
       std::fill(p_partition->begin(), p_partition->end(), 0u);
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
       centres[i] = data[didx];
       sums[i] += data[didx];
       nNearest[i]++;
       didx++;
     }
    }
  else if (initialise_from_clusters)
    {                         // calculate centres fro existing
      do
     {
       sums[(*p_partition)[didx] ] += data[didx];
       nNearest[(*p_partition)[didx] ]++;
     } while (didx++<n_data);
      // Calculate centres
      for (i=0; i<k; ++i)
     centres[i] = sums[i]/static_cast<T>(nNearest[i]);
      didx = 0;
      std::fill(sums.begin(), sums.end(), vnl_vector_fixed<T, n>(T(0)));
      std::fill(nNearest.begin(), nNearest.end(), 0);
    }

  bool changed = true;
  while (changed)
    {
      changed = false;
      do
     {
       unsigned bestCentre = 0;
       T bestDist = vnl_vector_ssd(centres[0], data[didx]);
       for (i=1; i<k; ++i)
         {
           T dist = vnl_vector_ssd(centres[i], data[didx]);
           if (dist < bestDist)
          {
            bestDist = dist;
            bestCentre = i;
          }
         }
       sums[bestCentre] += data[didx];
       nNearest[bestCentre] ++;
       if (bestCentre != (*p_partition)[didx])
         {
           changed = true;
           (*p_partition)[didx] = bestCentre;
         }
     } while (++didx<n_data);


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
     centres[i] = sums[i]/static_cast<T>(nNearest[i]);

      // and repeat

      didx = 0;
      std::fill(sums.begin(), sums.end(), vnl_vector<T>(dims, T(0)));
      std::fill(nNearest.begin(), nNearest.end(), 0);
      iterations ++;
    }

  if (!partition)
    delete p_partition;

  return iterations;
}
template <class T>
static inline void incXbyYv(vnl_vector<T> *X, const vnl_vector<T> &Y, T v)
{
  assert(X->size() == Y.size());
  assert(X->size() > 0);
  int i = ((int)X->size()) - 1;
  T * const pX=X->data_block();
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
template <class T>
unsigned bsta_k_means_weighted(std::vector<vnl_vector<T> > &data,
                               unsigned& k,
                               const std::vector<T>& wts,
                               std::vector<vnl_vector<T> >* cluster_centres,
                               std::vector<unsigned> * partition //=0
                               )
{
  unsigned n_data = data.size();
  if(n_data==0){
    std::cout << "no data to process in bsta_k_means\n";
    return 0;
  }
  std::vector<vnl_vector<T> > & centres = *cluster_centres;

  std::vector<unsigned> * p_partition;
  unsigned didx = 0;
  unsigned  dims = data[didx].size();
  std::vector<vnl_vector<T> > sums(k, vnl_vector<T>(dims, T(0)));
  std::vector<T> nNearest(k, T(0));
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

  const vnl_vector<T>  vector_T_dims_0(dims, T(0));


  // Calculate initial centres

  if (centres.size() != k) // use first k non-zero weighted data items as centres
    {
      centres.resize(k);
      for (i=0; i<k; ++i)
     {
       while (wts[didx] == T(0)) // skip zero weighted data
         {
#ifdef NDEBUG
           didx++;
#else
           if (++didx<n_data)
          {
            std::cerr << "ERROR: bsta_k_means_weighted, while initialising centres from data\n"
                  << "Not enough non-zero-weighted data\n";
            std::abort();
          }
#endif //NDEBUG
         }
       centres[i] = data[didx];
       incXbyYv(&sums[i], data[didx], wts[didx]);

       nNearest[i]+= wts[didx];
       didx++;
     }
    }
  else if (initialise_from_clusters)
    {                         // calculate centres fro existing

      do
     {
       incXbyYv(&sums[(*p_partition)[didx] ], data[didx], wts[didx]);
       nNearest[(*p_partition)[didx] ]+=wts[didx];
     } while (++didx<n_data);
      // Calculate centres
      for (i=0; i<k; ++i)
     centres[i] = sums[i]/nNearest[i];
      didx = 0;
      std::fill(sums.begin(), sums.end(), vector_T_dims_0);
      std::fill(nNearest.begin(), nNearest.end(), T(0));
    }

  bool changed = true;
  while (changed)
    {
      changed = false;
      do
     {
       const T w = wts[didx];
       if (w != T(0))
         {
           unsigned bestCentre = 0;
           T bestDist = vnl_vector_ssd(centres[0], data[didx]);
           for (i=1; i<k; ++i)
          {
            T dist = vnl_vector_ssd(centres[i], data[didx]);
            if (dist < bestDist)
              {
                bestDist = dist;
                bestCentre = i;
              }
          }
           incXbyYv(&sums[bestCentre], data[didx], w);
           nNearest[bestCentre] += w;
           if (bestCentre != (*p_partition)[didx])
          {
            changed = true;
            (*p_partition)[didx] = bestCentre;
          }
         }
     } while (++didx<n_data);


      // reduce k if any centres have no data items assigned to its cluster.
      for (i=0; i<k; ++i)
     {
       if ( nNearest[i] == T(0))
         {
           k--;
           centres.erase(centres.begin()+i);
           sums.erase(sums.begin()+i);
           nNearest.erase(nNearest.begin()+i);
           for (unsigned j=0; j<p_partition->size(); ++j)
          {
            if (wts[j] != T(0))
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
      didx = 0;
      std::fill(sums.begin(), sums.end(), vector_T_dims_0);
      std::fill(nNearest.begin(), nNearest.end(), T(0));
      iterations ++;
    }

  if (!partition)
    delete p_partition;
  else // assign all the zero weighted samples to their nearest centres.
    {
      didx = 0;
      do
     {
       if (wts[didx] == T(0))
         {
           unsigned bestCentre = 0;
           T bestDist = vnl_vector_ssd(centres[0], data[didx]);
           for (i=1; i<k; ++i)
          {
            T dist = vnl_vector_ssd(centres[i], data[didx]);
            if (dist < bestDist)
              {
                bestDist = dist;
                bestCentre = i;
              }
          }
           (*p_partition)[didx] = bestCentre;
         }
     } while (++didx<n_data);
    }

  return iterations;
}

template <class T, unsigned int n>
unsigned bsta_k_means_weighted(std::vector<vnl_vector_fixed<T,n> > &data,
                      unsigned& k,
                      const std::vector<T>& wts,
                      std::vector<vnl_vector_fixed<T,n> >* cluster_centres,
                      std::vector<unsigned> * partition){
  unsigned n_data = data.size();
  if(n_data==0){
    std::cout << "no data to process in bsta_k_means\n";
    return 0;
  }
  std::vector<vnl_vector_fixed<T,n> > & centres = *cluster_centres;

  std::vector<unsigned> * p_partition;
  unsigned didx = 0;
  std::vector<vnl_vector_fixed<T,n> > sums(k, vnl_vector_fixed<T,n>(T(0)));
  std::vector<T> nNearest(k, T(0));
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

  const vnl_vector_fixed<T,n>  vector_T_dims_0(T(0));


  // Calculate initial centres

  if (centres.size() != k) // use first k non-zero weighted data items as centres
    {
      centres.resize(k);
      for (i=0; i<k; ++i)
     {
       while (wts[didx] == T(0)) // skip zero weighted data
         {
#ifdef NDEBUG
           didx++;
#else
           if (++didx<n_data)
          {
            std::cerr << "ERROR: bsta_k_means_weighted, while initialising centres from data\n"
                  << "Not enough non-zero-weighted data\n";
            std::abort();
          }
#endif //NDEBUG
         }
       centres[i] = data[didx];
       //incXbyYv(&sums[i], data[didx], wts[didx]n);
       sums[i] += (wts[didx]*data[didx]);
       nNearest[i]+= wts[didx];
       didx++;
     }
    }
  else if (initialise_from_clusters)
    {                         // calculate centres fro existing

      do
     {
       //incXbyYv(&sums[(*p_partition)[didx] ], data[didx], wts[didx]);
       sums[(*p_partition)[didx] ] += (wts[didx]*data[didx]);
       nNearest[(*p_partition)[didx] ]+=wts[didx];
     } while (++didx<n_data);
      // Calculate centres
      for (i=0; i<k; ++i)
     centres[i] = sums[i]/nNearest[i];
      didx = 0;
      std::fill(sums.begin(), sums.end(), vector_T_dims_0);
      std::fill(nNearest.begin(), nNearest.end(), T(0));
    }

  bool changed = true;
  while (changed)
    {
      changed = false;
      do
     {
       const T w = wts[didx];
       if (w != T(0))
         {
           unsigned bestCentre = 0;
           T bestDist = vnl_vector_ssd(centres[0], data[didx]);
           for (i=1; i<k; ++i)
          {
            T dist = vnl_vector_ssd(centres[i], data[didx]);
            if (dist < bestDist)
              {
                bestDist = dist;
                bestCentre = i;
              }
          }
           //incXbyYv(&sums[bestCentre], data[didx], w);
           sums[bestCentre]+= (w*data[didx]);
           nNearest[bestCentre] += w;
           if (bestCentre != (*p_partition)[didx])
          {
            changed = true;
            (*p_partition)[didx] = bestCentre;
          }
         }
     } while (++didx<n_data);


      // reduce k if any centres have no data items assigned to its cluster.
      for (i=0; i<k; ++i)
     {
       if ( nNearest[i] == T(0))
         {
           k--;
           centres.erase(centres.begin()+i);
           sums.erase(sums.begin()+i);
           nNearest.erase(nNearest.begin()+i);
           for (unsigned j=0; j<p_partition->size(); ++j)
          {
            if (wts[j] != T(0))
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
      didx = 0;
      std::fill(sums.begin(), sums.end(), vector_T_dims_0);
      std::fill(nNearest.begin(), nNearest.end(), T(0));
      iterations ++;
    }

  if (!partition)
    delete p_partition;
  else // assign all the zero weighted samples to their nearest centres.
    {
      didx = 0;
      do
     {
       if (wts[didx] == T(0))
         {
           unsigned bestCentre = 0;
           T bestDist = vnl_vector_ssd(centres[0], data[didx]);
           for (i=1; i<k; ++i)
          {
            T dist = vnl_vector_ssd(centres[i], data[didx]);
            if (dist < bestDist)
              {
                bestDist = dist;
                bestCentre = i;
              }
          }
           (*p_partition)[didx] = bestCentre;
         }
     } while (++didx<n_data);
    }
  return iterations;
}
#undef BSTA_K_MEANS_INSTANTIATE
#define BSTA_K_MEANS_INSTANTIATE(T, n)      \
 template void incXbyYv(vnl_vector<T> *, const vnl_vector<T> &, T); \
 template unsigned bsta_k_means_weighted(std::vector<vnl_vector<T> > &, unsigned&, const std::vector<T>&, std::vector<vnl_vector<T> >*, std::vector<unsigned> *); \
 template unsigned bsta_k_means_weighted(std::vector<vnl_vector_fixed<T,n> > &, \
   unsigned&, const std::vector<T>&, std::vector<vnl_vector_fixed<T,n> >*, \
   std::vector<unsigned> * partition); \
 template unsigned bsta_k_means(std::vector<vnl_vector<T> > &, unsigned&, std::vector<vnl_vector<T> >*, std::vector<unsigned> *); \
 template unsigned bsta_k_means(std::vector<vnl_vector_fixed<T, n> > &, unsigned&, std::vector<vnl_vector_fixed<T, n> >*, std::vector<unsigned> *)

#endif // bsta_k_means_hxx_
