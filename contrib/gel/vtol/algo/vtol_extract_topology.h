#ifndef vtol_extract_topology_h_
#define vtol_extract_topology_h_

//:
// \file
// \author Amitha Perera
// \date   July 2003
//

#include <vxl_config.h>

#include <vcl_vector.h>

#include <vil/vil_image_view.h>

#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_one_chain_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>

// A class in the test harness that will test some of the internal
// methods
//
class test_vtol_extract_topology;

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
class vtol_extract_topology
{
public: // public types

  //: Input image type
  typedef vil_image_view< vxl_byte > label_image_type;

public: // public methods

  //: Prepare to extract the topology from \a image.
  vtol_extract_topology( label_image_type const& image );

  //: List of vertices in the segmentation
  //
  vcl_vector< vtol_vertex_2d_sptr >
  vertices() const;

  //: List of all the faces in the segmentation
  //
  vcl_vector< vtol_intensity_face_sptr >
  faces() const;

private:   // internal classes and constants

  //: Image of indicies into the vertex node list
  typedef vil_image_view< unsigned > index_image_type;

  // Defined in the .cxx file
  struct edgel_chain;

  typedef vbl_smart_ptr< edgel_chain > edgel_chain_sptr;

  // Defined in the .cxx file
  class region_type;

public:
  // internal typedef. Needs to be public for implementation reasons.

  typedef vbl_smart_ptr< region_type > region_type_sptr;

private:

  // The smart pointer classes need access to the internal classes so
  // that we can have smart pointers to the internal classes.
  friend class vbl_smart_ptr< edgel_chain >;
  friend class vbl_smart_ptr< region_type >;

  //: A node in the graph of vertices.
  //
  // The links correspond to edges between the vertices. Each vertex
  // is located at the corner of the pixel.
  //
  struct vertex_node
  {
    //: Create a node for vertex at (i,j).
    vertex_node( unsigned i, unsigned j );

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
    edgel_chain_sptr edgel_chain[4];
  };

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

  // For VC6, to give access to the constants
  friend struct vertex_node;

private: // internal methods

  // Allow the test harness to call on the "internal" member functions
  // for through testing.
  //
  friend class test_vtol_extract_topology;

  //: The label at pixel position (i,j).
  //
  // If (i,j) falls inside the input image, return the value of pixel (i,j) of the
  // input image. Otherwise, it will return min_label_ - 1.
  //
  int
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
               int& left, int& right ) const;

  //: The node index of the vertex at coordinate (i,j)
  unsigned
  vertex_index( unsigned i, unsigned j ) const;

  //: Marks vertex (i,j) as having the given index
  void
  set_vertex_index( unsigned i, unsigned j, unsigned index );

  //: The vertex node structure given by \a index
  vertex_node&
  node( unsigned index );

  //: The vertex node structure given by \a index
  vertex_node const&
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
  // This will return, in \a chain, the one chain of edges
  // corresponding to the closed contour bounding a region. It will
  // also return the label of the enclosed region.
  //
  void
  trace_face_boundary( vcl_vector<unsigned>& markers,
                       unsigned index,
                       unsigned dir,
                       region_type& chain,
                       int& region_label ) const;

  //: Create a set of faces given a set of boundary chains.
  //
  // The boundary chains must all bound regions with the same
  // label. This routine will determine which chains should be
  // interior boundaries and which should be exterior. It will add a
  // set of faces to \a faces based on this determination.
  //
  // Each face so added will form a single connected component.
  //
  void
  compute_faces( vcl_vector< region_type_sptr > const& chains,
                 vcl_vector< vtol_intensity_face_sptr >& faces ) const;

private: // internal data

  //: The input label image
  label_image_type const& img_;

  //: The label ranges in the image
  int min_label_, max_label_;

  //: List of vertices (which form the nodes of the graph)
  vcl_vector< vertex_node > node_list_;

  //: Quick conversion from vertex coordinates to vertex node indices
  index_image_type index_img_;

};

#endif // vtol_extract_topology_h_
