// This is gel/vtol/algo/vtol_extract_topology.cxx
#include "vtol_extract_topology.h"
//:
// \file
// \author Amitha Perera
// \date   July 2003

#include <vcl_iosfwd.h>
#include <vcl_cassert.h>

#include <vil/algo/vil_region_finder.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/algo/vgl_line_2d_regression.h>

#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_intensity_face.h>

#include <vsol/vsol_curve_2d_sptr.h>

#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_digital_region.h>

#ifndef NDEBUG
#  include <vcl_iostream.h>
#  define DEBUG( x ) x;
#else
#  define DEBUG( x ) /*debugging removed*/ do {} while (0)
#endif


// =============================================================================
//                                   declaration of HELPER CLASSES AND FUNCTIONS
// =============================================================================

// The internal classes are defined in a separate file because we need
// to instantiate smart pointers to them. We cannot include
// vbl_smart_ptr.txx in this file because that may cause the compiler
// to try to instantiate smart pointers of incomplete types (like
// vsol_point_2d). (Borland 55 does this.) The real problem is in the
// constructor of vertex_node, defined below. Refer to the comments
// there.
//
#include "vtol_extract_topology_internals.h"

namespace {

// For convenience
typedef vtol_extract_topology::region_type_sptr region_type_sptr;
typedef vtol_extract_topology::data_image_type data_image_type;
typedef vil_region_finder< vtol_extract_topology::data_pixel_type > finder_type;

// Holds the tree describing the containment structure for a set of
// chains bounding regions with the same label.
//
struct chain_tree_node
{
  // The region for the current node. Can be null only for the root
  // node. The root node represents the universe. All regions are
  // contained in the universe.
  //
  region_type_sptr region;

  // The regions from all child nodes are spatially contained in the
  // region for this node.
  //
  vcl_vector<chain_tree_node*> children;

  chain_tree_node( region_type_sptr region );

  ~chain_tree_node();

  // Add a new region below this node. Prerequiste: the new region is
  // contained within this chain.
  //
  void
  add( region_type_sptr new_region );

  // Create a face from the regions at this node and its children.
  //
  vtol_intensity_face_sptr
  make_face( finder_type* find, data_image_type const* img ) const;

  void
  print( vcl_ostream& ostr, unsigned indent ) const;
};

// Returns true if the region bounded by a contains the region bounded
// by b.  Assumes that (1) the chains are cycles and thus bound a
// region, (2) that a containment relationship holds (i.e. no partial
// overlap). A special case allows for a "universe": if a is null,
// then the function will return true. Currently, b cannot be null.
//
bool
contains( region_type_sptr a, region_type_sptr b );

// Computes the number of times that a ray in the positive x direction
// originating from (x,y) intersects the edgel chain.
//
// The implementation assumes that (1) the neighbouring edgels form
// vertical or horizontal lines only, and (2) the ray does not go
// through a vertex (i.e. \a y is not equal to the y-coordinate of any
// edgel).
//
unsigned
num_crosses_x_pos_ray( double x, double y, vdgl_edgel_chain const& chain );

// Adds the faces contained in the tree rooted at node. Essentially,
// it will create a face from each node at an even depth. (The root
// node, the universe, is at an odd depth.) At an even depth, the
// chain for the node represents the outer boundary while the chains
// of the children represent inner boundaries. (Grandchildren
// represent the outer boundaries of smaller faces.)
//
void
add_faces( vcl_vector<vtol_intensity_face_sptr>& faces,
           finder_type* find,
           data_image_type const* img,
           chain_tree_node* node,
           bool even_level = false );

// Smoothes an edgel chain by fitting a line to the local
// neighbourhood and projecting onto that line
//
vdgl_edgel_chain_sptr
smooth_chain( vdgl_edgel_chain_sptr chain,
              unsigned int num_pts );

} // end anonymous namespace


// =============================================================================
//                                                              EXTRACT TOPOLOGY
// =============================================================================


// ---------------------------------------------------------------------------
//                                              static variables and constants


#if !VCL_STATIC_CONST_INIT_INT_NO_DEFN
const unsigned
vtol_extract_topology::null_index  VCL_STATIC_CONST_INIT_INT_DEFN( unsigned(-2) );

const unsigned
vtol_extract_topology::done_index  VCL_STATIC_CONST_INIT_INT_DEFN( unsigned(-1) );
#endif


// ---------------------------------------------------------------------------
//                                                                 constructor

vtol_extract_topology::
vtol_extract_topology( label_image_type const& in_image,
                       vtol_extract_topology_params const& in_params )
  : img_( in_image ),
    params_( in_params )
{
  compute_label_range();
  construct_topology();
}


// ---------------------------------------------------------------------------
//                                                                    vertices

vcl_vector< vtol_vertex_2d_sptr >
vtol_extract_topology::
vertices() const
{
  typedef vcl_vector< vertex_node >::const_iterator vertex_iterator_t;

  vcl_vector< vtol_vertex_2d_sptr > verts;

  for ( vertex_iterator_t i = node_list_.begin();
        i != node_list_.end(); ++i ) {
    verts.push_back( i->vertex );
  }

  return verts;
}


// ---------------------------------------------------------------------------
//                                                                       faces

vcl_vector< vtol_intensity_face_sptr >
vtol_extract_topology::
faces( data_image_type const& data_img ) const
{
  region_collection region_list;
  collect_regions( region_list );

  // Generate faces for each label. A given label may generate more
  // than one face based on containment, etc.
  //
  vcl_vector< vtol_intensity_face_sptr > faces;
  for ( unsigned i = 0; i < region_list.size(); ++i ) {
    if ( ! region_list[i].empty() ) {
      compute_faces( region_list[i], faces, &data_img );
    }
  }

  return faces;
}


vcl_vector< vtol_intensity_face_sptr >
vtol_extract_topology::
faces( ) const
{
  region_collection region_list;
  collect_regions( region_list );

  // Generate faces for each label. A given label may generate more
  // than one face based on containment, etc.
  //
  vcl_vector< vtol_intensity_face_sptr > faces;
  for ( unsigned i = 0; i < region_list.size(); ++i ) {
    if ( ! region_list[i].empty() ) {
      compute_faces( region_list[i], faces, 0 );
    }
  }

  return faces;
}


// ---------------------------------------------------------------------------
//                                                                        init

void
vtol_extract_topology::
compute_label_range()
{
  assert( img_.ni() >= 1 && img_.nj() >= 1 );

  // Determine the label ranges
  //
  min_label_ = img_(0,0);
  max_label_ = min_label_;
  for ( unsigned j = 0; j < img_.nj(); ++j ) {
    for ( unsigned i = 0; i < img_.ni(); ++i ) {
      if ( min_label_ > img_(i,j) )
        min_label_ = img_(i,j);
      if ( max_label_ < img_(i,j) )
        max_label_ = img_(i,j);
    }
  }
}

// ---------------------------------------------------------------------------
//                                                                       label

int
vtol_extract_topology::
label( unsigned i, unsigned j ) const
{
  if ( i < img_.ni() && j < img_.nj() ) {
    return img_( i, j );
  } else {
    return min_label_ - 1;
  }
}


// ---------------------------------------------------------------------------
//                                                          is junction vertex

bool
vtol_extract_topology::
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

bool
vtol_extract_topology::
is_boundary_vertex( unsigned i, unsigned j ) const
{
  // A non-boundary (interior) point is surrounded by pixels of the
  // same value.

  int pixel1 = label( i  , j   );
  int pixel2 = label( i  , j-1 );
  int pixel3 = label( i-1, j   );
  int pixel4 = label( i-1, j-1 );

  return pixel1 != pixel2 || pixel2 != pixel3 || pixel3 != pixel4;
}


// ---------------------------------------------------------------------------
//                                                                     is edge

bool
vtol_extract_topology::
is_edge( unsigned i, unsigned j, unsigned dir ) const
{
  int left, right;

  edge_labels( i, j, dir, left, right );

  return left != right;
}


// ---------------------------------------------------------------------------
//                                                                 edge labels

void
vtol_extract_topology::
edge_labels( unsigned i, unsigned j, unsigned dir,
             int& left, int& right ) const
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

unsigned
vtol_extract_topology::
vertex_index( unsigned i, unsigned j ) const
{
  assert( i < index_img_.ni() &&
          j < index_img_.nj() );

  return index_img_( i, j );
}


// ---------------------------------------------------------------------------
//                                                            set vertex index

void
vtol_extract_topology::
set_vertex_index( unsigned i, unsigned j, unsigned index )
{
  assert( i < index_img_.ni() &&
          j < index_img_.nj() );

  index_img_( i, j ) = index;
}


// ---------------------------------------------------------------------------
//                                                                        node

vtol_extract_topology::vertex_node&
vtol_extract_topology::
node( unsigned index )
{
  assert( index < node_list_.size() );

  return node_list_[index];
}


vtol_extract_topology::vertex_node const&
vtol_extract_topology::
node( unsigned index ) const
{
  assert( index < node_list_.size() );

  return node_list_[index];
}


// ---------------------------------------------------------------------------
//                                                                        move

void
vtol_extract_topology::
move( unsigned dir, unsigned& i, unsigned& j )
{
  assert( dir < 4 );

  switch( dir ) {
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
  }
}


// ---------------------------------------------------------------------------
//                                                                    set mark

void
vtol_extract_topology::
set_mark( unsigned& marker, unsigned dir ) const
{
  marker |= ( 1 << dir );
}


// ---------------------------------------------------------------------------
//                                                                   is marked

bool
vtol_extract_topology::
is_marked( unsigned marker, unsigned dir ) const
{
  return ( marker & ( 1 << dir ) ) != 0;
}


// ---------------------------------------------------------------------------
//                                                            trace edge chain

void
vtol_extract_topology::
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

  while ( vertex_index( i, j ) == null_index )
  {
    set_vertex_index( i, j, done_index );

    chain->add_edgel( vdgl_edgel( i-0.5, j-0.5, -1, dir*90 ) );

    // find and move in the outgoing direction. There should be
    // exactly one, since this is not a vertex.
    //
    DEBUG( unsigned count = 0 );
    dir = (dir+3) % 4; // same as dir = dir - 1
    while ( ! is_edge( i, j, dir ) ) {
      dir = (dir+1) % 4;
      DEBUG( ++count );
      DEBUG( assert( count < 3 ) );
    }

    move( dir, i, j );

    // The same non-junction vertex should not appear on multiple
    // traces.
    //
    assert( vertex_index( i, j ) != done_index );
  }

  // End vertex
  chain->add_edgel( vdgl_edgel( i-0.5, j-0.5 ) );

  chain = smooth_chain( chain, params_.num_for_smooth );

  unsigned end_index = vertex_index( i, j );

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

void
vtol_extract_topology::
construct_topology( )
{
  // Construct the list of vertex nodes from the junctions and
  // initialize the vertex index array.
  //
  index_img_.set_size( img_.ni()+1, img_.nj()+1 );

  node_list_.clear();
  index_img_.fill( null_index );

  for ( unsigned j = 0; j <= img_.nj(); ++j ) {
    for ( unsigned i = 0; i <= img_.ni(); ++i ) {
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
      if ( node(index).link[dir] == null_index ) {
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
  for ( unsigned j = 0; j <= img_.nj(); ++j ) {
    for ( unsigned i = 0; i <= img_.ni(); ++i ) {
      if ( vertex_index( i, j ) == null_index &&
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
        assert( vertex_index( i2, j2 ) == null_index &&
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
      if ( node(index).link[dir] != null_index ) {
        unsigned nbr = node(index).link[dir];
        unsigned back_dir = node(index).back_dir[dir];
        if ( node(nbr).link[back_dir] != index ) {
          vcl_cerr << "Bad back link on vertex " << index << " ("<<node(index).i
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

bool
vtol_extract_topology::
trace_face_boundary( vcl_vector<unsigned>& markers,
                     unsigned index,
                     unsigned dir,
                     region_type& chain_list,
                     int& region_label ) const
{
  unsigned start_index = index;
  int start_left;
  edge_labels( node(index).i, node(index).j, dir,
               start_left, region_label );

  if ( region_label + 1 == min_label_ )
    return false;

  // Find an interior pixel of this face basd on the first edge we
  // encounter. The vertex (i,j) corresponds to pixel-coordinate-space
  // (i-0.5,j-0.5). Use this and the direction to get a point on the
  // right of the first edge. The delta_* store the appropriate
  // offsets for each direction.
  //
  static const int delta_i[4] = {  0, -1, -1,  0 };
  static const int delta_j[4] = {  0,  0, -1, -1 };
  chain_list.i = node(index).i + delta_i[dir];
  chain_list.j = node(index).j + delta_j[dir];

  assert( chain_list.i < img_.ni() );
  assert( chain_list.j < img_.nj() );

  do {
    // Mark the current direction of the current node as travelled,
    // and go to the next node and find the outgoing direction there.

    assert( ! is_marked( markers[index], dir ) );
    set_mark( markers[index], dir );
    chain_list.push_back( node(index).edgel_chain[dir] );

    unsigned back_dir = node(index).back_dir[ dir ];
    index = node(index).link[ dir ];
    assert( index != null_index );

    // Look from right to left, so that we are more conservative at
    // corner touches. That is, we will take
    //
    //     +----+
    //     |    |
    //     +----+----+
    //          |    |
    //          +----+
    //
    // as two regions intead of one figure-8 region.
    //
    // We keep the object on the right, which actually corresponds to
    // the standard "keep the object on the left" in a right-handed
    // coordinate system.
    //
    unsigned i = node(index).i;
    unsigned j = node(index).j;
    int left, right;
    dir = back_dir;
    DEBUG( unsigned old_dir = dir % 4 );
    do {
      dir = (dir+3) % 4; // same as dir = dir - 1

      // Make sure we haven't done a full cycle.
      DEBUG( assert( dir != old_dir ) );

      edge_labels( i, j, dir, left, right );
    } while ( left == right || right != region_label );

  } while ( index != start_index );

  return true;
}


// ---------------------------------------------------------------------------
//                                                             collect regions

void
vtol_extract_topology::
collect_regions( region_collection& region_list ) const
{
  // Use to need marks about which nodes (vertices) and directions
  // have been processed.
  //
  vcl_vector< unsigned > markers( node_list_.size(), 0 );

  region_list.resize( max_label_ - min_label_ + 1 );

  // Process each vertex, generating the boundary chains
  //
  for ( unsigned i = 0; i < node_list_.size(); ++i ) {
    for ( unsigned dir = 0; dir < 4; ++dir ) {
      if ( ! is_marked( markers[i], dir ) &&
          node(i).link[dir] != null_index ) {
        region_type_sptr chain = new region_type;
        int label;
        if ( trace_face_boundary( markers, i, dir, *chain, label ) ) {
          assert( label >= min_label_ );
          region_list[ label - min_label_ ].push_back( chain );
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
      assert( node(i).link[dir] == null_index ||
              ( dir==0 && node(i).j==img_.nj() ) ||
              ( dir==1 && node(i).i==0 ) ||
              ( dir==2 && node(i).j==0 ) ||
              ( dir==3 && node(i).i==img_.ni() ) ||
              is_marked( markers[i], dir ) );
    }
  }
#endif
}


// ---------------------------------------------------------------------------
//                                                               compute faces

void
vtol_extract_topology::
compute_faces( vcl_vector< region_type_sptr > const& chains,
               vcl_vector< vtol_intensity_face_sptr >& faces,
               data_image_type const* data_img ) const
{
  assert( chains.size() > 0 );

  // Determine the containment tree by repeatedly adding the chains
  // into a tree. The adding process makes sure that the chain falls
  // into the appropriate place in the containment hierarchy.

  chain_tree_node universe( 0 );
  for ( unsigned i = 0; i < chains.size(); ++i ) {
    universe.add( chains[i] );
  }

  // If we have a data image, use it to add digital region information
  // to the face
  //
  if ( data_img ) {
    finder_type* finder = new finder_type( img_, vil_region_finder_4_conn );
    add_faces( faces, finder, data_img, &universe );
    delete finder;
  } else {
    add_faces( faces, 0, 0, &universe );
  }
}


// =============================================================================
//                                                                   VERTEX NODE
// =============================================================================


// ---------------------------------------------------------------------------
//                                                                 constructor

// If we include vbl_smart_ptr.txx in this file, Borland 55 will try
// to instantiate vbl_smart_ptr<vsol_point_2d> and fail because
// vsol_point_2d is incomplete. It seems to make the attempt because
// of the vtol_vertex_2d constructor call. Any call to a function in
// vtol_vertex_2d seems to cause the instantiation. I'm not sure if we
// are running into a compiler bug or missing some subtle detail in
// templates with incomplete types. -- Amitha Perera
//
// Uncommenting the following line will trigger the bug(?) with Borland.
//#include <vbl/vbl_smart_ptr.txx>

vtol_extract_topology::vertex_node::
vertex_node( unsigned in_i, unsigned in_j )
  : i( in_i ),
    j( in_j ),
    vertex( new vtol_vertex_2d( i-0.5, j-0.5 ) )
{
  // The edge[4] will already be default constructed to null. We need
  // to explicitly initialize the link[4] array since it is a built-in
  // type and thus will not be zero-initialized.
  for ( unsigned i = 0; i < 4; ++i ) {
    link[i] = null_index;
    back_dir[i] = 100;
  }
}


// =============================================================================
//                                                                   REGION TYPE
// =============================================================================


// ---------------------------------------------------------------------------
//                                                                   push back

void
vtol_extract_topology::region_type::
push_back( edgel_chain_sptr chain )
{
  list_.push_back( chain );
}


// ---------------------------------------------------------------------------
//                                                                        size

unsigned
vtol_extract_topology::region_type::
size() const
{
  return list_.size();
}


// ---------------------------------------------------------------------------
//                                                                 operator []

vdgl_edgel_chain_sptr const&
vtol_extract_topology::region_type::
operator[]( unsigned i ) const
{
  return list_[i]->chain;
}


// ---------------------------------------------------------------------------
//                                                              make one chain

vtol_one_chain_sptr
vtol_extract_topology::region_type::
make_one_chain( ) const
{
  vcl_vector< vtol_edge_sptr > edges;

  for ( unsigned i = 0; i < list_.size(); ++i ) {
    edges.push_back( &*list_[i]->edge );
  }

  return new vtol_one_chain( edges, /*is_cycle=*/ true );
}


// =============================================================================
//                                                               CHAIN TREE NODE
// =============================================================================

namespace {

// ---------------------------------------------------------------------------
//                                                                  constructor

chain_tree_node::
chain_tree_node( region_type_sptr in_region )
  : region( in_region )
{
}


// ---------------------------------------------------------------------------
//                                                                  destructor

chain_tree_node::
~chain_tree_node()
{
  vcl_vector<chain_tree_node*>::iterator itr = children.begin();
  for ( ; itr != children.end(); ++itr ) {
    delete *itr;
  }
}


// ---------------------------------------------------------------------------
//                                                                         add

void
chain_tree_node::
add( region_type_sptr new_region )
{
  vcl_vector<chain_tree_node*>::iterator itr;

  // First, determine if it should go further down the tree. If so,
  // add it to the appropriate child and exit immediately.
  //
  for ( itr = children.begin(); itr != children.end(); ++itr ) {
    if ( contains( (*itr)->region, new_region ) ) {
      (*itr)->add( new_region );
      return;
    }
  }

  // It belongs at this level. Create a new node for it, and find out
  // if this new node swallows up any of the existing children. Then
  // add the new node as a child of this.
  //
  chain_tree_node* new_node = new chain_tree_node( new_region );
  itr = children.begin();
  while ( itr != children.end() ) {
    if ( contains( new_region, (*itr)->region ) ) {
      new_node->children.push_back( *itr );
      this->children.erase( itr );
    } else {
      ++itr;
    }
  }
  this->children.push_back( new_node );
}


// ---------------------------------------------------------------------------
//                                                                   make face

vtol_intensity_face_sptr
chain_tree_node::
make_face( finder_type* find, data_image_type const* img ) const
{
  vcl_vector< vtol_one_chain_sptr > face_chains;
  face_chains.push_back( region->make_one_chain() );
  for ( unsigned i = 0; i < children.size(); ++i ) {
    face_chains.push_back( children[i]->region->make_one_chain() );
  }
  if ( find ) {
    assert( img );

    vcl_vector<unsigned> ri, rj;
    find->same_int_region( region->i, region->j, ri, rj );
    assert( ri.size() == rj.size() && !ri.empty() );

    vcl_vector<float> x, y;
    vcl_vector<unsigned short> intensity;
    for ( unsigned c = 0; c < ri.size(); ++c ) {
      x.push_back( static_cast<float>(ri[c]) );
      y.push_back( static_cast<float>(rj[c]) );
      intensity.push_back( (*img)( ri[c], rj[c] ) );
    }
    vdgl_digital_region r( x.size(), &x[0], &y[0], &intensity[0]  );
    return new vtol_intensity_face( &face_chains, r );
  } else {
    // create a face without a digital geometry
    return new vtol_intensity_face( face_chains );
  }
}

// ---------------------------------------------------------------------------
//                                                                       print

void
chain_tree_node::
print( vcl_ostream& ostr, unsigned indent ) const
{
  for ( unsigned i = 0; i < indent; ++i ) {
    ostr << ' ';
  }
  ostr << '['<<children.size()<<']';
  if ( ! children.empty() ) {
    ostr << "___\n";
    for ( unsigned i = 0; i < indent; ++i ) {
      ostr << ' ';
    }
    ostr << "      \\\n";
    for ( unsigned i = 0; i < children.size(); ++i ) {
      children[i]->print( ostr, indent+7 );
    }
  } else {
    ostr << '\n';
  }
}

} // end anonymous namespace


// =============================================================================
//                                                          NON-MEMBER FUNCTIONS
// =============================================================================

namespace {

// ---------------------------------------------------------------------------
//                                                                   add faces

void
add_faces( vcl_vector<vtol_intensity_face_sptr>& faces,
           finder_type* find,
           data_image_type const* img,
           chain_tree_node* node,
           bool even_level )
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

bool
contains( region_type_sptr a, region_type_sptr b )
{
  assert( b );
  if ( !a ) {
    return true;
  } else {
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

unsigned
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

vdgl_edgel_chain_sptr
smooth_chain( vdgl_edgel_chain_sptr chain,
              unsigned int num_pts )
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
    vcl_cerr << "line fit failed at start\n";
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
      vcl_cerr << "line fit failed at " << fit_start << '-' << fit_end << '\n';
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
  // constrainted fit to make sure we interpolate the end vertex, and
  // use this line for the final few edgel points.
  //
  if ( !reg.fit_constrained( chain->edgel(fit_end).x(),
                            chain->edgel(fit_end).y() ) ) {
    vcl_cerr << "line fit failed at end\n";
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

} // end anonymous namespace
