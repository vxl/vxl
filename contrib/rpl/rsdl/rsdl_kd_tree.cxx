// This is rpl/rsdl/rsdl_kd_tree.cxx
#include "rsdl_kd_tree.h"

#include <vcl_algorithm.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_utility.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_numeric_traits.h>

#include <rsdl/rsdl_dist.h>


rsdl_kd_tree::rsdl_kd_tree( const vcl_vector< rsdl_point >& points,
                            double min_angle,
                            int points_per_leaf )
  : points_(points), min_angle_(min_angle)
{
  assert(points_per_leaf > 0);

  Nc_ = points_[0].num_cartesian();
  Na_ = points_[0].num_angular();

  //  0. Consistency check

  for ( unsigned int i=1; i<points_.size(); ++i ) {
    assert( Nc_ == points_[i].num_cartesian() );
    assert( Na_ == points_[i].num_angular() );
  }

  // 1.  Build the initial bounding box.

  rsdl_point low(Nc_, Na_), high(Nc_, Na_);

  // 1a. initialize the cartesian upper and lower limits
  if ( Nc_ > 0 ) {
    for ( unsigned int i=0; i<Nc_; ++i ) {
      low.cartesian(i)  = -vnl_numeric_traits<double>::maxval;
      high.cartesian(i) =  vnl_numeric_traits<double>::maxval;
    }
  }

  // 1b. initialize the angular upper and lower limits
  if ( Na_ > 0 ) {
    for ( unsigned int i=0; i<Na_; ++i ) {
      low.angular(i) = min_angle;
      high.angular(i) = min_angle + 2*vnl_math::pi;
    }
  }

  // 1c. create the bounding box
  rsdl_bounding_box box( low, high );
#ifdef DEBUG
  vcl_cout << "Initial bounding box: " << box << vcl_endl;
#endif
  // 2. create the vector of ids
  vcl_vector< int > indices( points_.size() );
  for ( unsigned int i=0; i<indices.size(); ++i ) indices[ i ] = i;

  leaf_count_ = internal_count_ = 0;

  // 3. call recursive function to do the real work
  root_ = build_kd_tree( points_per_leaf, box, 0, indices );
}


bool
first_less( const vcl_pair<double,int>& left,
            const vcl_pair<double,int>& right )
{
  return left.first < right.first;
}


rsdl_kd_node *
rsdl_kd_tree::build_kd_tree( int points_per_leaf,
                             const rsdl_bounding_box& outer_box,
                             int depth,
                             vcl_vector< int >& indices )
{
  assert(points_per_leaf > 0);
  unsigned int i;
#ifdef DEBUG
  vcl_cout << "build_kd_tree:\n  ids:\n";
  for ( i=0; i<indices.size(); ++i )
    vcl_cout << " index: " << indices[i] << ",  point:  " << points_[ indices[i] ] << '\n';
  vcl_cout << "\n  outer bounding box:\n" << outer_box << vcl_endl;
#endif
  // 1. Build the inner box.  This is not done inside the bounding box
  // class because of the indices.

  rsdl_bounding_box inner_box = this -> build_inner_box( indices );
#ifdef DEBUG
  vcl_cout << "\n  inner bounding box:\n" << inner_box << vcl_endl;
#endif

  // 2. If the number of points is small enough, create and return a leaf node.

  if ( indices.size() <= (unsigned int)points_per_leaf ) {
#ifdef DEBUG
    vcl_cout << "making leaf node" << vcl_endl;
#endif
    leaf_count_ ++ ;
    return new rsdl_kd_node ( outer_box, inner_box, depth, indices );
  }

  // 3. Find the dimension along which there is the greatest variation
  // in the points.

  bool use_cartesian;
  int dim;
  this->greatest_variation( indices, use_cartesian, dim );
#ifdef DEBUG
  vcl_cout << "greatest_varation returned  use_cartesian = " << use_cartesian
           << ", dim = " << dim << vcl_endl;
#endif
  // 4. Form and then sort a vector of temporary pairs, containing the
  // indices and the values along the selected dim of the data.
  // Sort by the values.

  vcl_vector< vcl_pair< double, int > > values( indices.size() );
  for ( i=0; i<indices.size(); ++i ) {
    if ( use_cartesian ) {
      values[i] = vcl_pair<double, int> ( points_[ indices[i] ].cartesian( dim ), indices[i] );
    }
    else  {
      values[i] = vcl_pair<double, int> ( points_[ indices[i] ].angular( dim ), indices[i] );
    }
  }
  vcl_sort( values.begin(), values.end(), first_less );
#ifdef DEBUG
  vcl_cout << "\nsorted values/indices:\n";
  for ( i=0; i<values.size(); ++i )
    vcl_cout << i << ":  value = " << values[i].first << ", id = " << values[i].second << vcl_endl;
#endif
  // 5. Partition the vector and the bounding box along the dimension.

  unsigned int med_loc = (indices.size()-1) / 2;
  double median_value = (values[med_loc].first + values[med_loc+1].first) / 2;
#ifdef DEBUG
  vcl_cout << "med_loc = " << med_loc << ", median_value = " << median_value << vcl_endl;
#endif
  rsdl_bounding_box left_outer_box( outer_box ), right_outer_box( outer_box );
  if ( use_cartesian ) {
    left_outer_box.max_cartesian( dim ) = median_value;
    right_outer_box.min_cartesian( dim ) = median_value;
  }
  else {
    left_outer_box.max_angular( dim ) = median_value;
    right_outer_box.min_angular( dim ) = median_value;
  }
#ifdef DEBUG
  vcl_cout << "new bounding boxes:  left " << left_outer_box
           << "\n  right " << right_outer_box << vcl_endl;
#endif
  vcl_vector< int > left_indices( med_loc+1 ), right_indices( indices.size()-med_loc-1 );
  for ( i=0; i<=med_loc; ++i ) left_indices[i] = values[i].second;
  for ( ; i<indices.size(); ++i ) right_indices[i-med_loc-1] = values[i].second;

  rsdl_kd_node * node = new rsdl_kd_node( outer_box, inner_box, depth );
  internal_count_ ++ ;
  node->left_ = this->build_kd_tree( points_per_leaf, left_outer_box, depth+1, left_indices );
  node->right_ = this->build_kd_tree( points_per_leaf, right_outer_box, depth+1, right_indices );

  return node;
}


rsdl_bounding_box
rsdl_kd_tree :: build_inner_box( const vcl_vector< int >& indices )
{
  assert( indices.size() > 0 );
  rsdl_point min_point( points_[ indices[ 0 ] ] );
  rsdl_point max_point( min_point );

  for (unsigned int i=1; i < indices.size(); ++ i ) {
    const rsdl_point& pt = points_[ indices[ i ] ];

    for ( unsigned int j=0; j < Nc_; ++ j ) {
      if ( pt.cartesian( j ) < min_point.cartesian( j ) )
        min_point.cartesian( j ) = pt.cartesian( j );
      if ( pt.cartesian( j ) > max_point.cartesian( j ) )
        max_point.cartesian( j ) = pt.cartesian( j );
    }

    for ( unsigned int j=0; j < Na_; ++ j ) {
      if ( pt.angular( j ) < min_point.angular( j ) )
        min_point.angular( j ) = pt.angular( j );
      if ( pt.angular( j ) > max_point.angular( j ) )
        max_point.angular( j ) = pt.angular( j );
    }
  }
  return rsdl_bounding_box( min_point, max_point );
}


void
rsdl_kd_tree::greatest_variation( const vcl_vector<int>& indices,
                                  bool& use_cartesian,
                                  int& dim )
{
  use_cartesian = true;
  bool initialized = false;
  double interval_size = 0.0;

  // 1. Check the Cartesian dimensions, if they exist

  for ( unsigned int i=0; i<Nc_; ++i ) {
    double min_v, max_v;
    min_v = max_v = points_[ indices[0] ].cartesian( i );
    for ( unsigned int j=1; j<indices.size(); ++j ) {
      double v = points_[ indices[j] ].cartesian( i );
      if ( v < min_v ) min_v = v;
      if ( v > max_v ) max_v = v;
    }
    if ( !initialized || max_v - min_v > interval_size ) {
      dim = i;
      interval_size = max_v - min_v;
      initialized = true;
    }
  }

  // 2. Check the angular dimensions, if they exist

  for ( unsigned int i=0; i<Na_; ++i ) {
    double min_v, max_v;
    min_v = max_v = points_[ indices[0] ].angular( i );
    for (unsigned int j=1; j<indices.size(); ++j ) {
      double v = points_[ indices[j] ].angular( i );
      if ( v < min_v ) min_v = v;
      if ( v > max_v ) max_v = v;
    }
    if ( !initialized || max_v - min_v > interval_size ) {
      dim = i;
      use_cartesian = false;
      interval_size = max_v - min_v;
      initialized = true;
    }
  }

  assert( initialized );
}


rsdl_kd_tree::~rsdl_kd_tree( )
{
  destroy_tree( root_ );
}

void
rsdl_kd_tree::destroy_tree( rsdl_kd_node*&  p )
{
  if ( p ) {
    destroy_tree( p->left_ );
    destroy_tree( p->right_ );
    delete p;
    p = 0;
  }
}


void
rsdl_kd_tree::n_nearest( const rsdl_point& query_point,
                         int n,
                         vcl_vector< rsdl_point >& closest_points,
                         vcl_vector< int >& closest_indices,
                         bool use_heap )
{
  assert(n>0);
  assert( query_point.num_cartesian() == Nc_ );
  assert( query_point.num_angular() == Na_ );

  if ( closest_indices.size() != (unsigned int)n )
    closest_indices.resize( n );
  vcl_vector< double > sq_distances( n );  // could cache for (slight) efficiency gain
  int num_found = 0;

  leaves_examined_ = internal_examined_ = 0;

  if ( use_heap )
    this->n_nearest_with_heap( query_point, n, root_, closest_indices, sq_distances, num_found );
  else
    this->n_nearest_with_stack( query_point, n, root_, closest_indices, sq_distances, num_found );
#ifdef DEBUG
  vcl_cout << "\nAfter n_nearest, leaves_examined_ = " << leaves_examined_
           << ", fraction = " << float(leaves_examined_) / leaf_count_
           << "\n     internal_examined_ = " << internal_examined_
           << ", fraction = " << float(internal_examined_) / internal_count_ << vcl_endl;
#endif
  assert(num_found >= 0);
  if ( closest_points.size() != (unsigned int)num_found )
    closest_points.resize( num_found );

  for ( int i=0; i<num_found; ++i ) {
    closest_points[i] = points_[ closest_indices[i] ];
  }
}


void
rsdl_kd_tree::n_nearest_with_stack( const rsdl_point& query_point,
                                    int n,
                                    rsdl_kd_node* root,
                                    vcl_vector< int >& closest_indices,
                                    vcl_vector< double >& sq_distances,
                                    int & num_found )
{
  assert(n>0);
#ifdef DEBUG
  vcl_cout << "\n\n----------------\nn_nearest_with_stack" << vcl_endl;
#endif
  vcl_vector< rsdl_kd_heap_entry  > stack_vec;
  stack_vec.reserve( 100 );    // way more than will ever be needed
  double left_box_sq_dist, right_box_sq_dist;
  double sq_dist;
  bool initial_path = true;

  //  Go down tree,
  rsdl_kd_node* current = root;
  sq_dist = 0;

  do {
#ifdef DEBUG
    vcl_cout << "\ncurrent -- sq_dist " << sq_dist << ", depth: " << current->depth_
             << "\nouter_box: " << current->outer_box_
             << "\ninner_box: " << current->inner_box_
             << "\nstack size: " << stack_vec.size() << vcl_endl;
#endif
    // if the distance is too large, skip node and take the next node
    // from the stack

    if ( num_found >= n && sq_dist >= sq_distances[ num_found-1 ] ) {
#ifdef DEBUG
      vcl_cout << "skipping node" << vcl_endl;
#endif
      if ( stack_vec.size() == 0 )
        return;  // DONE
      else {
        sq_dist = stack_vec[ stack_vec.size()-1 ].dist_;
        current = stack_vec[ stack_vec.size()-1 ].p_;
        stack_vec.pop_back();
      }
    }

    //  if this is a leaf node, update the set of closest points, and
    //  take the next node from the stack

    else if ( ! current->left_ ) {
#ifdef DEBUG
      vcl_cout << "At a leaf" << vcl_endl;
#endif
      leaves_examined_ ++ ;
      update_closest( query_point, n, current, closest_indices, sq_distances, num_found );

      //  If stack is empty then we're done.
      if ( stack_vec.size() == 0 )
        return;  // done

      //  If we're on the first path down the tree and if we can decide there
      //  is no possibility of any other closer point, then quit.
      else
      {
        if ( initial_path )
        {
#ifdef DEBUG
          vcl_cout << "First leaf" << vcl_endl;
#endif
          initial_path = false ;
          if ( this-> bounded_at_leaf( query_point, n, current, sq_distances, num_found ) )
            return; //  done
        }

        //  Pop the stack as the next location.
        sq_dist = stack_vec[ stack_vec.size()-1 ].dist_;
        current = stack_vec[ stack_vec.size()-1 ].p_;
        stack_vec.pop_back();
      }
    }

    else
    {
#ifdef DEBUG
      vcl_cout << "Internal node" << vcl_endl;
#endif
      internal_examined_ ++ ;
      left_box_sq_dist = rsdl_dist_sq( query_point, current->left_->inner_box_ );
      right_box_sq_dist = rsdl_dist_sq( query_point, current->right_->inner_box_ );
#ifdef DEBUG
      vcl_cout << "left sq distance = " << left_box_sq_dist << vcl_endl
               << "right sq distance = " << right_box_sq_dist << vcl_endl;
#endif

      if ( left_box_sq_dist < right_box_sq_dist )
      {
#ifdef DEBUG
        vcl_cout << "going left, pushing right" << vcl_endl;
#endif
        stack_vec.push_back( rsdl_kd_heap_entry( right_box_sq_dist, current->right_ ) );
        current = current->left_ ;
      }
      else {
#ifdef DEBUG
        vcl_cout << "going right, pushing left" << vcl_endl;
#endif
        stack_vec.push_back( rsdl_kd_heap_entry( left_box_sq_dist, current->left_ ) );
        current = current->right_ ;
      }
    }
  } while ( true );
}


void
rsdl_kd_tree::n_nearest_with_heap( const rsdl_point& query_point,
                                   int n,
                                   rsdl_kd_node* root,
                                   vcl_vector< int >& closest_indices,
                                   vcl_vector< double >& sq_distances,
                                   int & num_found )
{
  assert(n>0);
#ifdef DEBUG
  vcl_cout << "\n\n----------------\nn_nearest_with_heap" << vcl_endl;
#endif
  vcl_vector< rsdl_kd_heap_entry > heap_vec;
  heap_vec.reserve( 100 );
  double left_box_sq_dist, right_box_sq_dist;
  double sq_dist;

  //  Go down tree,
  rsdl_kd_node* current = root;
  while ( current->left_ ) {
    internal_examined_ ++ ;

    if ( rsdl_dist_sq( query_point, current-> left_ -> outer_box_ ) < 1.0e-5 ) {
      right_box_sq_dist = rsdl_dist_sq( query_point, current->right_->inner_box_ );
      heap_vec.push_back( rsdl_kd_heap_entry( right_box_sq_dist, current->right_ ) );
      current = current->left_ ;
    }
    else {
      left_box_sq_dist = rsdl_dist_sq( query_point, current->left_->inner_box_ );
      heap_vec.push_back( rsdl_kd_heap_entry( left_box_sq_dist, current->left_ ) );
      current = current->right_ ;
    }
  }
  vcl_make_heap( heap_vec.begin(), heap_vec.end() );
  sq_dist = 0;

#ifdef DEBUG
  vcl_cout << "\nAfter initial trip down the tree, here's the heap\n";
  for ( int i=0; i<heap_vec.size(); ++i )
    vcl_cout << "  " << i << ":  sq distance " << heap_vec[i].dist_
             << ", node depth " << heap_vec[i].p_->depth_ << vcl_endl;
#endif
  bool first_leaf = true;

  do {
#ifdef DEBUG
    vcl_cout << "\ncurrent -- sq_dist " << sq_dist << ", depth: " << current->depth_
             << "\nouter_box: " << current->outer_box_
             << "\ninner_box: " << current->inner_box_
             << "\nheap size: " << heap_vec.size() << vcl_endl;
#endif
    if ( num_found < n || sq_dist < sq_distances[ num_found-1 ] ) {
      if ( ! current->left_ ) {  // a leaf node
#ifdef DEBUG
        vcl_cout << "Leaf" << vcl_endl;
#endif
        leaves_examined_ ++ ;
        update_closest( query_point, n, current, closest_indices, sq_distances, num_found );
        if ( first_leaf ) {  // check if we can quit just at this leaf node.
#ifdef DEBUG
          vcl_cout << "First leaf" << vcl_endl;
#endif
          first_leaf = false;
          if ( this-> bounded_at_leaf( query_point, n, current, sq_distances, num_found ) )
            return;
        }
      }

      else {
#ifdef DEBUG
        vcl_cout << "Internal" << vcl_endl;
#endif
        internal_examined_ ++ ;

        left_box_sq_dist = rsdl_dist_sq( query_point, current->left_->inner_box_ );
#ifdef DEBUG
        vcl_cout << "left sq distance = " << left_box_sq_dist << vcl_endl;
#endif
        if ( num_found < n || sq_distances[ num_found-1 ] > left_box_sq_dist ) {
#ifdef DEBUG
          vcl_cout << "pushing left onto the heap" << vcl_endl;
#endif
          heap_vec.push_back( rsdl_kd_heap_entry( left_box_sq_dist, current->left_ ) );
          vcl_push_heap( heap_vec.begin(), heap_vec.end() );
        };

        right_box_sq_dist = rsdl_dist_sq( query_point, current->right_->inner_box_ );
#ifdef DEBUG
        vcl_cout << "right sq distance = " << right_box_sq_dist << vcl_endl;
#endif
        if ( num_found < n || sq_distances[ num_found-1 ] > right_box_sq_dist ) {
#ifdef DEBUG
          vcl_cout << "pushing right onto the heap" << vcl_endl;
#endif
          heap_vec.push_back( rsdl_kd_heap_entry( right_box_sq_dist, current->right_ ) );
          vcl_push_heap( heap_vec.begin(), heap_vec.end() );
        }
      }
    }
#ifdef DEBUG
    else vcl_cout << "skipping node" << vcl_endl;
#endif

    if ( heap_vec.size() == 0 )
      return;
    else {
      vcl_pop_heap( heap_vec.begin(), heap_vec.end() );
      sq_dist = heap_vec[ heap_vec.size()-1 ].dist_;
      current = heap_vec[ heap_vec.size()-1 ].p_;
      heap_vec.pop_back();
    }
  } while ( true );
}

void
rsdl_kd_tree::update_closest( const rsdl_point& query_point,
                              int n,
                              rsdl_kd_node* p,
                              vcl_vector< int >& closest_indices,
                              vcl_vector< double >& sq_distances,
                              int & num_found )
{
  assert(n>0);
#ifdef DEBUG
  vcl_cout << "Update_closest for leaf\n query_point = " << query_point
           << "\n inner bounding box =\n" << p->inner_box_
           << "\n sq_dist = " << rsdl_dist_sq( query_point, p->inner_box_ )
           << vcl_endl;
#endif

  for ( unsigned int i=0; i < p->point_indices_.size(); ++i ) {  // check each id
    int id = p->point_indices_[i];
    double sq_dist = rsdl_dist_sq( query_point, points_[ id ] );
#ifdef DEBUG
    vcl_cout << "  id = " << id << ", point = " << points_[ id ]
             << ", sq_dist = " << sq_dist << vcl_endl;
#endif

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
#ifdef DEBUG
  vcl_cout << "  End of leaf computation, num_found =  " << num_found
           << ", and they are:\n";
  for ( int k=0; k<num_found; ++k )
    vcl_cout << "     " << k << ":  indices: " << closest_indices[ k ]
             << ", sq_dist " << sq_distances[ k ] << vcl_endl;
#endif
}


//  If there are already n points (at the first leaf) and if a box
//  drawn around the first point of half-width the distance of the
//  n-th point fits inside the outer box of this node, then no points
//  anywhere else in the tree will can replace any of the closest
//  points.

bool
rsdl_kd_tree :: bounded_at_leaf ( const rsdl_point& query_point,
                                  int n,
                                  rsdl_kd_node* current,
                                  const vcl_vector< double >& sq_distances,
                                  int & num_found )
{
  assert(n>0);
#ifdef DEBUG
  vcl_cout << "bounded_at_leaf\n num_found = " << num_found << vcl_endl;
#endif

  if ( num_found != n ) {
    return false;
  }

  double radius = vcl_sqrt( sq_distances[ n-1 ] );

  for ( unsigned int i = 0; i < Nc_; ++ i ) {
    double x = query_point.cartesian( i );
    if ( current -> outer_box_ . min_cartesian( i ) > x - radius ||
         current -> outer_box_ . max_cartesian( i ) < x + radius ) {
      return false;
    }
  }

  for ( unsigned int i = 0; i < Na_; ++ i ) {
    double a = query_point.angular( i );
    if ( current -> outer_box_ . min_angular( i ) > a - radius ||
         current -> outer_box_ . max_angular( i ) < a + radius ) {
      return false;
    }
  }

  return true;
}


//  Find all points within a query's bounding box
void
rsdl_kd_tree :: points_in_bounding_box( const rsdl_bounding_box& box,
                                        vcl_vector< rsdl_point >& points_in_box,
                                        vcl_vector< int >& indices_in_box )
{
  points_in_box.clear();
  indices_in_box.clear();
  this -> points_in_bounding_box( root_, box, indices_in_box );
  for ( unsigned int i=0; i<indices_in_box.size(); ++i )
    points_in_box.push_back( this -> points_[ indices_in_box[i] ] );
}


void
rsdl_kd_tree :: points_in_radius( const rsdl_point& query_point,
                                  double radius,
                                  vcl_vector< rsdl_point >& points_within_radius,
                                  vcl_vector< int >& indices_within_radius )
{
  //  Form a bounding box of width 2*radius, centered at the point.
  //  Start by creating the corner points of this box.
  rsdl_point min_point( query_point.num_cartesian(), query_point.num_angular() );
  rsdl_point max_point( query_point.num_cartesian(), query_point.num_angular() );

  //  Fill in the cartesian values.
  for ( unsigned int i=0; i < query_point.num_cartesian(); ++i ) {
    min_point.cartesian( i ) = query_point.cartesian(i) - radius;
    max_point.cartesian( i ) = query_point.cartesian(i) + radius;
  }

  //  Fill in the angular values.  If the radius is at least pi then
  //  the whole range of angles will be covered.
  if ( radius >= vnl_math::pi ) {
    for ( unsigned int j=0; j < query_point.num_angular(); ++j ) {
      min_point.angular( j ) = this -> min_angle_;
      max_point.angular( j ) = this -> min_angle_ + 2 * vnl_math::pi;
    }
  }
  //  The radius is less than pi.  For each angular value, generate
  //  the min and max angular values.  Check both for wrap-around.
  else {
    for ( unsigned int j=0; j < query_point.num_angular(); ++j ) {
      double min_angle = query_point.angular(j) - radius;
      if ( min_angle < this -> min_angle_ )
        min_angle += 2 * vnl_math::pi;
      double max_angle = query_point.angular(j) + radius;
      if ( max_angle > this -> min_angle_ + 2 * vnl_math::pi )
        max_angle -= 2 * vnl_math::pi;
      min_point.angular( j ) = min_angle;
      max_point.angular( j ) = max_angle;
    }
  }

  //  Now, create the bounding box, and scratch vectors.
  rsdl_bounding_box box( min_point, max_point );
  vcl_vector< int > indices_in_box;

  //  Gather the points in the bounding box:
  this -> points_in_bounding_box( this -> root_, box, indices_in_box );

  //  Clear out the result vectors in preparation
  points_within_radius.clear();
  indices_within_radius.clear();

  //  Gather the points from the bounding box that are within the radius.
  double sq_radius = radius*radius;
  for ( unsigned int i=0; i < indices_in_box.size(); ++i ) {
    int index = indices_in_box[ i ];
    if ( rsdl_dist_sq( query_point, this -> points_[ index ] ) < sq_radius ) {
      points_within_radius.push_back( this -> points_[ index ] );
      indices_within_radius.push_back( index );
    }
  }
}

void
rsdl_kd_tree :: points_in_bounding_box( rsdl_kd_node* current,
                                        const rsdl_bounding_box& box,
                                        vcl_vector< int >& indices_in_box )
{
  if ( ! current -> left_ ) {
    for ( unsigned int i=0; i < current -> point_indices_ . size(); ++i ) {
      int index = current -> point_indices_[ i ];
      if ( rsdl_dist_point_in_box( this -> points_[ index ], box ) )
        indices_in_box.push_back( index );
    }
  }
  else {
    bool inside, intersects;
    rsdl_dist_box_relation( current -> inner_box_, box, inside, intersects );
    if ( inside )
      this -> report_all_in_subtree( current, indices_in_box );
    else if ( intersects ) {
      this -> points_in_bounding_box( current -> left_, box, indices_in_box );
      this -> points_in_bounding_box( current -> right_, box, indices_in_box );
    }
  }
}

void
rsdl_kd_tree :: report_all_in_subtree( rsdl_kd_node* current,
                                       vcl_vector< int >& indices )
{
  if ( current -> left_ ) {
    this -> report_all_in_subtree( current -> left_, indices );
    this -> report_all_in_subtree( current -> right_, indices );
  }
  else {
    for ( unsigned int i=0; i < current -> point_indices_ . size(); ++ i ) {
      indices.push_back( current -> point_indices_[ i ] );
    }
  }
}
