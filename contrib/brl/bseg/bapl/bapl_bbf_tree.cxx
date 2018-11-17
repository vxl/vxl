// This is brl/bseg/bapl/bapl_bbf_tree.cxx
#include <limits>
#include <utility>
#include <iostream>
#include <algorithm>
#include "bapl_bbf_tree.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>

#include <bapl/bapl_keypoint.h>


//------------------------------------------------------------------------------
// BBF Bounding Box
//------------------------------------------------------------------------------

//: Constructor
bapl_bbf_box::bapl_bbf_box( const vnl_vector_fixed<double,128>& min_point,
                            const vnl_vector_fixed<double,128>& max_point )
  : min_point_(min_point), max_point_(max_point)
{
  // swap incorrectly ordered values
  for ( unsigned int i=0; i<128; ++i )
    if ( min_point_[i] > max_point_[i] ) {
      double temp = min_point_[i];
      min_point_[i] = max_point_[i];
      max_point_[i] = temp;
    }
}

//: Copy Constructor
bapl_bbf_box::bapl_bbf_box( const bapl_bbf_box& old ) = default;

//: Assignment Operator
bapl_bbf_box &
bapl_bbf_box::operator= ( const bapl_bbf_box& old )
= default;

//: Return the minimum square distance between \a p and any point in \a b.
double
bapl_bbf_dist_sq( const bapl_keypoint_sptr& p, const bapl_bbf_box& b )
{
  double sum_sq = 0;
  unsigned int dim = p->descriptor().size();
  assert( dim == b.min_point_.size() && dim == b.max_point_.size() );

  for ( unsigned int i=0; i<dim; ++i ) {
    double x0 = b.min_point_[i], x1 = b.max_point_[i];
    double x = p->descriptor()[i];
    if ( x < x0 ) {
      sum_sq += vnl_math::sqr( x0 - x );
    }
    else if ( x > x1 ) {
      sum_sq += vnl_math::sqr( x1 - x );
    }
  }

  return sum_sq;
}


//------------------------------------------------------------------------------
// BBF Tree
//------------------------------------------------------------------------------


//: Constructor
bapl_bbf_tree::bapl_bbf_tree(std::vector< bapl_keypoint_sptr >  points, int points_per_leaf)
 : leaf_count_(0), leaves_examined_(0), internal_count_(0),
   internal_examined_(0), points_(std::move(points))
{
  assert(points_per_leaf > 0);

  //  0. Consistency check
  for ( unsigned int i=1; i<points_.size(); ++i ) {
    assert( 128 == points_[i]->descriptor().size() );
    assert( 128 == points_[i]->descriptor().size() );
  }

  // 1.  Build the initial bounding box.
  vnl_vector_fixed<double,128> low(-std::numeric_limits<double>::min());
  vnl_vector_fixed<double,128> high(std::numeric_limits<double>::max());
  bapl_bbf_box box( low, high );

  // 2. create the vector of ids
  std::vector< int > indices( points_.size() );
  for ( unsigned int i=0; i<indices.size(); ++i ) indices[ i ] = i;

  // 3. call recursive function to do the real work
  root_ = build_tree( points_per_leaf, box, 0, indices );

  std::cout << "total leaves = " << leaf_count_ << '\n'
           << "total internal = " << internal_count_ << std::endl;
}


//: For sorting pairs by their first element
namespace{
    bool
    first_less( const std::pair<double,int>& left_pair,
                const std::pair<double,int>& right_pair )
    {
      return left_pair.first < right_pair.first;
    }
}


//: Build the tree
bapl_bbf_node_sptr
bapl_bbf_tree::build_tree( int points_per_leaf,
                           const bapl_bbf_box& outer_box,
                           int depth,
                           std::vector< int >& indices )
{
  unsigned int i;

  // 1. Build the inner box.  This is not done inside the bounding box
  //    class because of the indices.
  bapl_bbf_box inner_box = this->build_inner_box( indices );

  // 2. If only one point is left, create and return a leaf node.
  if ( indices.size() <= (unsigned int)points_per_leaf) {
    leaf_count_++;
    return new bapl_bbf_node ( outer_box, inner_box, depth, indices );
  }

  // 3. Find the dimension along which there is the greatest variation
  //    in the points.
  int dim = this->greatest_variation( indices );

  // 4. Form and then sort a vector of temporary pairs, containing the
  //    indices and the values along the selected dim of the data.
  //    Sort by the values.
  std::vector< std::pair< double, int > > values( indices.size() );
  for ( i=0; i<indices.size(); ++i )
    values[i] = std::pair<double, int> ( points_[ indices[i] ]->descriptor()[dim], indices[i] );
  std::sort( values.begin(), values.end(), first_less );

  // 5. Partition the vector and the bounding box along the dimension.
  unsigned int med_loc = (indices.size()-1) / 2;
  double median_value = (values[med_loc].first + values[med_loc+1].first) / 2;

  bapl_bbf_box left_outer_box( outer_box ), right_outer_box( outer_box );
  left_outer_box.max_point_[dim] = median_value;
  right_outer_box.min_point_[dim] = median_value;

  std::vector< int > left_indices( med_loc+1 ), right_indices( indices.size()-med_loc-1 );
  for ( i=0; i<=med_loc; ++i ) left_indices[i] = values[i].second;
  for ( ; i<indices.size(); ++i ) right_indices[i-med_loc-1] = values[i].second;

  // Create a new internal node and branch
  bapl_bbf_node_sptr node = new bapl_bbf_node( outer_box, inner_box, depth );
  internal_count_++;
  node->left_  = this->build_tree( points_per_leaf, left_outer_box,  depth+1, left_indices );
  node->right_ = this->build_tree( points_per_leaf, right_outer_box, depth+1, right_indices );

  return node;
}

//: Build an inner bounding box
bapl_bbf_box
bapl_bbf_tree::build_inner_box( const std::vector< int >& indices )
{
  assert( indices.size() > 0 );
  vnl_vector_fixed<double,128> min_point( points_[ indices[ 0 ] ]->descriptor() );
  vnl_vector_fixed<double,128> max_point( min_point );

  for (unsigned int i=1; i < indices.size(); ++i ) {
    const vnl_vector_fixed<double,128>& pt = points_[ indices[ i ] ]->descriptor();
    for ( unsigned int j=0; j < 128; ++j ) {
      if ( pt[j] < min_point[j] )
        min_point[j] = pt[j];
      if ( pt[j] > max_point[j] )
        max_point[j] = pt[j];
    }
  }
  return bapl_bbf_box( min_point, max_point );
}

//: Find the dimension with the greatest variation
int
bapl_bbf_tree::greatest_variation( const std::vector<int>& indices )
{
  int dim = -1;
  double interval_size = 0.0;

  for ( unsigned int i=0; i<128; ++i ) {
    double min_v, max_v;
    min_v = max_v = points_[ indices[0] ]->descriptor()[0];
    for ( unsigned int j=1; j<indices.size(); ++j ) {
      double v = points_[ indices[j] ]->descriptor()[i];
      if ( v < min_v ) min_v = v;
      if ( v > max_v ) max_v = v;
    }
    if ( dim < 0 || max_v - min_v > interval_size ) {
      dim = i;
      interval_size = max_v - min_v;
    }
  }

  return dim;
}


//: Return an estimate of the n closest points to the query point
void
bapl_bbf_tree::n_nearest( const bapl_keypoint_sptr& query_point,
                          std::vector< bapl_keypoint_sptr >& closest_points,
                          int n, int max_search_nodes)
{
  std::vector< int > closest_indices( n );
  this->n_nearest(query_point, closest_points, closest_indices, n, max_search_nodes);
}

//: Return an estimate of the n closest points to the query point
void
bapl_bbf_tree::n_nearest(const bapl_keypoint_sptr& query_point,
                         std::vector< bapl_keypoint_sptr >& closest_points,
                         std::vector< int >& closest_indices,
                         int n, int max_search_nodes)
{
  assert(n>0);
  assert(max_search_nodes != 0);
  assert( query_point->descriptor().size() == 128 );
  leaves_examined_ = internal_examined_ = 0;

  if ( closest_indices.size() != (unsigned int)n )
    closest_indices.resize( n );

  std::vector< double > sq_distances( n );
  std::vector< bapl_bbf_queue_entry > priority_queue;
  priority_queue.reserve( 100 );    // should be more than enough

  int num_found = 0;
  double left_box_sq_dist, right_box_sq_dist;
  double sq_dist;

  //  Go down the tree, finding the leaf node which contains the query point
  bapl_bbf_node_sptr current = root_;
  while ( current->left_ ) {
    internal_examined_++;

    if ( bapl_bbf_dist_sq( query_point, current->left_->outer_box_ ) < 1.0e-5 ) {
      right_box_sq_dist = bapl_bbf_dist_sq( query_point, current->right_->inner_box_ );
      priority_queue.emplace_back( right_box_sq_dist, current->right_ );
      current = current->left_ ;
    }
    else {
      left_box_sq_dist = bapl_bbf_dist_sq( query_point, current->left_->inner_box_ );
      priority_queue.emplace_back( left_box_sq_dist, current->left_ );
      current = current->right_ ;
    }
  }
  std::make_heap( priority_queue.begin(), priority_queue.end() );
  sq_dist = 0;

   /*std::cout << "\nAfter initial trip down the tree, here's the heap\n";
   int i;
   for ( i=0; i<priority_queue.size(); ++i )
     std::cout << "  " << i << ":  sq distance " << priority_queue[i].dist_
              << ", node depth " << priority_queue[i].node_->depth_ << std::endl;*/

  bool first_leaf = true;

  do {
     //std::cout << "\ncurrent -- sq_dist " << sq_dist << ", depth: " << current->depth_ << '\n';
             //<< current->outer_box_ << "\ninner_box: "
          //<< current->inner_box_ << '\n';
     //std::cout << "heap size: " << heap_vec.size() << std::endl;
    if ( num_found < n || sq_dist < sq_distances[ num_found-1 ] ) {
      if ( ! current->left_ ) {  // a leaf node
         //std::cout << "Leaf\n";
        leaves_examined_ ++ ;
        update_closest( query_point, n, current, closest_indices, sq_distances, num_found );
        if ( first_leaf ) {  // check if we can quit just at this leaf node.
           //std::cout << "First leaf\n";
          first_leaf = false;
          if ( this-> bounded_at_leaf( query_point, n, current, sq_distances, num_found ) )
            break;
        }
      }

      else {
         //std::cout << "Internal\n";
        internal_examined_ ++ ;

        left_box_sq_dist = bapl_bbf_dist_sq( query_point, current->left_->inner_box_ );
         //std::cout << "left sq distance = " << left_box_sq_dist << std::endl;
        if ( num_found < n || sq_distances[ num_found-1 ] > left_box_sq_dist ) {
           //std::cout << "pushing left onto the heap\n";
          priority_queue.emplace_back( left_box_sq_dist, current->left_ );
          std::push_heap( priority_queue.begin(), priority_queue.end() );
        };

        right_box_sq_dist = bapl_bbf_dist_sq( query_point, current->right_->inner_box_ );
         //std::cout << "right sq distance = " << right_box_sq_dist << std::endl;
        if ( num_found < n || sq_distances[ num_found-1 ] > right_box_sq_dist ) {
           //std::cout << "pushing right onto the heap\n";
          priority_queue.emplace_back( right_box_sq_dist, current->right_ );
          std::push_heap( priority_queue.begin(), priority_queue.end() );
        }
      }
    }
    // else std::cout << "skipping node\n";

    if ( leaves_examined_ >= max_search_nodes && max_search_nodes > 0)
      break;
    else if ( priority_queue.size() == 0 )
      break;
    else {
      std::pop_heap( priority_queue.begin(), priority_queue.end() );
      sq_dist = priority_queue.back().dist_;
      current = priority_queue.back().node_;
      priority_queue.pop_back();
    }
  } while ( true );

  /*std::cout << "\nAfter n_nearest, leaves_examined_ = " << leaves_examined_
           << ", fraction = " << float(leaves_examined_) / leaf_count_
           << "\n     internal_examined_ = " << internal_examined_
           << ", fraction = " << float(internal_examined_) / internal_count_ << std::endl;*/

  assert(num_found >= 0);
  if ( closest_points.size() != (unsigned int)num_found )
    closest_points.resize( num_found );

  for ( int i=0; i<num_found; ++i ) {
    closest_points[i] = points_[ closest_indices[i] ];
  }
}

void
bapl_bbf_tree::update_closest( const bapl_keypoint_sptr& query_point,
                               int n,
                               const bapl_bbf_node_sptr& p,
                               std::vector< int >& closest_indices,
                               std::vector< double >& sq_distances,
                               int & num_found )
{
  assert(n>0);
#ifdef DEBUG
  std::cout << "Update_closest for leaf" << '\n'
           << " query_point = " << query_point << '\n'
           << " inner bounding box =\n" << p->inner_box_ << '\n'
           << " sq_dist = " << rsdl_dist_sq( query_point, p->inner_box_ ) << std::endl;
#endif

  for (int id : p->point_indices_) {  // check each id
    double sq_dist = vnl_vector_ssd( query_point->descriptor(), points_[ id ]->descriptor() );
    // std::cout << "  id = " << id << ", point = " << points_[ id ]
    //          << ", sq_dist = " << sq_dist << std::endl;

    // if enough points have been found and the distance of this point is
    // too large then skip it.

    if ( num_found >= n && sq_dist >= sq_distances[ num_found-1 ] )
      continue;

    // Increment the num_found counter if fewer than the desired
    // number have already been found.

    if ( num_found < n ) {
      num_found ++;
    }

    // Insert the id and square distance in order.

    int j=num_found-2;
    while ( j >= 0 && sq_distances[j] > sq_dist ) {
      closest_indices[ j+1 ] = closest_indices[ j ];
      sq_distances[ j+1 ] = sq_distances[ j ];
      j -- ;
    }
    closest_indices[ j+1 ] = id;
    sq_distances[ j+1 ] = sq_dist;
  }
  // std::cout << "  End of leaf computation, num_found =  " << num_found
  //          << ", and they are:" << std::endl;
  // for ( int k=0; k<num_found; ++k )
  //   std::cout << "     " << k << ":  indices: " << closest_indices[ k ]
  //            << ", sq_dist " << sq_distances[ k ] << std::endl;
}


//  If there are already n points (at the first leaf) and if a box
//  drawn around the first point of half-width the distance of the
//  n-th point fits inside the outer box of this node, then no points
//  anywhere else in the tree will can replace any of the closest
//  points.
bool
bapl_bbf_tree::bounded_at_leaf( const bapl_keypoint_sptr& query_point,
                                int n,
                                const bapl_bbf_node_sptr& current,
                                const std::vector< double >& sq_distances,
                                int & num_found )
{
  assert(n>0);
  // std::cout << "bounded_at_leaf\n"
  //          << "num_found = " << num_found << '\n';

  if ( num_found != n ) {
    return false;
  }

  double radius = std::sqrt( sq_distances[ n-1 ] );

  for ( unsigned int i=0; i < query_point->descriptor().size(); ++i ) {
    double x = query_point->descriptor()[ i ];
    if ( current->outer_box_.min_point_[i] > x - radius ||
         current->outer_box_.max_point_[i] < x + radius ) {
      return false;
    }
  }

  return true;
}
