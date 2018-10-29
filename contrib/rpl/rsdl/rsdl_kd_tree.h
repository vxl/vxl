#ifndef rsdl_kd_tree_h_
#define rsdl_kd_tree_h_
//:
// \file

#include <iostream>
#include <utility>
#include <vector>
#include <rsdl/rsdl_bounding_box.h>
#include <rsdl/rsdl_point.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_compiler.h>

class rsdl_kd_node
{
 public:
  //: ctor for internal node
  rsdl_kd_node( const rsdl_bounding_box& outer_box,
                const rsdl_bounding_box& inner_box,
                unsigned int depth )
    : outer_box_(outer_box), inner_box_(inner_box), depth_(depth),
      point_indices_(0), left_(nullptr), right_(nullptr) {}

  //: ctor for leaf node
  rsdl_kd_node( const rsdl_bounding_box& outer_box,
                const rsdl_bounding_box& inner_box,
                unsigned int depth,
                std::vector<int>  indices )
    : outer_box_(outer_box), inner_box_(inner_box), depth_(depth),
      point_indices_(std::move(indices)), left_(nullptr), right_(nullptr) {}

  //: outer bounding box in both cartesian and angular dimensions
  rsdl_bounding_box outer_box_;
  //: inner bounding box in both cartesian and angular dimensions
  rsdl_bounding_box inner_box_;
  //: depth of node in the tree
  unsigned int depth_;
  //: indices of the points stored at this leaf
  std::vector< int > point_indices_;
  //: left child
  rsdl_kd_node* left_;
  //: right child
  rsdl_kd_node* right_;
};


class rsdl_kd_heap_entry
{
 public:
  rsdl_kd_heap_entry() = default;
  rsdl_kd_heap_entry( double dist, rsdl_kd_node* p )
    : dist_(dist), p_(p) {}
  bool operator< ( const rsdl_kd_heap_entry& right ) const
  { return right.dist_ < this->dist_; }  // kludge because max heap

  double dist_;
  rsdl_kd_node* p_;
};


class rsdl_kd_tree : public vbl_ref_count
{
 private:
  //: copy ctor is private, for now, to prevent its use
  rsdl_kd_tree( const rsdl_kd_tree& /*old*/ ): vbl_ref_count() {}

  //: operator= is private, for now, to prevent its use
  rsdl_kd_tree& operator=( const rsdl_kd_tree& /*old*/ ) { return *this; }

 public:
  //: ctor requires the points and values associated with the tree;
  rsdl_kd_tree( std::vector< rsdl_point >  points,
                double min_angle = 0,
                int points_per_leaf=4 );

  //: dtor deletes the nodes in tree
  ~rsdl_kd_tree() override;

  //: find the n points nearest to the query point (and their associate indices).
  // max_leaves = -1 to not use approximate nearest neighbor queries;
  // if max_leaves is not -1 then use_heap must be true
  void n_nearest( const rsdl_point& query_point,
                  int n,
                  std::vector< rsdl_point >& closest_points,
                  std::vector< int >& indices,
                  bool use_heap = false,
                  int max_leaves = -1 );

  //: find all points within a query's bounding box
  void points_in_bounding_box( const rsdl_bounding_box& box,
                               std::vector< rsdl_point >& closest_points,
                               std::vector< int >& indices );

  //: find all points within a given distance of the query_point.
  void points_in_radius( const rsdl_point& query_point,
                         double radius,
                         std::vector< rsdl_point >& points,
                         std::vector< int >& indices );

 private:
  rsdl_kd_node* root_;

  std::vector< rsdl_point > points_;

  unsigned int Nc_, Na_; // number of cartesian and angular dimensions
  double min_angle_;

  int leaf_count_;
  int leaves_examined_;
  int internal_count_;
  int internal_examined_;

 private:
  void destroy_tree( rsdl_kd_node*& p );

  rsdl_kd_node* build_kd_tree( int points_per_leaf,
                               const rsdl_bounding_box& outer_box,
                               int depth,
                               std::vector< int >& indices );

  rsdl_bounding_box build_inner_box( const std::vector< int >& indices );

  void greatest_variation( const std::vector<int>& indices,
                           bool& use_cartesian, int& dim );

  void n_nearest_with_stack( const rsdl_point& query_point,
                             int n,
                             rsdl_kd_node* root,
                             std::vector< int >& closest_indices,
                             std::vector< double >& sq_distances,
                             int & num_found );

  void n_nearest_with_heap( const rsdl_point& query_point,
                            int n,
                            rsdl_kd_node* root,
                            std::vector< int >& closest_indices,
                            std::vector< double >& sq_distances,
                            int & num_found,
                            int max_leaves);

  void update_closest( const rsdl_point& query_point,
                       int n,
                       rsdl_kd_node* p,
                       std::vector< int >& closest_indices,
                       std::vector< double >& sq_distances,
                       int & num_found );

  bool bounded_at_leaf ( const rsdl_point& query_point,
                         int n,
                         rsdl_kd_node* current,
                         const std::vector< double >& sq_distances,
                         int & num_found );

  void points_in_bounding_box( rsdl_kd_node* current,
                               const rsdl_bounding_box& box,
                               std::vector< int >& indices );

  void report_all_in_subtree( rsdl_kd_node* current,
                              std::vector< int >& indices );
};

#endif // rsdl_kd_tree_h_
