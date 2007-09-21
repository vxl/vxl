// This is mul/mbl/mbl_clusters.txx
#ifndef mbl_clusters_txx_
#define mbl_clusters_txx_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  Class to record clusters of data, for faster neighbour finding
// \author Tim Cootes

#include "mbl_clusters.h"
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>
#include <vcl_cassert.h>

//: Default constructor
template<class T, class D>
mbl_clusters<T,D>::mbl_clusters()
  : data_(0)
{
}

//: Define maximum radius for each cluster
template<class T, class D>
void mbl_clusters<T,D>::set_max_r(double r)
{
  max_r_ = r;
}

//: Empty clusters
template<class T, class D>
void mbl_clusters<T,D>::empty()
{
  p_.resize(0);
  index_.resize(0);
  r_.resize(0);
}

//: Define external data array (pointer retained)
//  Empty existing clusters, then process every element of data
//  to create clusters, by calling add_object() 
template<class T, class D>
void mbl_clusters<T,D>::set_data(const vcl_vector<T>& data)
{
  empty();
  data_ = &data;
  unsigned n = data.size();
  for (unsigned i=0;i<n;++i)  add_object(i);
}


//: Return index of nearest object in data() to t
//  Nearest object in data() to t is given by data()[nearest(t,d)];
//  The distance to the point is d
template<class T, class D>
unsigned mbl_clusters<T,D>::nearest(const T& t, double& d) const
{
  assert(data_!=0);
  
  // Initialise with first in data
  unsigned best_i = 0;
  d = D::d(data()[0],t);

  const T* data_ptr = &data()[0];

  // Try each cluster in turn
  for (unsigned j=0;j<p_.size();++j)
  {
    double dj = D::d(t,p_[j]);
    if (dj-r_[j]<d)
    {
      // There may be a point in the cluster closer than the current best
      const vcl_vector<unsigned>& ind = index_[j];
      for (unsigned i=0;i<ind.size();++i)
      {
        double di=D::d(data_ptr[ind[i]],t);
        if (di<d) { d=di; best_i=ind[i]; }
      }
    }
  }
  return best_i;
}

//: Return index of nearest object in data() to t
  //  Consider only objects in clusters given in c_list
//  Nearest object in data() to t is given by data()[nearest(t,d)];
//  The distance to the point is d
template<class T, class D>
unsigned mbl_clusters<T,D>::nearest(const T& t, double& d,
                   const vcl_vector<unsigned>& c_list) const
{
  assert(data_!=0);

  // Initialise with first in set for c_list[0]
  unsigned best_i = 0;
  d = D::d(data()[index_[c_list[0]][0]],t);

  const T* data_ptr = &data()[0];

  // Try each cluster in turn
  for (unsigned k=0;k<c_list.size();++k)
  {
    unsigned j=c_list[k];
    double dj = D::d(t,p_[j]);
    if (dj-r_[j]<d)
    {
      // There may be a point in the cluster closer than the current best
      const vcl_vector<unsigned>& ind = index_[j];
      for (unsigned i=0;i<ind.size();++i)
      {
        double di=D::d(data_ptr[ind[i]],t);
        if (di<d) { d=di; best_i=ind[i]; }
      }
    }
  }
  return best_i;
}


//: Return index of nearest cluster in data() to t
//  Finds nearest cluster key point to t 
//  The distance to the point is d
template<class T, class D>
unsigned mbl_clusters<T,D>::nearest_cluster(const T& t, double& d) const
{
  assert(p_.size()>0);

  d = D::d(p_[0],t);
  unsigned best_j = 0;

  // Try each cluster in turn
  for (unsigned j=1;j<p_.size();++j)
  {
    double dj = D::d(t,p_[j]);
    if (dj<d) { d=dj; best_j=j; }
  }

  return best_j;
}

//: Create a new cluster around point index i
// Return index of cluster
template<class T, class D>
unsigned mbl_clusters<T,D>::create_cluster(unsigned new_i)
{
  // Create a new cluster using this as a key point
  p_.push_back(data()[new_i]);
  r_.push_back(0);
  vcl_vector<unsigned> ind(1);
  ind[0]=new_i;
  index_.push_back(ind);
  return p_.size()-1;
}

//: Append new object with index i and assign to a cluster
//  Assumes that new object data()[i] is available. 
//  Deduce which cluster it belongs to and add it.
//  Create new cluster if further than max_r() from any.
//  Return index of cluster it is assigned to
template<class T, class D>
unsigned mbl_clusters<T,D>::add_object(unsigned new_i)
{
  assert(new_i<data_->size());

  // If initially empty, create one cluster
  if (p_.size()==0) return create_cluster(new_i);

  double d;
  unsigned j=nearest_cluster(data()[new_i],d);
  if (d<max_r_)
  {
    // Add it to cluster j
    index_[j].push_back(new_i);
    if (d>r_[j]) r_[j]=d;  // Update max radius of cluster
  }
  else
    j=create_cluster(new_i);

  return j;
}


#define MBL_CLUSTERS_INSTANTIATE(T,D) \
template class mbl_clusters< T , D >

#endif // mbl_clusters_txx_
