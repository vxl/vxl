#ifndef vtol_edge_2d_h
#define vtol_edge_2d_h

// .NAME vtol_edge_2d - Represents the basic 1D topological entity
// .LIBRARY vtol
// .HEADER vxl package
// .INCLUDE vtol/vtol_edge_2d.h
// .FILE vtol_edge_2d.cxx
//
// .SECTION Description
//  The vtol_edge_2d class is used to represent a topological edge.  An vtol_edge_2d
//  maintains a data pointer to the specific mathematical curve geometry
//  which describes the point set that makes up the edge.  For convenience
//  in working with linear edges, pointers to the two endpoint vertices
//  are maintained. The direction of an edge is the vector from _v1 to _v2.
//  A OneChain is the Superior of the edge in the topological
//  hierarchy, and a ZeroChain is the Inferior of the edge in the
//  topological hierarchy.  In rare cases, an edge will be used to represent
//  a Ray.  In this case, only _v1 will be valid and _v2 will be NULL.
//
// .SECTION Modifications:
//     JLM December 1995, Added timeStamp (Touch) to
//         operations which affect bounds.
//     JLM December 1995 Added method for ComputeBoundingBox
//         (Need to decide proper policy for curved edges
//         and possibly inconsistent linear edge geometry)
//
//     Samer Abdallah - 21/06/1996
//      Robotics Research Group, Oxford
//      Changed the constructor vtol_edge_2d(vtol_edge_2d &) to vtol_edge_2d(const vtol_edge_2d &)
//
//     JLM September 1996 - Added default curve argument to two vertex
//     constructors.  This addition is necessary because it is not
//     always the case that one wants to construct an ImplicitLine from
//     two vertices.  The curve might be a DigitalCurve, for example.
//     On the other hand in grouping or similar applications, the
//     curve endpoints can be different from the topological connections.
//     So, it is necessary to pass in the vertices as well as the curve.
//
//   02-26-97 Added implementation for virtual Transform() - Peter Vanroose
//   PTU ported to vxl may 2000.

#include <vtol/vtol_edge_2d_ref.h>

#include <vtol/vtol_topology_object_2d.h>
#include <vcl/vcl_vector.h>
#include <vtol/vtol_zero_chain_2d.h>
#include <vtol/vtol_vertex_2d.h>
#include <vsol/vsol_curve_2d_ref.h>

//: \brief topological edge

class vtol_edge_2d
  : public vtol_topology_object_2d
{
private:
  vsol_curve_2d_ref _curve;

  // Keeping vertex pointers inside of edge
  // for convenience...for now.
  vtol_vertex_2d_ref _v1;
  vtol_vertex_2d_ref _v2;

public:
  //***************************************************************************
  // Initialization
  //***************************************************************************
  
  //---------------------------------------------------------------------------
  //: Default constructor. Empty edge. Not a valid edge.
  //---------------------------------------------------------------------------
  explicit vtol_edge_2d(void);

  //---------------------------------------------------------------------------
  //: Constructor from the two endpoints `new_v1', `new_v2' and from a
  //: curve `new_curve'. If `new_curve' is 0, a line is created from
  //: `new_v1' and `new_v2'.
  //---------------------------------------------------------------------------
  explicit vtol_edge_2d(vtol_vertex_2d &new_v1,
                        vtol_vertex_2d &new_v2,
                        const vsol_curve_2d_ref &new_curve=0);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vtol_edge_2d(const vtol_edge_2d &other);

  //---------------------------------------------------------------------------
  //: Constructor from a zero-chain.
  //---------------------------------------------------------------------------
  explicit vtol_edge_2d(vtol_zero_chain_2d &new_zero_chain);

  //---------------------------------------------------------------------------
  //: Constructor from an array of zero-chains.
  //---------------------------------------------------------------------------
  explicit vtol_edge_2d(zero_chain_list_2d &new_zero_chains);

  explicit vtol_edge_2d(vsol_curve_2d &);
  explicit vtol_edge_2d(vtol_vertex_2d &,
                        vcl_vector<double> &);
 
  explicit vtol_edge_2d(double,
                        double,
                        double,
                        double,
                        vsol_curve_2d_ref c=0);
  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_edge_2d();
  
  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //: See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_ref clone(void) const;

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_2d_type topology_type(void) const { return EDGE; } 

  //---------------------------------------------------------------------------
  //: Return the curve associated to `this'
  //---------------------------------------------------------------------------
  virtual vsol_curve_2d_ref curve(void) const; // { return _curve; }

  //---------------------------------------------------------------------------
  //: Set the curve with `new_curve'
  //---------------------------------------------------------------------------
  virtual void set_curve(vsol_curve_2d &new_curve);

  //---------------------------------------------------------------------------
  //: Return the first endpoint
  //---------------------------------------------------------------------------
  virtual vtol_vertex_2d_ref v1(void) const { return _v1; }

  //---------------------------------------------------------------------------
  //: Return the second endpoint
  //---------------------------------------------------------------------------
  virtual vtol_vertex_2d_ref v2(void) const { return _v2; }

  //---------------------------------------------------------------------------
  //: Return the first zero-chain of `this'
  //---------------------------------------------------------------------------
  virtual vtol_zero_chain_2d_ref zero_chain(void) const;

  //---------------------------------------------------------------------------
  //: Set the first endpoint.
  //---------------------------------------------------------------------------
  virtual void set_v1(vtol_vertex_2d *new_v1);

  //---------------------------------------------------------------------------
  //: Set the last endpoint
  //---------------------------------------------------------------------------
  virtual void set_v2(vtol_vertex_2d *new_v2);

  //---------------------------------------------------------------------------
  // Task: Determine the endpoints of an edge from its inferiors
  //---------------------------------------------------------------------------
  virtual void set_vertices_from_zero_chains(void);

  //---------------------------------------------------------------------------
  //: Set the first and last endpoints
  //: REQUIRE: vertex_of_edge(new_v1) and vertex_of_edge(new_v2)
  //---------------------------------------------------------------------------
  virtual void set_end_points(vtol_vertex_2d &new_v1,
                              vtol_vertex_2d &new_v2);

  virtual void replace_end_point(vtol_vertex_2d &,
                                 vtol_vertex_2d &);
 
  virtual bool operator==(const vtol_edge_2d &other) const;
  bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

  virtual void add_edge_loop(vtol_one_chain_2d &);
  virtual void remove_edge_loop(vtol_one_chain_2d &);

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an edge, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_edge_2d *cast_to_edge(void) const;
  
  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an edge, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_edge_2d *cast_to_edge(void);

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `inferior' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool
  valid_inferior_type(const vtol_topology_object_2d &inferior) const;

  //---------------------------------------------------------------------------
  //: Is `superior' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool
  valid_superior_type(const vtol_topology_object_2d &superior) const;

  //:
  // Inferior/Superior Accessor Methods

  // : Warning - should not be used by clients
  virtual vcl_vector<vtol_vertex_2d*> *compute_vertices(void);
  virtual vcl_vector<vtol_edge_2d*> *compute_edges(void);
  virtual vcl_vector<vtol_zero_chain_2d*> *compute_zero_chains(void);
  virtual vcl_vector<vtol_one_chain_2d*> *compute_one_chains(void);
  virtual vcl_vector<vtol_face_2d*> *compute_faces(void);
  virtual vcl_vector<vtol_two_chain_2d*> *compute_two_chains(void);
  virtual vcl_vector<vtol_block_2d*> *compute_blocks(void);

  //:
  // get a list of endpoints
  virtual vertex_list_2d *endpoints(void);

  //:
  // Utility Functions
 
  virtual bool share_vertex_with(vtol_edge_2d &other);

  virtual bool add_vertex(vtol_vertex_2d &);
  virtual bool remove_vertex(vtol_vertex_2d &);

  virtual bool is_endpoint(const vtol_vertex_2d &) const;
  virtual bool is_endpoint1(const vtol_vertex_2d &) const;
  virtual bool is_endpoint2(const vtol_vertex_2d &) const;

  virtual vtol_vertex_2d_ref other_endpoint(const vtol_vertex_2d &) const;

 
  virtual void compute_bounding_box(void); // A local implementation

  virtual void print(ostream &strm=cout) const;
  virtual void describe(ostream &strm=cout,
                        int blanking=0) const;
};
#endif
