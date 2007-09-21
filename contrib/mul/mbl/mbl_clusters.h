#ifndef mbl_clusters_h_
#define mbl_clusters_h_
//:
// \file
// \brief  Class to record clusters of data, for faster neighbour finding
// \author Tim Cootes

#include <vcl_vector.h>

//:  Class to record clusters of data, for faster neighbour finding
//  Used to record clusters of objects of type T.
//  D(T t1, T t2) is a measure of distance between two objects.
//  It must obey the triangle inequality: D(t1,t2)<=D(t1,t3)+D(t2,t3).
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
class mbl_clusters {
private:
  //: Pointer to external list of objects
  const vcl_vector<T>* data_;

  //: Cluster key point
  vcl_vector<T> p_;

  //: Maximum radius for any cluster 
  double max_r_;

  //: Indices of objects associated with each cluster
  vcl_vector<vcl_vector<unsigned> > index_;

  //: Furthest distance of a cluster object from key point for cluster
  vcl_vector<double> r_;

  //: Create a new cluster around point index i
  // Return index of cluster
  unsigned create_cluster(unsigned i);

  //: Empty clusters
  void empty();
public:
  mbl_clusters();

  //: Define maximum radius for each cluster
  void set_max_r(double r);

  //: Define external data array (pointer retained)
  //  Empty existing clusters, then process every element of data
  //  to create clusters, by calling add_object() 
  void set_data(const vcl_vector<T>& data);

  //: External list of objects
  const vcl_vector<T>& data() const { return *data_; }

  //: Maximum radius for any cluster 
  double max_r() const { return max_r_; }

  //: Cluster key points
  const vcl_vector<T>& p() const { return p_; }

  //: Furthest distance of a cluster object from key point for cluster
  const vcl_vector<double>& r() const { return r_; }

  //: Return index of nearest object in data() to t
  //  Nearest object in data() to t is given by data()[nearest(t,d)];
  //  The distance to the point is d
  unsigned nearest(const T& t, double& d) const;

  //: Return index of nearest object in data() to t
  //  Consider only objects in clusters given in c_list
  //  Nearest object in data() to t is given by data()[nearest(t,d)];
  //  The distance to the point is d
  unsigned nearest(const T& t, double& d,
                   const vcl_vector<unsigned>& c_list) const;

  //: Return index of nearest cluster in data() to t
  //  Finds nearest cluster key point to t 
  //  The distance to the point is d
  unsigned nearest_cluster(const T& t, double& d) const;
  
  //: Append new object with index i and assign to a cluster
  //  Assumes that new object data()[i] is available. 
  //  Deduce which cluster it belongs to and add it.
  //  Create new cluster if further than max_r() from any.
  //  Return index of cluster it is assigned to
  unsigned add_object(unsigned i);
};

#endif // mbl_clusters_h_

