// This is brl/bbas/bsta/bsta_k_medoid.h
#ifndef bsta_k_medoid_h_
#define bsta_k_medoid_h_
//:
// \file
// \brief Form k clusters using distance to representative objects (medoids)
// \author Joseph Mundy
// \date 6/11/05
//
// A clustering algorithm based on the distance within the cluster to
// a representative element and the total distance between representatives. 
// The input is a n x n matrix of pairwise distances
// The output is a set of k representatives (medoids) that minimize the 
// sum of the average distance from each medoid to other elements 
// closest to the medoid and the average distance between medoids.
//
// For k = 1 the medoid would be the element that minimizes the average
// distance to all other elements. The elements are indexed from 0 to n-1.
//
//  Fairly computationally expensive:  The space requirement is n x n 
//  The time is k x (n - k) x (n - k) x number of swaps to minimize 
//  total distance.  There might be on the order of k swaps (or worse).
// \verbatim
//  Modifications
// \endverbatim
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_utility.h>
#include <vcl_iostream.h>
#include <vbl/vbl_array_2d.h>
class bsta_k_medoid
{
 public:
  bsta_k_medoid(unsigned n_elements, bool verbose = false);
  ~bsta_k_medoid(){}

  //:insert a distance into the array, the entry j, i is automatically added
  inline void insert_distance(const i, const j, double d)
  {assert((i<n_elements_)&&(j<n_elements_));
  distance_array_[i][j] = d; distance_array_[j][i] = d;}

  //:The distance between two elements
  inline double distance(const unsigned i, const unsigned j) const
    {assert((i<n_elements_)&&(j<n_elements_)); return distance_array_[i][j];}
  
  //:cluster the elements using k medoids
  void do_clustering(const unsigned k);

  //:get number of mediods
  inline unsigned k() const
    {return medoids_.size();}

  //:get a mediod
  unsigned medoid(const unsigned i) const
    {assert(i<medoids_.size()); return medoids_[i];}

  //: is an element a medoid?
  bool is_medoid(const unsigned i) const;

  //:number of elements in cluster k
  inline unsigned size(const unsigned k) const
    {assert(k<this->k());return clusters_[k].size();}

  //:the elements in cluster k
  inline vcl_vector<unsigned> elements(const unsigned k)
    {assert(k<this->k());return clusters_[k];}

  //: is an element in cluster k ?
  bool in_cluster(const unsigned i, const unsigned k) const;

  //:the distance between an element and its medoid
  double medoid_distance(const unsigned i) const;

  //:the total distance between elements and the medoid in cluster k
  double total_distance(const unsigned k) const;

  //: print distance array (for debugging)
  inline void print_distance_array(vcl_ostream & str = vcl_cout)
    {str << '\n' << distance_array_ << '\n';}

 protected:
  
  //: avg distance change for element i reslting from swapping medoids, j->k. 
  double dc(const unsigned i, const unsigned j, const unsigned k);

  //: avg inter-medoid distance change resulting from swapping medoids, j->k. 
  double dcm(const unsigned j, const unsigned k);
    
  //: replace medoid k with medoid k in the set of medoids
  bool replace_medoid(const unsigned j, const unsigned k);

  //: determine if a swap of j with k leads to a reduction in distance
  bool test_medoid_swap(unsigned& mj, unsigned& mk);

  //: clear the cluster sets
  void clear_clusters();

  //: assign non-medoids to their nearest medoid, forming clusters
  void form_clusters();

 private:
  //:print useful debug messages
  bool verbose_;

  //:the size of the distance array
  unsigned n_elements_;

  //: the k medoids
  vcl_vector<unsigned> medoids_;

  //: The set of elements closest to a given medoid
  vcl_vector<vcl_vector<unsigned> > clusters_;

  //: The array of pair-wise distances between elements
  vbl_array_2d<double> distance_array_;
  
};

#endif // bsta_k_medoid_h_
