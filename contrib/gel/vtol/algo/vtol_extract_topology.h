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
#include <vtol/vtol_intensity_face_sptr.h>

// A class in the test harness that will test some of the internal
// methods
//
class test_vtol_extract_topology;

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
  vtol_extract_topology_params()
    : num_for_smooth( 0 )
    { }
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
class vtol_extract_topology
{
 public: // public types

  //: Input label image type
  typedef vil_image_view< vxl_byte > label_image_type;

  typedef vxl_byte  data_pixel_type;

  //: Input data image type
  typedef vil_image_view< data_pixel_type > data_image_type;

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

private:   // internal classes and constants

  //: Image of indices into the vertex node list
  typedef vil_image_view< unsigned > index_image_type;

 public:
  // These types are implementation details, and should be private to
  // the class. However, there are some issues with creating smart
  // pointers to private classes. Until I have it figured out, I'm
  // making them public. -- Amitha Perera

  // Defined in the .cxx file
  struct edgel_chain;

  typedef vbl_smart_ptr< edgel_chain > edgel_chain_sptr;

  // Defined in the .cxx file
  class region_type;

  // internal typedef. Needs to be public for implementation reasons.

  typedef vbl_smart_ptr< region_type > region_type_sptr;

 private:

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

  //: Determine the max and min labels in the label image
  void
  compute_label_range();

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
                       int& region_label ) const;

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
  label_image_type const& img_;

  //: Parameters
  vtol_extract_topology_params params_;

  //: The label ranges in the image
  int min_label_, max_label_;

  //: List of vertices (which form the nodes of the graph)
  vcl_vector< vertex_node > node_list_;

  //: Quick conversion from vertex coordinates to vertex node indices
  index_image_type index_img_;
};

#endif // vtol_extract_topology_h_
