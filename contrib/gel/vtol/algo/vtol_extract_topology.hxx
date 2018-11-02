#ifndef vtol_extract_topology_hxx_
#define vtol_extract_topology_hxx_

#include <iostream>
#include <iosfwd>
#include "vtol_extract_topology.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/algo/vgl_line_2d_regression.h>

#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_intensity_face.h>

#include <vsol/vsol_curve_2d_sptr.h>

#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_digital_curve.h>

#ifndef NDEBUG
#  define DBG( x ) x;
#else
#  define DBG( x ) /*debugging removed*/ do {} while (false)
#endif

// =============================================================================
//                                                              EXTRACT TOPOLOGY
// =============================================================================

typedef vtol_extract_topology_vertex_node vertex_node;

// ---------------------------------------------------------------------------
//                                              static variables and constants
constexpr unsigned vtol_extract_topology_vertex_node::null_index;
constexpr unsigned vtol_extract_topology_vertex_node::done_index;


// ---------------------------------------------------------------------------
//                                                                 constructor


template< typename T >
vtol_extract_topology<T>::
vtol_extract_topology( label_image_type const& in_image,
                       vtol_extract_topology_params const& in_params )
  : label_img_( in_image ),
    params_( in_params )
{
  compute_label_range();
  construct_topology();
}


// ---------------------------------------------------------------------------
//                                                                    vertices

template< typename T >
std::vector< vtol_vertex_2d_sptr >
vtol_extract_topology<T>::
vertices() const
{
  typedef std::vector< vertex_node >::const_iterator vertex_iterator_t;

  std::vector< vtol_vertex_2d_sptr > verts;

  for ( vertex_iterator_t i = node_list_.begin();
        i != node_list_.end(); ++i ) {
    verts.push_back( i->vertex );
  }

  return verts;
}


// ---------------------------------------------------------------------------
//                                                                       faces

template< typename T >
std::vector< vtol_intensity_face_sptr >
vtol_extract_topology<T>::
faces( data_image_type const& data_img ) const
{
  region_collection region_list;
  collect_regions( region_list );

  // Generate faces for each label. A given label may generate more
  // than one face based on containment, etc.
  //
  std::vector< vtol_intensity_face_sptr > faces;
  for (auto & i : region_list) {
    if ( ! i.empty() ) {
      compute_faces( i, faces, &data_img );
    }
  }

  return faces;
}


template< typename T >
std::vector< vtol_intensity_face_sptr >
vtol_extract_topology<T>::
faces( ) const
{
  region_collection region_list;
  collect_regions( region_list );

  // Generate faces for each label. A given label may generate more
  // than one face based on containment, etc.
  //
  std::vector< vtol_intensity_face_sptr > faces;
  for (auto & i : region_list) {
    if ( ! i.empty() ) {
      compute_faces( i, faces, nullptr );
    }
  }

  return faces;
}


// ---------------------------------------------------------------------------
//                                                                        init

template< typename T >
void
vtol_extract_topology<T>::
compute_label_range()
{
  assert( label_img_.ni() >= 1 && label_img_.nj() >= 1 );

  // Determine the label ranges
  //
  min_label_ = label_img_(0,0);
  max_label_ = min_label_;
  for ( unsigned j = 0; j < label_img_.nj(); ++j ) {
    for ( unsigned i = 0; i < label_img_.ni(); ++i ) {
      if ( min_label_ > label_img_(i,j) )
        min_label_ = label_img_(i,j);
      if ( max_label_ < label_img_(i,j) )
        max_label_ = label_img_(i,j);
    }
  }
}

// ---------------------------------------------------------------------------
//                                                                       label

template< typename LABEL_TYPE >
typename vtol_extract_topology< LABEL_TYPE >::LabelPoint
vtol_extract_topology< LABEL_TYPE >::
label( unsigned i, unsigned j ) const
{
  if ( i < label_img_.ni() && j < label_img_.nj() ) {
    return LabelPoint(label_img_( i, j ), true);
  }
  else {
    return LabelPoint(0, false);
  }
}


// ---------------------------------------------------------------------------
//                                                          is junction vertex

template< typename T >
bool
vtol_extract_topology<T>::
is_junction_vertex( unsigned i, unsigned j ) const
{
  // A junction must have at least three incident boundary edges

  unsigned edge_count = 0;
  for ( unsigned dir = 0; dir < 4; ++dir ) {
    if ( is_edge( i, j, dir ) ) {
      ++edge_count;
    }
  }

  return edge_count >= 3;
}


// ---------------------------------------------------------------------------
//                                                          is boundary vertex

template< typename LABEL_TYPE >
bool
vtol_extract_topology< LABEL_TYPE >::
is_boundary_vertex( unsigned i, unsigned j ) const
{
  // A non-boundary (interior) point is surrounded by pixels of the
  // same value.

  LabelPoint pixel1( label( i  , j   ) );
  LabelPoint pixel2( label( i  , j-1 ) );
  LabelPoint pixel3( label( i-1, j   ) );
  LabelPoint pixel4( label( i-1, j-1 ) );

  return pixel1 != pixel2 || pixel2 != pixel3 || pixel3 != pixel4;
}


// ---------------------------------------------------------------------------
//                                                                     is edge

template< typename LABEL_TYPE >
bool
vtol_extract_topology< LABEL_TYPE >::
is_edge( unsigned i, unsigned j, unsigned dir ) const
{
  LabelPoint left, right;

  edge_labels( i, j, dir, left, right );

  return left != right;
}


// ---------------------------------------------------------------------------
//                                                                 edge labels

template< typename LABEL_TYPE >
void
vtol_extract_topology< LABEL_TYPE >::
edge_labels( unsigned i, unsigned j, unsigned dir,
             LabelPoint& left, LabelPoint& right ) const
{
  assert( dir <= 3 );

  // These are the offsets to get the "left" pixel position for each
  // direction given the vertex location (i,j).  The vertices occur at
  // the corners of the pixels, so that vertex (0,0) is at the
  // top-left corner of pixel (0,0).
  //
  // The offsets for the "right" pixel in direction d is the offset
  // for the left pixel in direction (d+1).
  //
  static const int offsets[4][2] =
    { { 0, -1 }, { 0, 0 }, { -1, 0 }, { -1, -1 } };

  unsigned dir2 = (dir+1) % 4;

  left  = label( i+offsets[dir ][0], j+offsets[dir ][1] );
  right = label( i+offsets[dir2][0], j+offsets[dir2][1] );
}


// ---------------------------------------------------------------------------
//                                                                vertex index

template< typename T >
unsigned
vtol_extract_topology<T>::
vertex_index( unsigned i, unsigned j ) const
{
  assert( i < index_img_.ni() &&
          j < index_img_.nj() );

  return index_img_( i, j );
}


// ---------------------------------------------------------------------------
//                                                            set vertex index

template< typename T >
void
vtol_extract_topology<T>::
set_vertex_index( unsigned i, unsigned j, unsigned index )
{
  assert( i < index_img_.ni() &&
          j < index_img_.nj() );

  index_img_( i, j ) = index;
}


// ---------------------------------------------------------------------------
//                                                                        node

template< typename T >
vtol_extract_topology_vertex_node&
vtol_extract_topology<T>::
node( unsigned index )
{
  assert( index < node_list_.size() );

  return node_list_[index];
}


template< typename T >
vtol_extract_topology_vertex_node const&
vtol_extract_topology<T>::
node( unsigned index ) const
{
  assert( index < node_list_.size() );

  return node_list_[index];
}


// ---------------------------------------------------------------------------
//                                                                        move

template< typename T >
void
vtol_extract_topology<T>::
move( unsigned dir, unsigned& i, unsigned& j )
{
  assert( dir < 4 );

  switch ( dir )
  {
   case 0: // right
    ++i;
    break;
   case 1: // down
    ++j;
    break;
   case 2: // left
    --i;
    break;
   case 3: // up
    --j;
    break;
   default: break; // never reached
  }
}


// ---------------------------------------------------------------------------
//                                                                    set mark

template< typename T >
void
vtol_extract_topology<T>::
set_mark( unsigned& marker, unsigned dir ) const
{
  marker |= ( 1 << dir );
}


// ---------------------------------------------------------------------------
//                                                                   is marked

template< typename T >
bool
vtol_extract_topology<T>::
is_marked( unsigned marker, unsigned dir ) const
{
  return ( marker & ( 1 << dir ) ) != 0;
}


// ---------------------------------------------------------------------------
//                                                            trace edge chain

template< typename T >
void
vtol_extract_topology<T>::
trace_edge_chain( unsigned i, unsigned j, unsigned dir )
{
  // Quick exit if there is nothing to trace in this direction.
  //
  if ( ! is_edge( i, j, dir ) )
    return;


  // When constructing the spatial and topological objects from the
  // "pixel corner" vertices, we add the (-.5,-.5) offset required to
  // bring everything into pixel coordinates.

  unsigned start_index = vertex_index( i, j );
  unsigned start_dir = dir;

  // The chain of "pixel corners" from one vertex to the other.
  //
  vdgl_edgel_chain_sptr chain = new vdgl_edgel_chain();

  // Start vertex
  chain->add_edgel( vdgl_edgel( i-0.5, j-0.5, -1, dir*90 ) );
  move( dir, i, j );

  // If we have a complete cycle (i.e. start==end), we have to step
  // back one because vtol doesn't like edges with the same start and
  // end. When we step back, we need to keep track of the direction
  // from which we came to that point. We'll only ever need to step
  // back once, so it is sufficient to keep the current direction
  // (direction of the last step) in dir) and the previous direction
  // in prev_dir.
  unsigned int prev_dir = (unsigned int)(-1);

  while ( vertex_index( i, j ) == vertex_node::null_index )
  {
    set_vertex_index( i, j, vertex_node::done_index );

    chain->add_edgel( vdgl_edgel( i-0.5, j-0.5, -1, dir*90 ) );

    // find and move in the outgoing direction. There should be
    // exactly one, since this is not a vertex.
    //
    DBG( unsigned count = 0 );
    prev_dir = dir;
    dir = (dir+3) % 4; // same as dir = dir - 1
    while ( ! is_edge( i, j, dir ) ) {
      dir = (dir+1) % 4;
      DBG( ++count );
      DBG( assert( count < 3 ) );
    }

    move( dir, i, j );

    // The same non-junction vertex should not appear on multiple
    // traces. (The "reverse trace" is will be done below by just
    // reversing the start and end.)
    //
    assert( vertex_index( i, j ) != vertex_node::done_index );
  }

  unsigned end_index = vertex_index( i, j );

  if ( end_index == start_index ) {
    // Construct a new vertex at the just-before-last point to avoid
    // having a chain with identical end-points. Move backwards one
    // unit and create a vertex.
    move( (dir+2)%4, i, j );
    set_vertex_index( i, j, node_list_.size() );
    node_list_.push_back( vertex_node( i, j ) );
    end_index = vertex_index( i, j );
    dir = prev_dir; // the direction we came from to the new end point.
    // The new end point is already in the edgel chain; no need to add again.
  }
  else {
    // Add the end vertex
    chain->add_edgel( vdgl_edgel( i-0.5, j-0.5 ) );
  }

  chain = smooth_chain( chain, params_.num_for_smooth );

  vertex_node& start_node = node( start_index );
  vertex_node& end_node   = node( end_index );

  vdgl_interpolator_sptr interp = new vdgl_interpolator_linear( chain );
  vsol_curve_2d_sptr curve = new vdgl_digital_curve( interp );
  vtol_edge_2d_sptr edge = new vtol_edge_2d( start_node.vertex,
                                             end_node.vertex,
                                             curve );

  edgel_chain_sptr chain2 = new edgel_chain;
  chain2->chain = chain;
  chain2->edge  = edge;

  // Turn around, because the that is the direction we would begin in
  // from the end node for a reverse trace.
  //
  dir = (dir+2) % 4;

  start_node.link[ start_dir ] = end_index;
  start_node.back_dir[ start_dir ] = dir;
  start_node.edgel_chain[ start_dir ] = chain2;

  end_node.link[ dir ] = start_index;
  end_node.back_dir[ dir ] = start_dir;
  end_node.edgel_chain[ dir ] = chain2;
}


// ---------------------------------------------------------------------------
//                                                          construct topology

template< typename T >
void
vtol_extract_topology<T>::
construct_topology( )
{
  // Construct the list of vertex nodes from the junctions and
  // initialize the vertex index array.
  //
  index_img_.set_size( label_img_.ni()+1, label_img_.nj()+1 );

  node_list_.clear();
  index_img_.fill( vertex_node::null_index );

  for ( unsigned j = 0; j <= label_img_.nj(); ++j ) {
    for ( unsigned i = 0; i <= label_img_.ni(); ++i ) {
      if ( is_junction_vertex( i, j ) ) {
        set_vertex_index( i, j, node_list_.size() );
        node_list_.push_back( vertex_node( i, j ) );
      }
    }
  }

  // Construct the edge graph by following each edge from
  // each vertex.
  //
  for ( unsigned index = 0; index < node_list_.size(); ++index ) {
    for ( unsigned dir = 0; dir < 4; ++dir ) {
      if ( node(index).link[dir] == vertex_node::null_index ) {
        trace_edge_chain( node(index).i,
                          node(index).j,
                          dir );
      }
    }
  }

  // Look for untouched boundary vertices, and make them vertices
  // too. Tracing from one of these must lead back to itself. The
  // topology classes don't like edges that are themselves
  // cycles. They expect that each edge has two distinct
  // vertices. Instead of creating two vertices at the same point, we
  // create two vertices adjacent to each other. Thus, each of these
  // faces (created later) will be bounded by two edges, one
  // potentially long edge, and one edge with length one pixel.
  //
  for ( unsigned j = 0; j <= label_img_.nj(); ++j ) {
    for ( unsigned i = 0; i <= label_img_.ni(); ++i ) {
      if ( vertex_index( i, j ) == vertex_node::null_index &&
           is_boundary_vertex( i, j ) )
      {
        // Find the two outgoing directions
        //
        unsigned dir = 0;
        while ( dir < 4 && ! is_edge( i, j, dir ) ) {
          ++dir;
        }
        assert( dir < 4 );
        unsigned dir2 = dir+1;
        while ( dir2 < 4 && ! is_edge( i, j, dir2 ) ) {
          ++dir2;
        }
        assert( dir2 < 4 );

        // Create a vertex at this point
        //
        set_vertex_index( i, j, node_list_.size() );
        node_list_.push_back( vertex_node( i, j ) );

        // Create a vertex at a neighbour
        //
        unsigned i2 = i, j2 = j;
        move( dir, i2, j2 );
        assert( vertex_index( i2, j2 ) == vertex_node::null_index &&
                is_boundary_vertex( i2, j2 ) );
        set_vertex_index( i2, j2, node_list_.size() );
        node_list_.push_back( vertex_node( i2, j2 ) );

        // Trace from here, going both ways
        //
        trace_edge_chain( i, j, dir );
        trace_edge_chain( i, j, dir2 );
      }
    }
  }

#ifndef NDEBUG
  // Verify the integrity of the vertex graph
  bool good = true;
  for ( unsigned index = 0; index < node_list_.size(); ++index ) {
    for ( unsigned dir = 0; dir < 4; ++dir ) {
      if ( node(index).link[dir] != vertex_node::null_index ) {
        unsigned nbr = node(index).link[dir];
        unsigned back_dir = node(index).back_dir[dir];
        if ( node(nbr).link[back_dir] != index ) {
          std::cerr << "Bad back link on vertex " << index << " ("<<node(index).i
                   << ',' << node(index).j << " in dir " << dir << '\n'
                   << "  link     " << dir << " = " << node(index).link[dir] << ";\n"
                   << "  back_dir " << dir << " = " << node(index).back_dir[dir] << '\n';
        }
      }
    }
    assert( good );
  }
#endif
}


// ---------------------------------------------------------------------------
//                                                         trace face boundary

template< typename LABEL_TYPE >
bool
vtol_extract_topology< LABEL_TYPE >::
trace_face_boundary( std::vector<unsigned>& markers,
                     unsigned index,
                     unsigned dir,
                     region_type& chain_list,
                     LabelPoint& region_label ) const
{
  unsigned start_index = index;
  LabelPoint start_left;
  edge_labels( node(index).i, node(index).j, dir,
               start_left, region_label );

#ifdef DEBUG
  std::cout << "start left, region label: " << (int) start_left << ' '
           << (int)region_label << " ; i,j: " << node(index).i << ' '
           << node(index).j << " ; index,dir " << index << ' '
           << dir << " ; label(i,j) = "
           << (int)label_img_(node(index).i, node(index).j) << std::endl;
  if ( region_label + 1 == min_label_ ) {
    std::cout << "exiting" << std::endl;
    return false;
  }
#endif
  if (!region_label.valid) {
    return false;
  }

  // Find an interior pixel of this face based on the first edge we
  // encounter. The vertex (i,j) corresponds to pixel-coordinate-space
  // (i-0.5,j-0.5). Use this and the direction to get a point on the
  // right of the first edge. The delta_* store the appropriate
  // offsets for each direction.
  //
  static const int delta_i[4] = {  0, -1, -1,  0 };
  static const int delta_j[4] = {  0,  0, -1, -1 };
  chain_list.i = node(index).i + delta_i[dir];
  chain_list.j = node(index).j + delta_j[dir];

#ifdef DEBUG
  std::cout << "index " << index << " dir " << dir << "  node " << node(index).i
           << " delta " << delta_i[dir] << std::endl;
#endif
  assert( chain_list.i < label_img_.ni() );
  assert( chain_list.j < label_img_.nj() );

  do {
    // Mark the current direction of the current node as travelled,
    // and go to the next node and find the outgoing direction there.

    assert( ! is_marked( markers[index], dir ) );
    set_mark( markers[index], dir );
    chain_list.push_back( node(index).edgel_chain[dir] );

    unsigned back_dir = node(index).back_dir[ dir ];
    index = node(index).link[ dir ];
    assert( index != vertex_node::null_index );

    // Look from right to left, so that we are more conservative at
    // corner touches. That is, we will take
    //
    //     +----+
    //     |    |
    //     +----+----+
    //          |    |
    //          +----+
    //
    // as two regions instead of one figure-8 region.
    //
    // We keep the object on the right, which actually corresponds to
    // the standard "keep the object on the left" in a right-handed
    // coordinate system.
    //
    unsigned i = node(index).i;
    unsigned j = node(index).j;
    LabelPoint left, right;
    dir = back_dir;
    DBG( unsigned old_dir = dir % 4 );
    do {
      dir = (dir+3) % 4; // same as dir = dir - 1

      // Make sure we haven't done a full cycle.
      DBG( assert( dir != old_dir ) );

      edge_labels( i, j, dir, left, right );
    } while ( left == right || right != region_label );

  } while ( index != start_index );

  return true;
}


// ---------------------------------------------------------------------------
//                                                             collect regions

template< typename LABEL_TYPE >
void
vtol_extract_topology< LABEL_TYPE >::
collect_regions( region_collection& region_list ) const
{
  // Use put marks about which nodes (vertices) and directions have
  // been processed.
  //
  std::vector< unsigned > markers( node_list_.size(), 0 );

  region_list.resize( max_label_ - min_label_ + 1 );

  // Process each vertex, generating the boundary chains
  //
  for ( unsigned i = 0; i < node_list_.size(); ++i ) {
    for ( unsigned dir = 0; dir < 4; ++dir ) {
      if ( ! is_marked( markers[i], dir ) &&
           node(i).link[dir] != vertex_node::null_index ) {
        region_type_sptr chain = new region_type;
        LabelPoint label;
        if ( trace_face_boundary( markers, i, dir, *chain, label ) ) {
          assert( (label.valid) && (label.label >= min_label_) );
          region_list[ label.label - min_label_ ].push_back( chain );
        }
      }
    }
  }

#ifndef NDEBUG
  // After we extract all the chains, we must have gone through every
  // vertex in every available direction except for the
  // counter-clockwise chain at the boundary (which is the border of
  // the "outside" region).
  //
  for ( unsigned i = 0; i < node_list_.size(); ++i ) {
    for ( unsigned dir = 0; dir < 4; ++dir ) {
      assert( node(i).link[dir] == vertex_node::null_index ||
              ( dir==0 && node(i).j==label_img_.nj() ) ||
              ( dir==1 && node(i).i==0 ) ||
              ( dir==2 && node(i).j==0 ) ||
              ( dir==3 && node(i).i==label_img_.ni() ) ||
              is_marked( markers[i], dir ) );
    }
  }
#endif
}


// ---------------------------------------------------------------------------
//                                                               compute faces

template< typename T >
void
vtol_extract_topology<T>::
compute_faces( std::vector< region_type_sptr > const& chains,
               std::vector< vtol_intensity_face_sptr >& faces,
               data_image_type const* data_img ) const
{
  assert( chains.size() > 0 );

  // Determine the containment tree by repeatedly adding the chains
  // into a tree. The adding process makes sure that the chain falls
  // into the appropriate place in the containment hierarchy.

  chain_tree_node universe( nullptr );
  for (const auto & chain : chains) {
    universe.add( chain );
  }

  // If we have a data image, use it to add digital region information
  // to the face
  //
  if ( data_img ) {
    finder_type* finder = new finder_type( label_img_, vil_region_finder_4_conn );
    add_faces( faces, finder, data_img, &universe );
    delete finder;
  }
  else {
    add_faces( faces, nullptr, nullptr, &universe );
  }
}

// ---------------------------------------------------------------------------
//                                                                   add faces

template <typename T>
void
vtol_extract_topology<T>::
add_faces( std::vector<vtol_intensity_face_sptr>& faces,
           typename vtol_extract_topology<T>::finder_type* find,
           data_image_type const* img,
           typename vtol_extract_topology<T>::chain_tree_node* node,
           bool even_level ) const
{
  if ( even_level ) {
    faces.push_back( node->make_face( find, img ) );
  }
  for ( unsigned i = 0; i < node->children.size(); ++i ) {
    add_faces( faces, find, img, node->children[i], !even_level );
  }
}

// ---------------------------------------------------------------------------
//                                                                    contains


template <typename T>
bool
vtol_extract_topology<T>::
contains( region_type_sptr a, region_type_sptr b )
{
  assert( b );
  if ( !a ) {
    return true;
  }
  else {
    // Odd number of crossings => inside.
    //
    unsigned num_crossings = 0;
    for ( unsigned i = 0; i < a->size(); ++i ) {
      num_crossings += num_crosses_x_pos_ray( b->i, b->j, *(*a)[i] );
    }
    return ( num_crossings % 2 ) == 1;
  }
}


// ---------------------------------------------------------------------------
//                                                       num crosses x pos ray

template <typename T>
unsigned
vtol_extract_topology<T>::
num_crosses_x_pos_ray( double x, double y, vdgl_edgel_chain const& chain )
{
  if ( chain.size() < 2 )
    return 0;

  // The edges are vertical or horizontal line segments. Leverage that
  // in doing the intersection check. Assume that the ray will *not*
  // cross on a vertex.
  //
  unsigned count = 0;
  for ( unsigned int i = 0; i+1 < chain.size(); ++i ) {
    vdgl_edgel const* e0 = &chain[i];
    vdgl_edgel const* e1 = &chain[i+1];
    assert( e0->y() != y && e1->y() != y );
    if ( ( e0->y() < y && y < e1->y() ) ||
        ( e1->y() < y && y < e0->y() ) ) {
      assert( e0->x() == e1->x() );
      assert( e0->x() != x );
      if ( e0->x() > x ) {
        ++count;
      }
    }
  }

  return count;
}


// ---------------------------------------------------------------------------
//                                                                smooth chain

template <typename T>
vdgl_edgel_chain_sptr
vtol_extract_topology<T>::
smooth_chain( vdgl_edgel_chain_sptr chain,
              unsigned int num_pts ) const
{
  // Can't smooth over more points than we have
  if ( num_pts > chain->size() ) {
    num_pts = chain->size();
  }

  // Need at least two points to fit a line
  if ( num_pts < 2)
    return chain;

  vdgl_edgel_chain_sptr new_chain = new vdgl_edgel_chain;

  vgl_line_2d_regression<double> reg;

  // These store the indices of the edgel points used to estimate the
  // line. The points in [fit_start,fit_end) are used.
  //
  unsigned fit_start;
  unsigned fit_end;

  // This is the index of the edgel point we are smoothing.
  //
  unsigned curr_ind = 0;

  vgl_point_2d<double> pt1, pt2;
  vgl_vector_2d<double> dir;
  double slope;

  // Add the first few points to get the first line. We'll use this
  // line as the smoothing estimate for the first few edgel pixels. We
  // don't add the first point, because we will constrain the line to
  // pass through it. This will make sure that the vertices don't move
  // because of the smoothing.
  //
  for ( fit_end = 1; fit_end < num_pts; ++fit_end ) {
    reg.increment_partial_sums( chain->edgel(fit_end).x(),
                                chain->edgel(fit_end).y() );
  }
  assert( reg.get_n_pts() + 1 == num_pts );
  if ( !reg.fit_constrained( chain->edgel(0).x(), chain->edgel(0).y() ) ) {
    std::cerr << "line fit failed at start\n";
  }

  // Project the first half of the points used in estimating the line
  // onto the line to get the smoothed positions.
  //
  reg.get_line().get_two_points( pt1, pt2 );
  dir = reg.get_line().direction();
  slope = reg.get_line().slope_degrees();
  for ( ; curr_ind < (fit_end+1)/2; ++curr_ind ) {
    pt2.set( chain->edgel(curr_ind).x(), chain->edgel(curr_ind).y() );
    pt2 = pt1 + dot_product( pt2-pt1, dir ) * dir;
    new_chain->add_edgel( vdgl_edgel( pt2.x(), pt2.y(), -1, slope ) );
  }

  // We have all the points from [1,fit_end_] in the regression object.
  //
  fit_start = 1;

  while ( fit_end + 1 < chain->size() ) {
    // Add one more point to get num_pts points
    //
    reg.increment_partial_sums( chain->edgel(fit_end).x(),
                                chain->edgel(fit_end).y() );
    ++fit_end;

    assert( reg.get_n_pts() == num_pts );

    // Estimate a new line
    //
    if ( !reg.fit() ) {
      std::cerr << "line fit failed at " << fit_start << '-' << fit_end << '\n';
    }

    // Project the current point onto the line to get the smoothed position
    //
    reg.get_line().get_two_points( pt1, pt2 );
    dir = reg.get_line().direction();
    slope = reg.get_line().slope_degrees();
    pt2.set( chain->edgel(curr_ind).x(), chain->edgel(curr_ind).y() );
    pt2 = pt1 + dot_product( pt2-pt1, dir ) * dir;
    new_chain->add_edgel( vdgl_edgel( pt2.x(), pt2.y(), -1, slope ) );
    ++curr_ind;

    // Remove the start point in preparation for the next line segment
    //
    reg.decrement_partial_sums( chain->edgel(fit_start).x(),
                                chain->edgel(fit_start).y() );
    ++fit_start;
  }

  assert( reg.get_n_pts() + 1 == num_pts );

  // The special case when we are using all the points to fit a line,
  // the end point is already in the regression object when it
  // normally wouldn't be. We have to replace it with the starting
  // point (which is not in the regression object) so we can do a
  // constrained fit.
  //
  if ( num_pts == chain->size() ) {
    assert( fit_end == chain->size() );
    assert( fit_start == 1 );
    --fit_end;
    reg.decrement_partial_sums( chain->edgel(fit_end).x(),
                                chain->edgel(fit_end).y() );
    reg.increment_partial_sums( chain->edgel(0).x(),
                                chain->edgel(0).y() );
  }

  // We have num_pts-1 points in the regression object. We do a
  // constrained fit to make sure we interpolate the end vertex, and
  // use this line for the final few edgel points.
  //
  if ( !reg.fit_constrained( chain->edgel(fit_end).x(),
                            chain->edgel(fit_end).y() ) ) {
    std::cerr << "line fit failed at end\n";
  }

  dir = reg.get_line().direction();
  reg.get_line().get_two_points( pt1, pt2 );
  slope = reg.get_line().slope_degrees();
  for ( ; curr_ind <= fit_end; ++curr_ind ) {
    pt2.set( chain->edgel(curr_ind).x(), chain->edgel(curr_ind).y() );
    pt2 = pt1 + dot_product( pt2-pt1, dir )*dir;
    new_chain->add_edgel( vdgl_edgel( pt2.x(), pt2.y(), -1, slope ) );
  }

  return new_chain;
}

#endif // vtol_extract_topology_hxx_
