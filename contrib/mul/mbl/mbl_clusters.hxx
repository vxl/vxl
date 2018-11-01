// This is mul/mbl/mbl_clusters.hxx
#ifndef mbl_clusters_hxx_
#define mbl_clusters_hxx_
//:
// \file
// \brief  Class to record clusters of data, for faster neighbour finding
// \author Tim Cootes

#include <iostream>
#include <algorithm>
#include "mbl_clusters.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>

//: Default constructor
template<class T, class D>
mbl_clusters<T,D>::mbl_clusters()
  : data_(nullptr)
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
void mbl_clusters<T,D>::set_data(const std::vector<T>& data)
{
  empty();
  data_ = &data;
  unsigned n = data.size();
  for (unsigned i=0;i<n;++i)  add_object(i);
}

//: Define external data array (pointer retained)
//  Use carefully! This sets the internal pointer to
//  point to data.  Really only to be used after loading
//  internals using b_read(bfs).
template<class T, class D>
void mbl_clusters<T,D>::set_data_ptr(const std::vector<T>& data)
{
  data_=&data;
}

//: Return index of nearest object in data() to t
//  Nearest object in data() to t is given by data()[nearest(t,d)];
//  The distance to the point is d
template<class T, class D>
unsigned mbl_clusters<T,D>::nearest(const T& t, double& d) const
{
  assert(data_!=nullptr);

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
      const std::vector<unsigned>& ind = index_[j];
      for (unsigned int i : ind)
      {
        double di=D::d(data_ptr[i],t);
        if (di<d) { d=di; best_i=i; }
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
                   const std::vector<unsigned>& c_list) const
{
  assert(data_!=nullptr);

  // Initialise with first in set for c_list[0]
  unsigned best_i = 0;
  d = D::d(data()[index_[c_list[0]][0]],t);

  const T* data_ptr = &data()[0];

  // Try each cluster in turn
  for (unsigned int j : c_list)
  {
    double dj = D::d(t,p_[j]);
    if (dj-r_[j]<d)
    {
      // There may be a point in the cluster closer than the current best
      const std::vector<unsigned>& ind = index_[j];
      for (unsigned int i : ind)
      {
        double di=D::d(data_ptr[i],t);
        if (di<d) { d=di; best_i=i; }
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

//: Return indices of clusters which may contain nearest point to t
//  Searches through all the clusters
template<class T, class D>
void mbl_clusters<T,D>::nearest_clusters(const T& t, double& max_d,
                           std::vector<unsigned>& near_c) const
{
  assert(p_.size()>0);

  std::vector<unsigned> c1;
  std::vector<double> d1;

  double d = D::d(p_[0],t);
  c1.push_back(0);
  d1.push_back(d);
  max_d = d+r_[0];

  // Try each cluster in turn, recording any that might include closest
  for (unsigned j=1;j<p_.size();++j)
  {
    double dj = D::d(t,p_[j]);
    if (dj-r_[j]<=max_d)
    {
      c1.push_back(j);
      d1.push_back(dj);
      max_d=std::min(max_d,dj+r_[j]);
    }
  }

  // Pass through the data again to prune out far clusters
  near_c.resize(0);
  for (unsigned i=0;i<c1.size();++i)
  {
    if (d1[i]-r_[c1[i]]<=max_d) near_c.push_back(c1[i]);
  }
}

//: Return indices of clusters which may contain nearest point to t
//  Searches through clusters listed in c_list.
//  On input, max_d gives initial limit on distance.
//  On exit, max_d gives the revised limit on the distance
template<class T, class D>
void mbl_clusters<T,D>::nearest_clusters(const T& t, double& max_d,
                           const std::vector<unsigned>& c_list,
                           std::vector<unsigned>& near_c) const
{
  assert(p_.size()>0);

  // Storage for first pass
  std::vector<unsigned> c1;
  std::vector<double> d1;

  // Try each cluster in turn, recording any that might include closest
  for (unsigned int j : c_list)
  {
    double dj = D::d(t,p_[j]);
    if (dj-r_[j]<=max_d)
    {
      c1.push_back(j);
      d1.push_back(dj);
      max_d=std::min(max_d,dj+r_[j]);
    }
  }

  // Pass through the data again to prune out far clusters
  near_c.resize(0);
  for (unsigned i=0;i<c1.size();++i)
  {
    if (d1[i]-r_[c1[i]]<=max_d) near_c.push_back(c1[i]);
  }
}

//: Create a new cluster around point index i
//  Return index of cluster
template<class T, class D>
unsigned mbl_clusters<T,D>::create_cluster(unsigned new_i, double r)
{
  // Create a new cluster using this as a key point
  p_.push_back(data()[new_i]);
  r_.push_back(r);
  std::vector<unsigned> ind(1);
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
unsigned mbl_clusters<T,D>::add_object(unsigned new_i, double r)
{
  assert(new_i<data_->size());

  // If initially empty, create one cluster
  if (p_.size()==0) return create_cluster(new_i,r);

  double d;
  unsigned j=nearest_cluster(data()[new_i],d);
  d+=r;  // Allow for new_i being key point of another cluster, radius r
  if (d<max_r_)
  {
    // Add it to cluster j
    index_[j].push_back(new_i);
    if (d>r_[j]) r_[j]=d;  // Update max radius of cluster
  }
  else
    j=create_cluster(new_i,r);

  return j;
}

//: Assign object data()[i] to cluster ci, knowing distance r.
//  r is the distance D::d(data()[i],p()[ci])
template<class T, class D>
void mbl_clusters<T,D>::assign_to_cluster(unsigned i, unsigned ci,
                                          double r)
{
  index_[ci].push_back(i);
  if (r>r_[ci]) r_[ci]=r;
}


//: Append new object with index i (data()[i]), creating a new cluster
//  Return index of resulting cluster, which is initialised with
//  given radius.
unsigned add_cluster(unsigned i, double r=0.0);


//: Finds list of clusters whose keypoint is within d of t
//  Returns number of such clusters. If >0, then nearest_c
//  gives index of cluster with centre nearest to t
template<class T, class D>
unsigned mbl_clusters<T,D>::clusters_within_d(const T& t, double d,
                                              std::vector<unsigned>& c_list,
                                              unsigned& nearest_c,
                                              double& min_d)
{
  c_list.resize(0);
  nearest_c=0;
  min_d = d+1;
  for (unsigned i=0;i<p_.size();++i)
  {
    double di=D::d(t,p_[i]);
    if (di<=d)
    {
      c_list.push_back(i);
      if (di<min_d) { nearest_c=i; min_d=di; }
    }
  }
  return c_list.size();
}

//: Finds list of clusters whose keypoint is within d of t
//  Returns number of such clusters. If >0, then nearest_c
//  gives index of cluster with centre nearest to t
template<class T, class D>
unsigned mbl_clusters<T,D>::clusters_within_d(const T& t, double d,
                                              const std::vector<unsigned>& in_list,
                                              std::vector<unsigned>& c_list,
                                              unsigned& nearest_c,
                                              double& min_d)
{
  c_list.resize(0);
  nearest_c=0;
  min_d = d+1;
  for (unsigned int i : in_list)
  {
    double di=D::d(t,p_[i]);
    if (di<=d)
    {
      c_list.push_back(i);
      if (di<min_d) { nearest_c=i; min_d=di; }
    }
  }
  return c_list.size();
}


//: Finds list of clusters whose keypoint is within max_r() of t
//  Returns number of such clusters. If >0, then nearest_c
//  gives index of cluster with centre nearest to t
template<class T, class D>
unsigned mbl_clusters<T,D>::clusters_within_max_r(const T& t,
                                                  std::vector<unsigned>& c_list,
                                                  unsigned& nearest_c,
                                                  double& min_d)
{
  return clusters_within_d(t,max_r(),c_list,nearest_c,min_d);
}

//: Finds list of clusters whose keypoint is within max_r() of t
template<class T, class D>
unsigned mbl_clusters<T,D>::clusters_within_max_r(const T& t,
                                                  const std::vector<unsigned>& in_list,
                                                  std::vector<unsigned>& c_list,
                                                  unsigned& nearest_c,
                                                  double& min_d)
{
  return clusters_within_d(t,max_r(),in_list,c_list,nearest_c,min_d);
}

//: Create list of object indices in listed clusters
//  Concatenates lists of indices for each cluster in c_list
template<class T, class D>
void mbl_clusters<T,D>::in_clusters(const std::vector<unsigned>& c_list,
                                    std::vector<unsigned>& o_list) const
{
  o_list.resize(0);
  for (unsigned int i : c_list)
  {
    const std::vector<unsigned>& ind = index()[i];
    for (unsigned int j : ind) o_list.push_back(j);
  }
}


//: Write out list of elements in each cluster
template<class T, class D>
void mbl_clusters<T,D>::print_cluster_sets(std::ostream& os) const
{
  for (unsigned i=0;i<index_.size();++i)
  {
    os << i << ") ";
    for (unsigned j=0;j<index_[i].size();++j)
      os<<index_[i][j] << ' ';
    os<<'\n';
  }
}

//: Write out list of elements in each cluster
template<class T, class D>
void mbl_clusters<T,D>::print_summary(std::ostream& os) const
{
  os << " max_r: " << max_r_ << " n_clusters: " << p_.size();
}

template<class T, class D>
short mbl_clusters<T,D>::version_no() const
{
    return 1;
}

template<class T, class D>
void mbl_clusters<T,D>::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,p_);
  vsl_b_write(bfs,r_);
  vsl_b_write(bfs,max_r_);
  vsl_b_write(bfs,index_);
}

template<class T, class D>
void mbl_clusters<T,D>::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case 1:
      vsl_b_read(bfs,p_);
      vsl_b_read(bfs,r_);
      vsl_b_read(bfs,max_r_);
      vsl_b_read(bfs,index_);
    break;

  default:
    std::cerr << "mbl_clusters<T,D>::b_read() "
      "Unexpected version number " << version << std::endl;
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }

  data_=nullptr;
}

//: Binary file stream output operator for class reference
template<class T, class D>
void vsl_b_write(vsl_b_ostream& bfs, const mbl_clusters<T,D>& c)
{
  c.b_write(bfs);
}

//: Binary file stream input operator for class reference
template<class T, class D>
void vsl_b_read(vsl_b_istream& bfs, mbl_clusters<T,D>& c)
{
  c.b_read(bfs);
}

//: Stream output operator for class reference
template<class T, class D>
std::ostream& operator<<(std::ostream& os,const mbl_clusters<T,D>& c)
{
  c.print_summary(os);
  return os;
}


#define MBL_CLUSTERS_INSTANTIATE(T,D) \
template class mbl_clusters< T,D >; \
template void vsl_b_write(vsl_b_ostream& bfs, const mbl_clusters<T,D >& c); \
template void vsl_b_read(vsl_b_istream& bfs, mbl_clusters<T,D >& c); \
template std::ostream& operator<<(std::ostream& os,const mbl_clusters<T,D >& c)

#endif // mbl_clusters_hxx_
