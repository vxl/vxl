#ifndef mbl_clusters_h_
#define mbl_clusters_h_
//:
// \file
// \brief  Class to record clusters of data, for faster neighbour finding
// \author Tim Cootes

#include <vector>
#include <iostream>
#include <vcl_compiler.h>
#include <vsl/vsl_fwd.h>

//:  Class to record clusters of data, for faster neighbour finding
//  Used to record clusters of objects of type T.
//  D::d(T t1, T t2) is a measure of distance between two objects.
//  It must obey the triangle inequality:
//  D::d(t1,t2)<=D::d(t1,t3)+D::d(t2,t3).
//
//  Pointer retained to an external vector of objects.  The class
//  is designed to allow fast location of the nearest of the external
//  objects to a given new object.  It represents the data as a
//  set of key point positions, together with a list of indices into
//  the external data for each cluster.
//
//  Thus to find the nearest neighbour, we first check for proximity
//  to the keypoints, and only consider objects in the clusters
//  which are sufficiently close.
template<class T, class D>
class mbl_clusters
{
 private:
  //: Pointer to external list of objects
  const std::vector<T>* data_;

  //: Cluster key point
  std::vector<T> p_;

  //: Maximum radius for any cluster
  double max_r_;

  //: Indices of objects associated with each cluster
  std::vector<std::vector<unsigned> > index_;

  //: Furthest distance of a cluster object from key point for cluster
  std::vector<double> r_;

 public:
  mbl_clusters();

  //: Empty clusters
  void empty();

  //: Define maximum radius for each cluster
  void set_max_r(double r);

  //: Define external data array (pointer retained)
  //  Empty existing clusters, then process every element of data
  //  to create clusters, by calling add_object()
  void set_data(const std::vector<T>& data);

  //: Define external data array (pointer retained)
  //  Use carefully! This sets the internal pointer to
  //  point to data.  Really only to be used after loading
  //  internals using b_read(bfs).
  void set_data_ptr(const std::vector<T>& data);

  //: External list of objects
  const std::vector<T>& data() const { return *data_; }

  //: Maximum radius for any cluster
  double max_r() const { return max_r_; }

  //: Cluster key points
  const std::vector<T>& p() const { return p_; }

  //: Number of clusters
  unsigned n_clusters() const { return p_.size(); }

  //: Furthest distance of a cluster object from key point for cluster
  const std::vector<double>& r() const { return r_; }

  //: Indices of objects associated with each cluster
  const std::vector<std::vector<unsigned> >& index() const
   { return index_; }

  //: Set given radius
  void set_r(unsigned i, double r) { r_[i]=r; }

  //: Return index of nearest object in data() to t
  //  Nearest object in data() to t is given by data()[nearest(t,d)];
  //  The distance to the point is d
  unsigned nearest(const T& t, double& d) const;

  //: Return index of nearest object in data() to t
  //  Consider only objects in clusters given in c_list
  //  Nearest object in data() to t is given by data()[nearest(t,d)];
  //  The distance to the point is d
  unsigned nearest(const T& t, double& d,
                   const std::vector<unsigned>& c_list) const;

  //: Return index of nearest cluster in data() to t
  //  Finds nearest cluster key point to t
  //  The distance to the point is d
  unsigned nearest_cluster(const T& t, double& d) const;

  //: Return indices of clusters which may contain nearest point to t
  //  Searches through all the clusters, returning list in near_c
  void nearest_clusters(const T& t, double& max_d,
                        std::vector<unsigned>& near_c) const;

  //: Return indices of clusters which may contain nearest point to t
  //  Searches through clusters listed in c_list.
  //  On input, max_d gives initial limit on distance.
  //  On exit, max_d gives the revised limit on the distance
  void nearest_clusters(const T& t, double& max_d,
                        const std::vector<unsigned>& c_list,
                        std::vector<unsigned>& near_c) const;

  //: Append new object with index i and assign to a cluster
  //  Assumes that new object data()[i] is available.
  //  Deduce which cluster it belongs to and add it.
  //  Create new cluster if further than max_r() from any.
  //  r is the radius associated with data()[i], which is
  //  zero for a single point, but non-zero when the point
  //  is itself a key point for a cluster (eg in mbl_cluster_tree)
  //  Return index of cluster it is assigned to
  unsigned add_object(unsigned i, double r=0.0);

  //: Create a new cluster around point data()[i]
  //  Assumes that new object data()[i] is available.
  //  Return index of cluster
  unsigned create_cluster(unsigned i, double r=0.0);

  //: Assign object data()[i] to cluster ci, knowing distance r.
  //  r is the distance D::d(data()[i],p()[ci])
  void assign_to_cluster(unsigned i, unsigned ci, double r);

  //: Finds list of clusters whose keypoint is within d of t
  //  Returns number of such clusters. If >0, then nearest_c
  //  gives index of cluster with centre nearest to t
  unsigned clusters_within_d(const T& t, double d,
                             std::vector<unsigned>& c_list,
                             unsigned& nearest_c,
                             double& min_d);

  //: Finds list of clusters whose keypoint is within max_r of t
  //  Returns number of such clusters. If >0, then nearest_c
  //  gives index of cluster with centre nearest to t
  unsigned clusters_within_max_r(const T& t,
                                 std::vector<unsigned>& c_list,
                                 unsigned& nearest_c,
                                 double& min_d);

  //: Finds list of clusters whose keypoint is within d of t
  //  Only considers subset of clusters listed in in_list
  //  Returns number of such clusters. If >0, then nearest_c
  //  gives index of cluster with centre nearest to t
  unsigned clusters_within_d(const T& t, double d,
                             const std::vector<unsigned>& in_list,
                             std::vector<unsigned>& c_list,
                             unsigned& nearest_c,
                             double& min_d);

  //: Finds list of clusters whose keypoint is within max_r of t
  //  Only considers subset of clusters listed in index
  //  Returns number of such clusters. If >0, then nearest_c
  //  gives index of cluster with centre nearest to t
  unsigned clusters_within_max_r(const T& t,
                             const std::vector<unsigned>& in_list,
                             std::vector<unsigned>& c_list,
                             unsigned& nearest_c,
                             double& min_d);

  //: Create list of object indices in listed clusters
  //  Concatenates lists of indices for each cluster in c_list
  void in_clusters(const std::vector<unsigned>& c_list,
                   std::vector<unsigned>& o_list) const;

  //: Write out list of elements in each cluster
  void print_cluster_sets(std::ostream& os) const;

  //: Version number for I/O
  short version_no() const;

  //: Print class to os
  void print_summary(std::ostream& os) const;

  //: Save class to binary file stream.
  //  Warning: Does not save external data - that must
  //  be recorded separately.
  void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  //  Warning: Does not load or link external data - that must
  //  be recorded separately, then connected using set_data_ptr()
  void b_read(vsl_b_istream& bfs);
};

//: Binary file stream output operator for class reference
template<class T, class D>
void vsl_b_write(vsl_b_ostream& bfs, const mbl_clusters<T,D>& c);

//: Binary file stream input operator for class reference
template<class T, class D>
void vsl_b_read(vsl_b_istream& bfs, mbl_clusters<T,D>& c);

//: Stream output operator for class reference
template<class T, class D>
std::ostream& operator<<(std::ostream& os,const mbl_clusters<T,D>& c);

#endif // mbl_clusters_h_
