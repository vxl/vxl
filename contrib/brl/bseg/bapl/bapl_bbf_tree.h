// This is algo/bapl/bapl_bbf_tree.h
#ifndef bapl_bbf_tree_h_
#define bapl_bbf_tree_h_
//:
// \file
// \brief Best-Bin-First search (extended KD-tree)
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date Sun Nov 9 2003
//
// This implementation is based on the kd-tree code found in contrib/rsdl
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vnl/vnl_vector_fixed.h>
#include <bapl/bapl_keypoint_sptr.h>

//: A bounding box for BBF nodes
class bapl_bbf_box
{
public:
  //: Constructor
  bapl_bbf_box();
  bapl_bbf_box( const vnl_vector_fixed<double,128>& min_point,
                const vnl_vector_fixed<double,128>& max_point );

  //: Copy constructor
  bapl_bbf_box( const bapl_bbf_box& old );

  //: Assignment operator
  bapl_bbf_box& operator= ( const bapl_bbf_box& old );

  //: Minimum point in all boundary dimensions
  vnl_vector_fixed<double,128> min_point_;
  //: Maximum point in all boundary dimensions
  vnl_vector_fixed<double,128> max_point_;
};


//: Return the minimum square distance between \a p and any point in \a b.
double
bapl_bbf_dist_sq( const bapl_keypoint_sptr p, const bapl_bbf_box& b );


class bapl_bbf_node;
typedef vbl_smart_ptr<bapl_bbf_node> bapl_bbf_node_sptr;

//: A node in the BBF tree
class bapl_bbf_node : public vbl_ref_count
{
 public:
  //: Constructor for internal node
  bapl_bbf_node( const bapl_bbf_box& outer_box,
                 const bapl_bbf_box& inner_box,
                unsigned int depth )
    : outer_box_(outer_box), inner_box_(inner_box), depth_(depth),
      point_indices_(0), left_(0), right_(0) {}

  //: Constuctor for leaf node
  bapl_bbf_node( const bapl_bbf_box& outer_box,
                 const bapl_bbf_box& inner_box,
                 unsigned int depth,
                 const vcl_vector<int>& indices )
    : outer_box_(outer_box), inner_box_(inner_box), depth_(depth),
      point_indices_(indices), left_(0), right_(0) {}
  
  //: Outer bounding box
  bapl_bbf_box outer_box_;
  //: Inner bounding box
  bapl_bbf_box inner_box_;
  //: Depth of node in the tree
  unsigned int depth_;
  //: Indices of the points stored at this leaf
  vcl_vector< int > point_indices_;
  //: Left child
  bapl_bbf_node_sptr left_;
  //: Right child
  bapl_bbf_node_sptr right_;
};


// A light-weight class used in the priorty queue
class bapl_bbf_queue_entry
{
public:
  //: Constructor
  bapl_bbf_queue_entry() {}
  //: Constructor
  bapl_bbf_queue_entry( double dist, bapl_bbf_node_sptr node )
    : dist_(dist), node_(node) {}
  //: Used in sorting by distance
  bool operator< ( const bapl_bbf_queue_entry& right ) const
  { return right.dist_ < this->dist_; } 

  //: Distance to this node
  double dist_;
  //: Smart pointer to the node
  bapl_bbf_node_sptr node_;
};


//: The BBF tree data structure
class bapl_bbf_tree
{
public:
  //: Constuctor
  bapl_bbf_tree( const vcl_vector< bapl_keypoint_sptr >& points,
                 int points_per_leaf=4 );

  //: Return an estimate of the n closest points to the query point
  void n_nearest( const bapl_keypoint_sptr query_point,
                  vcl_vector< bapl_keypoint_sptr >& closest_points,
                  vcl_vector< int >& closest_indices,
                  int n=1, int max_search_nodes=-1 );
  
  //: Return an estimate of the n closest points to the query point
  // \param n is the number of nearest nodes to return
  // \param max_search_nodes is the number of nodes to examine (-1 mean all)
  void n_nearest( const bapl_keypoint_sptr query_point,
                  vcl_vector< bapl_keypoint_sptr >& closest_points,
                  int n=1, int max_search_nodes=-1);
  
private:
  //: Build the tree
  bapl_bbf_node_sptr build_tree( int points_per_leaf,
                                 const bapl_bbf_box& outer_box,
                                 int depth,
                                 vcl_vector< int >& indices );
  //: Build an inner bounding box
  bapl_bbf_box build_inner_box( const vcl_vector< int >& indices );
  //: Find the dimension with the greatest variation
  int greatest_variation( const vcl_vector<int>& indices );
  //: Update
  void update_closest( const bapl_keypoint_sptr query_point, int n,
                       bapl_bbf_node_sptr p, vcl_vector< int >& closest_indices,
                       vcl_vector< double >& sq_distances, int & num_found );
  //: See if the current leaf contains the NN neighbors
  bool bounded_at_leaf( const bapl_keypoint_sptr query_point, int n,
                        bapl_bbf_node_sptr current, const vcl_vector< double >& sq_distances,
                        int & num_found );

  //: The number of leaves in the tree
  int leaf_count_;
  //: The number of leaves examined in a search
  int leaves_examined_;
  //: The number of internal nodes in the tree
  int internal_count_;
  //: The number of internal nodes examined in a search
  int internal_examined_;
  //: the root node in the tree
  bapl_bbf_node_sptr root_;
  //: vector of keypoints in the tree
  vcl_vector< bapl_keypoint_sptr > points_;
};

#endif // bapl_bbf_tree_h_
