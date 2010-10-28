// This is algo/bapl/bapl_bbf_tree.cxx
//:
// \file

#include "bapl_bbf_tree.h"

#include <vcl_limits.h>
#include <vcl_utility.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
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
bapl_bbf_box::bapl_bbf_box( const bapl_bbf_box& old )
  : min_point_(old.min_point_), max_point_(old.max_point_)
{
}

//: Assignment Operator
bapl_bbf_box &
bapl_bbf_box::operator= ( const bapl_bbf_box& old )
{
  min_point_ = old.min_point_;
  max_point_ = old.max_point_;
  return *this;
}

//: Return the minimum square distance between \a p and any point in \a b.
double
bapl_bbf_dist_sq( const bapl_keypoint_sptr p, const bapl_bbf_box& b )
{
  double sum_sq = 0;
  unsigned int dim = p->descriptor().size();
  assert( dim == b.min_point_.size() && dim == b.max_point_.size() );

  for ( unsigned int i=0; i<dim; ++i ) {
    double x0 = b.min_point_[i], x1 = b.max_point_[i];
    double x = p->descriptor()[i];
    if ( x < x0 ) {
      sum_sq += vnl_math_sqr( x0 - x );
    }
    else if ( x > x1 ) {
      sum_sq += vnl_math_sqr( x1 - x );
    }
  }

  return sum_sq;
}


//------------------------------------------------------------------------------
// BBF Tree
//------------------------------------------------------------------------------


//: Constuctor
bapl_bbf_tree::bapl_bbf_tree(const vcl_vector< bapl_keypoint_sptr >& points, int points_per_leaf)
 : leaf_count_(0), leaves_examined_(0), internal_count_(0),
   internal_examined_(0), points_(points) 
{
  assert(points_per_leaf > 0);
  
  //  0. Consistency check
  for ( unsigned int i=1; i<points_.size(); ++i ) {
    assert( 128 == points_[i]->descriptor().size() );
    assert( 128 == points_[i]->descriptor().size() );
  }

  // 1.  Build the initial bounding box.
  vnl_vector_fixed<double,128> low(-vcl_numeric_limits<double>::min());
  vnl_vector_fixed<double,128> high(vcl_numeric_limits<double>::max());
  bapl_bbf_box box( low, high );
  
  // 2. create the vector of ids
  vcl_vector< int > indices( points_.size() );
  for ( unsigned int i=0; i<indices.size(); ++i ) indices[ i ] = i;

  // 3. call recursive function to do the real work
  root_ = build_tree( points_per_leaf, box, 0, indices );

  vcl_cout << "total leaves = " << leaf_count_ << vcl_endl
           << "total interal = " << internal_count_ << vcl_endl;
}


//: For sorting pairs by their first element
bool
first_less( const vcl_pair<double,int>& left,
            const vcl_pair<double,int>& right )
{
  return left.first < right.first;
}


//: Build the tree
bapl_bbf_node_sptr
bapl_bbf_tree::build_tree( int points_per_leaf,
                           const bapl_bbf_box& outer_box,
                           int depth,
                           vcl_vector< int >& indices )
{
  unsigned int i;

  // 1. Build the inner box.  This is not done inside the bounding box
  //    class because of the indices.
  bapl_bbf_box inner_box = this->build_inner_box( indices );

  // 2. If only one point is left, create and return a leaf node.
  if ( indices.size() <= (unsigned int)points_per_leaf){
    leaf_count_++;
    return new bapl_bbf_node ( outer_box, inner_box, depth, indices );
  }

  // 3. Find the dimension along which there is the greatest variation
  //    in the points.
  int dim = this->greatest_variation( indices );

  // 4. Form and then sort a vector of temporary pairs, containing the
  //    indices and the values along the selected dim of the data.
  //    Sort by the values.
  vcl_vector< vcl_pair< double, int > > values( indices.size() );
  for ( i=0; i<indices.size(); ++i )
    values[i] = vcl_pair<double, int> ( points_[ indices[i] ]->descriptor()[dim], indices[i] );
  vcl_sort( values.begin(), values.end(), first_less );

  // 5. Partition the vector and the bounding box along the dimension.
  unsigned int med_loc = (indices.size()-1) / 2;
  double median_value = (values[med_loc].first + values[med_loc+1].first) / 2;
  
  bapl_bbf_box left_outer_box( outer_box ), right_outer_box( outer_box );
  left_outer_box.max_point_[dim] = median_value;
  right_outer_box.min_point_[dim] = median_value;
  
  vcl_vector< int > left_indices( med_loc+1 ), right_indices( indices.size()-med_loc-1 );
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
bapl_bbf_tree::build_inner_box( const vcl_vector< int >& indices )
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
bapl_bbf_tree::greatest_variation( const vcl_vector<int>& indices )
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
bapl_bbf_tree::n_nearest( const bapl_keypoint_sptr query_point,
                          vcl_vector< bapl_keypoint_sptr >& closest_points,
                          int n, int max_search_nodes)
{
  vcl_vector< int > closest_indices( n );
  this->n_nearest(query_point, closest_points, closest_indices, n, max_search_nodes);
}
                  
//: Return an estimate of the n closest points to the query point
void
bapl_bbf_tree::n_nearest(const bapl_keypoint_sptr query_point,
                         vcl_vector< bapl_keypoint_sptr >& closest_points,
                         vcl_vector< int >& closest_indices,
                         int n, int max_search_nodes)
{
  assert(n>0);
  assert(max_search_nodes != 0);
  assert( query_point->descriptor().size() == 128 );
  leaves_examined_ = internal_examined_ = 0;

  if ( closest_indices.size() != (unsigned int)n )
    closest_indices.resize( n );
    
  vcl_vector< double > sq_distances( n );
  vcl_vector< bapl_bbf_queue_entry > priority_queue;
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
      priority_queue.push_back( bapl_bbf_queue_entry( right_box_sq_dist, current->right_ ) );
      current = current->left_ ;
    }
    else {
      left_box_sq_dist = bapl_bbf_dist_sq( query_point, current->left_->inner_box_ );
      priority_queue.push_back( bapl_bbf_queue_entry( left_box_sq_dist, current->left_ ) );
      current = current->right_ ;
    }
  }
  vcl_make_heap( priority_queue.begin(), priority_queue.end() );
  sq_dist = 0;

//   vcl_cout << "\nAfter initial trip down the tree, here's the heap\n";
//   int i;
//   for ( i=0; i<priority_queue.size(); ++i )
//     vcl_cout << "  " << i << ":  sq distance " << priority_queue[i].dist_
//              << ", node depth " << priority_queue[i].node_->depth_ << vcl_endl;
              
  bool first_leaf = true;

  do {
    // vcl_cout << "\ncurrent -- sq_dist " << sq_dist << ", depth: " << current->depth_
    //         << current->outer_box_ << "\ninner_box: "
    //      << current->inner_box_ << "\n";
    // vcl_cout << "heap size: " << heap_vec.size() << vcl_endl;
    if ( num_found < n || sq_dist < sq_distances[ num_found-1 ] ) {
      if ( ! current->left_ ) {  // a leaf node
        // vcl_cout << "Leaf\n";
        leaves_examined_ ++ ;
        update_closest( query_point, n, current, closest_indices, sq_distances, num_found );
        if ( first_leaf ) {  // check if we can quit just at this leaf node.
          // vcl_cout << "First leaf\n";
          first_leaf = false;
          if ( this-> bounded_at_leaf( query_point, n, current, sq_distances, num_found ) )
            break;
        }
      }

      else {
        // vcl_cout << "Internal\n";
        internal_examined_ ++ ;

        left_box_sq_dist = bapl_bbf_dist_sq( query_point, current->left_->inner_box_ );
        // vcl_cout << "left sq distance = " << left_box_sq_dist << vcl_endl;
        if ( num_found < n || sq_distances[ num_found-1 ] > left_box_sq_dist ) {
          // vcl_cout << "pushing left onto the heap\n";
          priority_queue.push_back( bapl_bbf_queue_entry( left_box_sq_dist, current->left_ ) );
          vcl_push_heap( priority_queue.begin(), priority_queue.end() );
        };

        right_box_sq_dist = bapl_bbf_dist_sq( query_point, current->right_->inner_box_ );
        // vcl_cout << "right sq distance = " << right_box_sq_dist << vcl_endl;
        if ( num_found < n || sq_distances[ num_found-1 ] > right_box_sq_dist ) {
          // vcl_cout << "pushing right onto the heap\n";
          priority_queue.push_back( bapl_bbf_queue_entry( right_box_sq_dist, current->right_ ) );
          vcl_push_heap( priority_queue.begin(), priority_queue.end() );
        }
      }
    }
    // else vcl_cout << "skipping node\n";

    if ( leaves_examined_ >= max_search_nodes && max_search_nodes > 0)
      break;
    else if ( priority_queue.size() == 0 )
      break;
    else {
      vcl_pop_heap( priority_queue.begin(), priority_queue.end() );
      sq_dist = priority_queue.back().dist_;
      current = priority_queue.back().node_;
      priority_queue.pop_back();
    }
  } while ( true );

//  vcl_cout << "\nAfter n_nearest, leaves_examined_ = " << leaves_examined_
//           << ", fraction = " << float(leaves_examined_) / leaf_count_
//           << "\n     internal_examined_ = " << internal_examined_
//           << ", fraction = " << float(internal_examined_) / internal_count_ << vcl_endl;
           
  assert(num_found >= 0);
  if ( closest_points.size() != (unsigned int)num_found )
    closest_points.resize( num_found );

  for ( int i=0; i<num_found; ++i ) {
    closest_points[i] = points_[ closest_indices[i] ];
  }
}

void
bapl_bbf_tree::update_closest( const bapl_keypoint_sptr query_point,
                               int n,
                               bapl_bbf_node_sptr p,
                               vcl_vector< int >& closest_indices,
                               vcl_vector< double >& sq_distances,
                               int & num_found )
{
  assert(n>0);
  // vcl_cout << "Update_closest for leaf " << vcl_endl;
  // vcl_cout << " query_point = " << query_point << vcl_endl;
  // vcl_cout << " inner bounding box = \n" << p->inner_box_ << vcl_endl;
  // vcl_cout << " sq_dist = " << rsdl_dist_sq( query_point, p->inner_box_ ) << vcl_endl;

  for ( unsigned int i=0; i < p->point_indices_.size(); ++i ) {  // check each id
    int id = p->point_indices_[i];
    double sq_dist = vnl_vector_ssd( query_point->descriptor(), points_[ id ]->descriptor() );
    // vcl_cout << "  id = " << id << ", point = " << points_[ id ]
    //          << ", sq_dist = " << sq_dist << vcl_endl;

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
  // vcl_cout << "  End of leaf computation, num_found =  " << num_found
  //          << ", and they are: " << vcl_endl;
  // for ( int k=0; k<num_found; ++k )
  //   vcl_cout << "     " << k << ":  indices: " << closest_indices[ k ]
  //            << ", sq_dist " << sq_distances[ k ] << vcl_endl;
}


//  If there are already n points (at the first leaf) and if a box
//  drawn around the first point of half-width the distance of the
//  n-th point fits inside the outer box of this node, then no points
//  anywhere else in the tree will can replace any of the closest
//  points.
bool
bapl_bbf_tree::bounded_at_leaf( const bapl_keypoint_sptr query_point,
                                int n,
                                bapl_bbf_node_sptr current,
                                const vcl_vector< double >& sq_distances,
                                int & num_found )
{
  assert(n>0);
  // vcl_cout << "bounded_at_leaf\n"
  //          << "num_found = " << num_found << "\n";

  if ( num_found != n ) {
    return false;
  }

  double radius = vcl_sqrt( sq_distances[ n-1 ] );

  for ( unsigned int i=0; i < query_point->descriptor().size(); ++i ) {
    double x = query_point->descriptor()[ i ];
    if ( current->outer_box_.min_point_[i] > x - radius ||
         current->outer_box_.max_point_[i] < x + radius ) {
      return false;
    }
  }

  return true;
}
