#ifndef vtol_extract_topology_h_
#define vtol_extract_topology_h_
//:
// \file
// \author Amitha Perera
// \date   July 2003
//
// \verbatim
// Modifications:
//   29sep04 - templated over label image type for >256 labels;
//             some bug fixes found w/ Amitha [roddy collins]
//
// \endverbatim


#include <vxl_config.h>
#include <vcl_vector.h>

#include <vbl/vbl_ref_count.h>

#include <vil/vil_image_view.h>
#include <vil/algo/vil_region_finder.h>

#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vsol/vsol_curve_2d_sptr.h>

#include <vdgl/vdgl_digital_region.h>


#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_one_chain_sptr.h>

// A class in the test harness that will test some of the internal
// methods
//
class test_vtol_extract_topology;

//: Some data types, further aliased in the definitions and implementations.

typedef vxl_byte vtol_extract_topology_data_pixel_type;
typedef vil_image_view< vtol_extract_topology_data_pixel_type > 
         vtol_extract_topology_data_image_type;

//: Controls the behaviour of vtol_extract_topology
struct vtol_extract_topology_params
{
  //: The number of pixels used in smoothing.
  //
  // The edgel curves will be smoothed by fitting a line at each edgel
  // point to the \a num_for_smooth neighbouring edgel points and
  // projecting onto that line. A value of 0 means no smoothing will
  // occur.
  //
  // Default: 0 (no smoothing)
  //
  unsigned num_for_smooth;

  vtol_extract_topology_params&
  set_smooth( unsigned s ) { num_for_smooth = s; return *this; }

  // Please don't add a constructor that takes arguments.

  //: Construct with the default values for the parameters.
  //
  // The constructor does not take parameters by design. Use the
  // explicit set_* functions to set the parameters you wish to
  // change. This will make code more robust against changes to the
  // code and parameter set, because we don't have a bunch of unnamed
  // arguments to change or worry about.
  vtol_extract_topology_params() : num_for_smooth( 0 ) {}
};

//: Stores an edgel chain and a corresponding topological edge
//
// Although the edgel chain can be recovered from the edge, we will
// need the edgel chain often enough that it is worthwhile to cache
// the information.
//
struct vtol_extract_topology_edgel_chain
  : public vbl_ref_count
{
  vdgl_edgel_chain_sptr chain;
  vtol_edge_2d_sptr edge;
};

//: Stores the boundary of a region
//
// This stores the one chain corresponding to a complete region
// boundary. It also stores a point completely inside that
// region. This point is used to perform containment checks between
// regions.
//
// \sa contains
//
class vtol_extract_topology_region_type
  : public vbl_ref_count
{
 public:
  typedef vbl_smart_ptr< vtol_extract_topology_edgel_chain > edgel_chain_sptr;

  //: Add an edge to this region
  void
  push_back( edgel_chain_sptr chain );

  //: The number of edges in the boundary
  unsigned
  size() const;

  //: Extract segment \a i of the boundary one chain
  vdgl_edgel_chain_sptr const&
  operator[]( unsigned i ) const;

  //: Create a vtol_one_chain describing the boundary
  vtol_one_chain_sptr
  make_one_chain() const;

  //: Location of a pixel inside the region
  unsigned i, j;

 private:

  //: The list of bounday edges (which are edgel chains)
  vcl_vector< edgel_chain_sptr > list_;
};

//: A node in the graph of vertices.
//
// The links correspond to edges between the vertices. Each vertex
// is located at the corner of the pixel.
//
struct vtol_extract_topology_vertex_node
{
  //: Create a node for vertex at (i,j).
  vtol_extract_topology_vertex_node( unsigned i, unsigned j );
  
  //: Location
  unsigned i, j;
  
  //: vtol vertex in pixel coordinates.
  vtol_vertex_2d_sptr vertex;
  
  //: Neighbouring vertices in the graph.
  unsigned link[4];
  
  //: Direction in which we exit the neighbouring node to get back to this node.
  //
  // That is, (this->link)[n].link[ this->back_dir[n] ] == indexof(this).
  //
  unsigned back_dir[4];
  
  //: Edgel chains leading to neighbouring vertices.
  vbl_smart_ptr< vtol_extract_topology_edgel_chain > edgel_chain[4];

  //: Null index value.
  //
  // A vertex with an index value >= this value does not correspond to
  // a node in the graph.
  //
  static const unsigned null_index   VCL_STATIC_CONST_INIT_INT_DECL( unsigned(-2) );

  //: "Processed" index value
  //
  // This is used to indicate that the boundary edge following went
  // through a vertex.
  //
  static const unsigned done_index   VCL_STATIC_CONST_INIT_INT_DECL( unsigned(-1) );

};

//: Extracts the topology from a segmentation label image.
//
// This class contains the functionality to extract a set of regions
// from an image of region labels. Vertices are created only at
// junctions where possible. The edges connecting these vertices
// follow the boundary of the regions, and hence are often not
// straight lines.
//
// Vertices are formed at the corners of pixels, and edges are formed
// along the "cracks" between pixels. In this implementation, the
// vertices are indexed by the top-left corner of the pixel. That is,
// if the image has M x N pixels, it will have M+1 x N+1 vertices,
// indexed (0,0) to (M,N). A vertex (i,j) is positioned at
// (i-0.5,j-0.5) in pixel coordinates.
//
// Now templated; LABEL_TYPE can be vxl_byte, vxl_uint_16, etc.
//

template< typename LABEL_TYPE >
class vtol_extract_topology
{
 public: // public types

  //: alias for the region type
  typedef vtol_extract_topology_region_type region_type;
  typedef vbl_smart_ptr< region_type > region_type_sptr;

  //: Input label image type
  typedef vil_image_view< LABEL_TYPE > label_image_type;
  typedef vil_region_finder< LABEL_TYPE > finder_type;

  //: Input data image type
  typedef vtol_extract_topology_data_image_type data_image_type;


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
    
    chain_tree_node( region_type_sptr in_region ): region( in_region ) 
    { /* empty ctor */ }
    ~chain_tree_node() {
      vcl_vector<chain_tree_node*>::iterator itr = children.begin();
      for ( ; itr != children.end(); ++itr ) {
        delete *itr;
      }
    }
    
    // Add a new region below this node. Prerequiste: the new region is
    // contained within this chain.
    //
    void
    add( region_type_sptr new_region )
    {
      vcl_vector<chain_tree_node*>::iterator itr;
      
      // First, determine if it should go further down the tree. If so,
      // add it to the appropriate child and exit immediately.
      //
      for ( itr = children.begin(); itr != children.end(); ++itr ) {
        if ( vtol_extract_topology<LABEL_TYPE>::contains( (*itr)->region, new_region ) ) {
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


    
    // Create a face from the regions at this node and its children.
    //
    vtol_intensity_face_sptr
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
        vcl_clog << "Creating region with NO pixels"  << vcl_endl;
        return new vtol_intensity_face( face_chains );
      }
    }

    
    void
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
  };


 public: // public methods

  //: Prepare to extract the topology from \a image.
  vtol_extract_topology( label_image_type const& image,
                         vtol_extract_topology_params const& params = vtol_extract_topology_params() );

  //: List of vertices in the segmentation
  //
  vcl_vector< vtol_vertex_2d_sptr >
  vertices() const;

  //: List of all the faces in the segmentation
  //
  // These will be intensity faces without a digital region. This
  // function should probably return vtol_face_2d objects, not
  // vtol_intensity_face objects.
  //
  vcl_vector< vtol_intensity_face_sptr >
  faces() const;

  //: List of all the faces in the segmentation
  //
  // The faces will have digital regions formed using \a data_img.
  // \a data_img must have the same size as the label image.
  //
  vcl_vector< vtol_intensity_face_sptr >
  faces( data_image_type const& data_img ) const;

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
             bool even_level = false ) const ;


  // Returns true if the region bounded by a contains the region bounded
  // by b.  Assumes that (1) the chains are cycles and thus bound a
  // region, (2) that a containment relationship holds (i.e. no partial
  // overlap). A special case allows for a "universe": if a is null,
  // then the function will return true. Currently, b cannot be null.
  //
  static 
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
  static
  unsigned
  num_crosses_x_pos_ray( double x, double y, vdgl_edgel_chain const& chain );
  
  // Smoothes an edgel chain by fitting a line to the local
  // neighbourhood and projecting onto that line
  //
  vdgl_edgel_chain_sptr
  smooth_chain( vdgl_edgel_chain_sptr chain,
                unsigned int num_pts ) const;
  
private:   // internal classes and constants
  
  //: Queries into label_img_ return either (label, true) or (0, false)
  // ...this avoids using one of the possible labels as an off-image flag

  struct LabelPoint
  {
    LABEL_TYPE label;
    bool valid;
    LabelPoint(): label(0), valid(false) {}
    LabelPoint(LABEL_TYPE const& lt, bool v): label(lt), valid(v) {}
    bool operator==( LabelPoint const& lp ) { 
      return (lp.valid == this->valid) && ( (lp.valid) ? lp.label == this->label : true );
    }
    bool operator!=( LabelPoint const& lp ) {
      return !(*this == lp);
    }
  };

  typedef vtol_extract_topology_edgel_chain edgel_chain;
  typedef vbl_smart_ptr< edgel_chain > edgel_chain_sptr;

  //: Image of indices into the vertex node list
  typedef vil_image_view< unsigned > index_image_type;

  // For VC6, to give access to the constants
  friend struct vtol_extract_topology_vertex_node;

  // Allow the test harness to call on the "internal" member function is_edge()
  // for thorough testing.
  //
  friend class test_vtol_extract_topology;

  //: Determine the max and min labels in the label image
  void
  compute_label_range();

  //: The label at pixel position (i,j).
  //
  // If (i,j) falls inside the input image, return the value of pixel (i,j) of the
  // input image. Otherwise, it will return min_label_ - 1.
  //
  LabelPoint
  label( unsigned i, unsigned j ) const;

  //: Is this a vertex bordering at least three regions?
  bool
  is_junction_vertex( unsigned i, unsigned j ) const;

  //: Is this a non-interior vertex?
  bool
  is_boundary_vertex( unsigned i, unsigned j ) const;

  //: True iff there is an edge from vertex (i,j) in direction dir.
  //
  // The directions are
  // \verbatim
  //             3
  //
  //             ^
  //             |
  //             |
  //    2 <--- (i,j) ---> 0           +----->
  //             |                    |       i
  //             |                    |
  //             v                    v
  //                                    j
  //             1
  // \endverbatim
  //
  // The `crack' between two pixels forms an edge iff the pixels have
  // different labels.
  //
  bool
  is_edge( unsigned i, unsigned j, unsigned dir ) const;

  //: The labels of the pixels on either side of the edge.
  //
  // Left and right correspond to the displayed image. That is, left
  // and right are defined on a left-handed coordinate system.
  //
  void
  edge_labels( unsigned i, unsigned j, unsigned dir,
               LabelPoint& left, LabelPoint& right ) const;

  //: The node index of the vertex at coordinate (i,j)
  unsigned
  vertex_index( unsigned i, unsigned j ) const;

  //: Marks vertex (i,j) as having the given index
  void
  set_vertex_index( unsigned i, unsigned j, unsigned index );

  //: The vertex node structure given by \a index
  vtol_extract_topology_vertex_node&
  node( unsigned index );

  //: The vertex node structure given by \a index
  vtol_extract_topology_vertex_node const&
  node( unsigned index ) const;

  //: Move ( \a i, \a j ) in direction \a dir
  void
  move( unsigned dir, unsigned& i, unsigned& j );

  //: Mark direction \a dir as travelled
  //
  // \a marker is updated to reflect the new movement.
  //
  void
  set_mark( unsigned& marker, unsigned dir ) const;

  //: Check if a direction has been travelled
  //
  // The directions that have been travelled is extracted from \a marker.
  //
  bool
  is_marked( unsigned marker, unsigned dir ) const;

  //: Add an edge chain into the graph structure.
  //
  // This will trace the edgel chain starting at the vertex at (i,j),
  // starting in direction \a dir. It will update the graph nodes to
  // reflect the new chain.
  //
  void
  trace_edge_chain( unsigned i, unsigned j, unsigned dir );

  //: This will create the full graph structure of the vertices.
  //
  // It will find all the vertices and connecting edges necessary to
  // describe the topology of the segmentation. It will try to create
  // as few vertices as possible to do so.
  //
  void
  construct_topology();

  //: Trace the boundary of a region starting at vertex \a index going \a dir.
  //
  // It will output, in \a chain, the one chain of edges corresponding
  // to the closed contour bounding a region. It will also return the
  // label of the enclosed region.
  //
  // It will only trace regions interior to the image. That is, it
  // will not trace a "face" containing the region outside the image
  // boundaries. The return value will indicate this. A return value
  // of "true" indicates that the a region was successfully extracted,
  // and that \a chain_list and \a region_label are valid. A return
  // value of "false" indicates that a region was not traaced.
  //
  bool
  trace_face_boundary( vcl_vector<unsigned>& markers,
                       unsigned index,
                       unsigned dir,
                       region_type& chain,
                       LabelPoint& region_label ) const;

  typedef vcl_vector< vcl_vector< region_type_sptr > > region_collection;

  //: Trace the boundary curves and collect up a set of regions.
  //
  // The output in \a out_region_list is a set of regions indexed by
  // the label of the region. So, out_region_list[x] will be a set of
  // closed boundaries bounding regions with label x.
  //
  void
  collect_regions( region_collection& out_region_list ) const;

  //: Create a set of faces given a set of boundary chains.
  //
  // The boundary chains must all bound regions with the same
  // label. This routine will determine which chains should be
  // interior boundaries and which should be exterior. It will add a
  // set of faces to \a faces based on this determination.
  //
  // Each face so added will form a single connected component.
  //
  // If data_img is not null, each face will have a digital region
  // (vdgl_digital_region).
  //
  void
  compute_faces( vcl_vector< region_type_sptr > const& chains,
                 vcl_vector< vtol_intensity_face_sptr >& faces,
                 data_image_type const* data_img ) const;

 private: // internal data

  //: The input label image
  label_image_type const& label_img_;

  //: Parameters
  vtol_extract_topology_params params_;

  //: The label ranges in the image
  LABEL_TYPE min_label_, max_label_;

  //: List of vertices (which form the nodes of the graph)
  vcl_vector< vtol_extract_topology_vertex_node > node_list_;

  //: Quick conversion from vertex coordinates to vertex node indices
  index_image_type index_img_;
};

#endif // vtol_extract_topology_h_
