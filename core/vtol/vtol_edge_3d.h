
#ifndef vtol_edge_3d_h
#define vtol_edge_3d_h

// .NAME vtol_edge_3d - Represents the basic 1D topological entity
// .LIBRARY Topology
// .HEADER SpatialObjects Package
// .INCLUDE Topology/vtol_edge_3d.h
// .FILE vtol_edge_3d.h
// .FILE vtol_edge_3d.cxx
//
// .SECTION Description
//  The vtol_edge_3d class is used to represent a topological edge.  An vtol_edge_3d
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
//      Changed the constructor vtol_edge_3d(vtol_edge_3d &) to vtol_edge_3d(const vtol_edge_3d &)
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


#include <vtol/vtol_topology_object_3d.h>
#include <vcl/vcl_vector.h>
#include <vtol/vtol_zero_chain_3d.h>
#include <vtol/vtol_vertex_3d.h>
#include <vsol/vsol_curve_3d_ref.h>

//: \brief topological edge

class vtol_edge_3d
  : public vtol_topology_object_3d
{
private:
  vsol_curve_3d_ref _curve;

  // Keeping vertex pointers inside of edge
  // for convenience...for now.
  vtol_vertex_3d *_v1;
  vtol_vertex_3d *_v2;

public:

  //: \brief Constructors and Destructors

  vtol_edge_3d(void);
  vtol_edge_3d(vtol_vertex_3d *,
               vtol_vertex_3d *,
               vsol_curve_3d_ref c=0);
  vtol_edge_3d(vtol_edge_3d const &);
  vtol_edge_3d(vtol_zero_chain_3d *);
  vtol_edge_3d(vcl_vector<vtol_zero_chain_3d *> &);
  vtol_edge_3d(vsol_curve_3d_ref);
  vtol_edge_3d(vtol_vertex_3d *,
               vcl_vector<double> &);
  vtol_edge_3d(vtol_topology_object_3d *,
               vtol_topology_object_3d *,
               vsol_curve_3d_ref c=0);

  vtol_edge_3d(double,
               double,
               double,
               double,
               double,
               double,
               vsol_curve_3d_ref c=0);
  ~vtol_edge_3d();
  
  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //: See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d_ref clone(void) const;

  //: \brief Accessors

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_3d_type topology_type(void) const;

  inline const vsol_curve_3d *get_curve(void) const { return _curve;}
  inline vsol_curve_3d *get_curve(void) { return _curve;}
  void set_curve(vsol_curve_3d_ref newcurve);
  void set_curve(vsol_curve_3d_ref newcurve,
                 vsol_curve_3d_ref &oldcurve);

  //:
  // get the vertices 

  inline vtol_vertex_3d *get_v1(void)
  {
    return _v1;
  }
  inline vtol_vertex_3d *get_v2(void)
  {
    return _v2;
  }

  //: 
  // get the zero chain

  inline vtol_zero_chain_3d *get_zero_chain(void)
  {
    return (vtol_zero_chain_3d*)_inferiors[0];
  }
 

  //:
  // set the vertices 
  void set_v1(vtol_vertex_3d *v);
  void set_v2(vtol_vertex_3d *v);
 
  void set_vertices_from_zero_chains(void);

  bool set_end_points(vtol_vertex_3d *endpt1,
                      vtol_vertex_3d *endpt2);
  bool replace_end_point(vtol_vertex_3d *,
                         vtol_vertex_3d *);
 

  bool operator==(vsol_spatial_object_3d &obj);
  bool operator==(vtol_edge_3d const &) const;

  bool add_edge_loop(vtol_one_chain_3d *);
  bool remove_edge_loop(vtol_one_chain_3d *);

  //:
  // Inferior/Superior Accessor Methods
  
  vtol_edge_3d *cast_to_edge_3d(void)
  {
    return this;
  }
  vcl_vector<vtol_vertex_3d*> *vertices(void);
  vcl_vector<vtol_zero_chain_3d*> *zero_chains(void);
  vcl_vector<vtol_edge_3d*> *edges(void);
  vcl_vector<vtol_one_chain_3d*> *one_chains(void);
  vcl_vector<vtol_face_3d*> *faces(void);
  vcl_vector<vtol_two_chain_3d*> *two_chains(void);
  vcl_vector<vtol_block_3d*> *blocks(void);
  vcl_vector<vtol_vertex_3d*> *endpoints(void);

  //:
  // Utility Functions

  vtol_edge_3d *copy(void)
  {
    return new vtol_edge_3d(*this);
  }
  virtual vtol_topology_object_3d *shallow_copy_with_no_links(void);
  virtual vsol_spatial_object_3d *spatial_copy(void)
  {
    return this->copy();
  }
  bool share_vertex_with(vtol_edge_3d *e);

  bool add_vertex(vtol_vertex_3d *);
  bool remove_vertex(vtol_vertex_3d *);

  bool is_endpoint(vtol_vertex_3d *) const;
  bool is_endpoint1(vtol_vertex_3d *) const;
  bool is_endpoint2(vtol_vertex_3d *) const;

  vtol_vertex_3d *other_endpoint(vtol_vertex_3d *);

 
  virtual void compute_bounding_box(void); // A local implementation

  void print (ostream &strm=cout);
  void describe(ostream &strm=cout,
                int blanking=0);
  

  bool remove(vtol_zero_chain_3d *zero_chain,
              vcl_vector<vtol_topology_object_3d *> &changes,
              vcl_vector<vtol_topology_object_3d *> &deleted);
  void deep_remove(vcl_vector<vtol_topology_object_3d *> &removed);
 
  virtual bool disconnect(vcl_vector<vtol_topology_object_3d *> &changes,
                          vcl_vector<vtol_topology_object_3d *> &deleted);
};

#endif
