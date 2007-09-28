#ifndef mbl_cluster_tree_h_
#define mbl_cluster_tree_h_
//:
// \file
// \brief  Record trees of clusters of data, for faster neighbour finding
// \author Tim Cootes

#include <mbl/mbl_clusters.h>
#include <vcl_iosfwd.h>

//: Record trees of clusters of data, for fast neighbour finding
//  Used to record clusters of objects of type T.
//  D::d(T t1, T t2) is a measure of distance between two objects.
//  It must obey the triangle inequality: D::d(t1,t2)<=D::d(t1,t3)+D::d(t2,t3).
//
//  The class is designed to allow fast location of the nearest 
//  example in a set objects to a given new object.
//  It represents the data as a
//  set of key point positions, together with a list of indices into
//  the external data for each cluster.  Each cluster is in turn
//  assigned to a large cluster at a higher level.
//
//  Thus to find the nearest neighbour, we first check for proximity
//  to the keypoints, and only consider objects in the clusters
//  which are sufficiently close.
template<class T, class D>
class mbl_cluster_tree
{
 private:
  //: Storage for objects
  vcl_vector<T> data_;

  //: Clusters
  vcl_vector<mbl_clusters<T,D> > cluster_;

  //: Indicate which cluster each object is assigned to.
  //  parent_[0][i] indicates which cluster in cluster_[0] data_[i]
  //  is assigned to.
  //  parent_[j][i] (j>0) indicates which cluster in level above
  //  cluster_[j-1].p()[i] is assigned to.
  vcl_vector<vcl_vector<unsigned> > parent_;

  //: Empty clusters
  void empty();

  //: Append new object with index i and assign to clusters.
  //  Assumes that new object data()[i] is available. 
  //  Deduce which clusters belongs to and add it.
  //  Create new clusters if further than max_r() from any.
  void add_object(unsigned i);

 public:
  mbl_cluster_tree();

  //: Define number of levels and max radius of clusters at each level
  void set_max_r(const vcl_vector<double>& r);

  //: Copy in data
  //  Empty existing clusters, then process every element of data
  //  to create clusters, by calling add_object() 
  void set_data(const vcl_vector<T>& data);

  //: Add an extra element to data()
  void push_back(const T& t);

  //: List of objects
  const vcl_vector<T>& data() const { return data_; }

  //: Return index of nearest object in data() to t
  //  Nearest object in data() to t is given by data()[nearest(t,d)];
  //  The distance to the point is d
  unsigned nearest(const T& t, double& d) const;

  //: Print ancestry of every element
  void print_tree(vcl_ostream& os) const;

  //: Version number for I/O
  short version_no() const;

  //: Print class to os
  void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream.
  void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs);
};

//: Binary file stream output operator for class reference
template<class T, class D>
void vsl_b_write(vsl_b_ostream& bfs, const mbl_cluster_tree<T,D>& c);

//: Binary file stream input operator for class reference
template<class T, class D>
void vsl_b_read(vsl_b_istream& bfs, mbl_cluster_tree<T,D>& c);

//: Stream output operator for class reference
template<class T, class D>
vcl_ostream& operator<<(vcl_ostream& os,const mbl_cluster_tree<T,D>& c);

#endif // mbl_cluster_tree_h_
