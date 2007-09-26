// This is mul/mbl/mbl_cluster_tree.txx
#ifndef mbl_cluster_txx_
#define mbl_cluster_txx_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//:
// \file
// \brief  Record trees of clusters of data, for faster neighbour finding
// \author Tim Cootes

#include "mbl_cluster_tree.h"
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>
#include <vcl_cassert.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>

//: Default constructor
template<class T, class D>
mbl_cluster_tree<T,D>::mbl_cluster_tree()
  : data_(0)
{
}

//: Empty clusters
template<class T, class D>
void mbl_cluster_tree<T,D>::empty()
{
  for (unsigned i=0;i<cluster_.size();++i)
  {
    cluster_[i].empty();
    parent_[i].resize(0);
  }
}

//: Define number of levels and max radius of clusters at each level
template<class T, class D>
void mbl_cluster_tree<T,D>::set_max_r(const vcl_vector<double>& r)
{
  empty();
  unsigned nL = r.size();
  cluster_.resize(nL);
  for (unsigned i=0;i<nL;++i) cluster_[i].set_max_r(r[i]);
  parent_.resize( nL);
  for (unsigned i=0;i<nL;++i) parent_[i].resize(0);

  cluster_[0].set_data(data_);
  for (unsigned i=1;i<nL;++i)
    cluster_[i].set_data(cluster_[i-1].p());
}


//: Define external data array (pointer retained)
//  Empty existing clusters, then process every element of data
//  to create clusters, by calling add_object() 
template<class T, class D>
void mbl_cluster_tree<T,D>::set_data(const vcl_vector<T>& data)
{
  empty();
  data_ = data;
  parent_[0].resize(data.size());
  unsigned n = data.size();
  for (unsigned i=0;i<n;++i)  add_object(i);
}

//: Add an extra element to data() 
template<class T, class D>
void mbl_cluster_tree<T,D>::push_back(const T& t)
{
  data_.push_back(t);
  parent_[0].push_back(0);  // Create space for parent
  add_object(data().size()-1);
}

//: Return index of nearest object in data() to t
//  Nearest object in data() to t is given by data()[nearest(t,d)];
//  The distance to the point is d
template<class T, class D>
unsigned mbl_cluster_tree<T,D>::nearest(const T& t, double& d) const
{
  assert(data().size()>0);

  if (cluster_.size()==1) return cluster_[0].nearest(t,d);

  // Perform hierarchical search
  // Find possible clusters at top level
  int L=cluster_.size()-1;
  vcl_vector<unsigned> near_c0, near_c1;
  double max_d;
  cluster_[L].nearest_clusters(t,max_d,near_c1);

  cluster_[L].in_clusters(near_c1,near_c0);  // Find objects in next L
  L--;
  while (L>0)
  {
    cluster_[L].nearest_clusters(t,max_d,near_c0,near_c1);
    cluster_[L].in_clusters(near_c1,near_c0);   // Find objects in next L
    --L;
  }
  return cluster_[0].nearest(t,d,near_c0);
}

//: Append new object with index i and assign to clusters
//  Assumes that new object data()[i] is available. 
//  Deduce which cluster it belongs to and add it.
//  Create new cluster if further than max_r() from any.
template<class T, class D>
void mbl_cluster_tree<T,D>::add_object(unsigned new_i)
{
  assert(new_i<data().size());

  if (cluster_.size()==1) { cluster_[0].add_object(new_i); return; }

  const T& t = data()[new_i];

  unsigned Lhi = cluster_.size()-1;

  // Find any clusters at top level which could hold t
  vcl_vector<unsigned> c_list0;
  vcl_vector<unsigned>  nearest_c(Lhi+1);
  vcl_vector<double>  nearest_d(Lhi+1);

  if (cluster_[Lhi].clusters_within_max_r(t,c_list0,nearest_c[Lhi],
                                                   nearest_d[Lhi])==0)
  {
    // No clusters at the top level can include t, so create new
    // cluster at each level.
    unsigned cL0=cluster_[0].create_cluster(new_i);
    parent_[0][new_i]=cL0;
    for (unsigned L=1;L<=Lhi;++L)
    {
      unsigned cL1=cluster_[L].create_cluster(cL0);
      parent_[L].push_back(cL1);
      cL0=cL1;  // Record position
    }
    return;
  }

  for (int L=Lhi-1;L>=0;--L)
  {
    vcl_vector<unsigned> c_list1=c_list0;

     // Generate list of elements in each cluster to process at next level
    cluster_[L+1].in_clusters(c_list1,c_list0);

    // Find clusters at level L which could contain t
    unsigned nc=cluster_[L].clusters_within_max_r(t,c_list0,c_list1,
                                       nearest_c[L],nearest_d[L]);

    if (nc==0)
    {
      // No clusters at this level can include t, 
      // so create new cluster at levels 0..L
      unsigned cL=cluster_[0].create_cluster(new_i);
      parent_[0][new_i]=cL;
      if (parent_.size()>1)
        parent_[1].push_back(0);  // Make space for new one

      for (unsigned L1=1;L1<=L;++L1)
      {
        unsigned cL1=cluster_[L1].create_cluster(cL);
        if (L1<Lhi) 
          parent_[L1+1].push_back(0);  // Create space for record of new
        parent_[L1][cL]=cL1;  
        cL=cL1;  // Record index
      }

      // cL is cluster containing new object at level L
      // Assign to nearest cluster in level above (c)
      unsigned c=nearest_c[L+1];
      cluster_[L+1].assign_to_cluster(cL,c,nearest_d[L+1]);
      parent_[L+1][cL]=c;  // Record parent 

        // Track back through ancestors of c, updating radii
      for (unsigned L1=L+2;L1<=Lhi;++L1)
      {
        c = parent_[L1][c];  
        double d=D::d(t,cluster_[L1].p()[c]);
        if (d>cluster_[L1].r()[c])
          cluster_[L1].set_r(c,d);
      }
      return;
    }
    vcl_swap(c_list0,c_list1);  // Set c_list0 to current valid list
  }

  // If reached here, then t is in range of a cluster at every level
  unsigned c = nearest_c[0];
  cluster_[0].assign_to_cluster(new_i,c,nearest_d[0]);
  parent_[0][new_i]=c;
  for (unsigned L=1;L<=Lhi;++L)
  {
    // Track back through parents of c, updating radii
    c = parent_[L][c];  
    double d=D::d(t,cluster_[L].p()[c]);
    if (d>cluster_[L].r()[c])
      cluster_[L].set_r(c,d);
  }

}

//: Print ancestry of every element
template<class T, class D>
void mbl_cluster_tree<T,D>::print_tree(vcl_ostream& os) const
{
  for (unsigned i=0;i<data().size();++i)
  {
    os << i;
    unsigned p=i;
    for (unsigned L=0;L<cluster_.size();++L)
    {
      p=parent_[L][p];
      os << " - "<<p;
    }
    os<<vcl_endl;
  }
}

//: Print summary information
template<class T, class D>
void mbl_cluster_tree<T,D>::print_summary(vcl_ostream& os) const
{
  for (unsigned i=0;i<cluster_.size();++i)
  {
    os << "Level "<<i<<") max_r: "<<cluster_[i].max_r();
    os << " n_clusters: "<<cluster_[i].p().size()<<vcl_endl;
  }
}

template<class T, class D>
short mbl_cluster_tree<T,D>::version_no() const
{
    return 1;
}

template<class T, class D>
void mbl_cluster_tree<T,D>::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,data_);
  vsl_b_write(bfs,parent_);
  // Write out clusters explicitly to avoid creating another template
  vsl_b_write(bfs,cluster_.size());
  for (unsigned L=0;L<cluster_.size();++L)
    vsl_b_write(bfs,cluster_[L]);
}

template<class T, class D>
void mbl_cluster_tree<T,D>::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  unsigned nc=0;
  switch (version)
  {
    case 1:
      vsl_b_read(bfs,data_);
      vsl_b_read(bfs,parent_);
      vsl_b_read(bfs,nc);
      cluster_.resize(nc);
      for (unsigned L=0;L<nc;++L)
        vsl_b_read(bfs,cluster_[L]);
   break;

  default:
    vcl_cerr << "mbl_cluster_tree<T,D>::b_read() "
      "Unexpected version number " << version << vcl_endl;
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }

  // Connect each cluster to data below
  cluster_[0].set_data(data_);
  for (unsigned L=1;L<cluster_.size();++L)
    cluster_[L].set_data(cluster_[L-1].p());
}

//: Binary file stream output operator for class reference
template<class T, class D>
void vsl_b_write(vsl_b_ostream& bfs, const mbl_cluster_tree<T,D>& c)
{
  c.b_write(bfs);
}

//: Binary file stream input operator for class reference
template<class T, class D>
void vsl_b_read(vsl_b_istream& bfs, mbl_cluster_tree<T,D>& c)
{
  c.b_read(bfs);
}

//: Stream output operator for class reference
template<class T, class D>
vcl_ostream& operator<<(vcl_ostream& os,const mbl_cluster_tree<T,D>& c)
{
  c.print_summary(os);
  return os;
}


#define MBL_CLUSTER_TREE_INSTANTIATE(T,D) \
template class mbl_cluster_tree< T , D >; \
template void vsl_b_write(vsl_b_ostream& bfs, const mbl_cluster_tree<T,D>& c); \
template void vsl_b_read(vsl_b_istream& bfs, mbl_cluster_tree<T,D>& c); \
template vcl_ostream& operator<<(vcl_ostream& os,const mbl_cluster_tree<T,D>& c);

#endif // mbl_cluster_txx_
