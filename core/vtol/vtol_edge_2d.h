
#ifndef vtol_edge_2d_h
#define vtol_edge_2d_h

// .NAME vtol_edge_2d - Represents the basic 1D topological entity
// .LIBRARY Topology
// .HEADER SpatialObjects Package
// .INCLUDE Topology/vtol_edge_2d.h
// .FILE vtol_edge_2d.h
// .FILE vtol_edge_2d.C
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
  vtol_vertex_2d *_v1;
  vtol_vertex_2d *_v2;

public:

  //: \brief Constructors and Destructors

  vtol_edge_2d(void);
  vtol_edge_2d(vtol_vertex_2d *,
               vtol_vertex_2d *,
               vsol_curve_2d_ref c=0);
  vtol_edge_2d(vtol_edge_2d const &);
  vtol_edge_2d(vtol_zero_chain_2d *);
  vtol_edge_2d(vcl_vector<vtol_zero_chain_2d *> &);
  vtol_edge_2d(vsol_curve_2d_ref);
  vtol_edge_2d(vtol_vertex_2d *,
               vcl_vector<double> &);
  vtol_edge_2d(vtol_topology_object_2d *,
               vtol_topology_object_2d *,
               vsol_curve_2d_ref c=0);
  // vtol_edge_2d (double, double, double, double, double, double, vsol_curve_2d* = NULL);
  vtol_edge_2d(double,
               double,
               double,
               double,
               vsol_curve_2d_ref c=0);
  ~vtol_edge_2d();
  
  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //: See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_ref clone(void) const;

  //: \brief Accessors

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_2d_type topology_type(void) const;

  inline const vsol_curve_2d *get_curve(void) const { return _curve;}
  inline vsol_curve_2d *get_curve(void) { return _curve;}
  void set_curve(vsol_curve_2d_ref newcurve);
  void set_curve(vsol_curve_2d_ref newcurve,
                 vsol_curve_2d_ref &oldcurve);

  //:
  // get the vertices 

  inline vtol_vertex_2d *get_v1(void)
  {
    return _v1;
  }
  inline vtol_vertex_2d *get_v2(void)
  {
    return _v2;
  }

  //: 
  // get the zero chain

  inline vtol_zero_chain_2d *get_zero_chain(void)
  {
    return (vtol_zero_chain_2d*)_inferiors[0];
  }
 

  //:
  // set the vertices 
  void set_v1(vtol_vertex_2d *v);
  void set_v2(vtol_vertex_2d *v);
 
  void set_vertices_from_zero_chains(void);

  bool set_end_points(vtol_vertex_2d *endpt1,
                      vtol_vertex_2d *endpt2);
  bool replace_end_point(vtol_vertex_2d *,
                         vtol_vertex_2d *);
 

  bool operator==(vsol_spatial_object_2d &obj);
  bool operator==(vtol_edge_2d const &) const;

  bool add_edge_loop(vtol_one_chain_2d *);
  bool remove_edge_loop(vtol_one_chain_2d *);

  //:
  // Inferior/Superior Accessor Methods
  
  vtol_edge_2d *cast_to_edge_2d(void)
  {
    return this;
  }
  vcl_vector<vtol_vertex_2d*> *vertices(void);
  vcl_vector<vtol_zero_chain_2d*> *zero_chains(void);
  vcl_vector<vtol_edge_2d*> *edges(void);
  vcl_vector<vtol_one_chain_2d*> *one_chains(void);
  vcl_vector<vtol_face_2d*> *faces(void);
  vcl_vector<vtol_two_chain_2d*> *two_chains(void);
  vcl_vector<vtol_block_2d*> *blocks(void);
  vcl_vector<vtol_vertex_2d*> *endpoints(void);

  //:
  // Utility Functions

  vtol_edge_2d *copy(void)
  {
    return new vtol_edge_2d(*this);
  }
  virtual vtol_topology_object_2d *shallow_copy_with_no_links(void);
  virtual vsol_spatial_object_2d *spatial_copy(void)
  {
    return this->copy();
  }
  bool share_vertex_with(vtol_edge_2d *e);

  bool add_vertex(vtol_vertex_2d *);
  bool remove_vertex(vtol_vertex_2d *);

  bool is_endpoint(vtol_vertex_2d *) const;
  bool is_endpoint1(vtol_vertex_2d *) const;
  bool is_endpoint2(vtol_vertex_2d *) const;

  vtol_vertex_2d *other_endpoint(vtol_vertex_2d *);

 
  virtual void compute_bounding_box(void); // A local implementation

  void print (ostream &strm=cout);
  void describe(ostream &strm=cout,
                int blanking=0);
  

  bool remove(vtol_zero_chain_2d *zero_chain,
              vcl_vector<vtol_topology_object_2d *> &changes,
              vcl_vector<vtol_topology_object_2d *> &deleted);
  void deep_remove(vcl_vector<vtol_topology_object_2d *> &removed);
 
  virtual bool disconnect(vcl_vector<vtol_topology_object_2d *> &changes,
                          vcl_vector<vtol_topology_object_2d *> &deleted);
};

#endif
