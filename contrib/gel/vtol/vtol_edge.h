// This is gel/vtol/vtol_edge.h
#ifndef vtol_edge_h_
#define vtol_edge_h_
//:
// \file
// \brief Represents the basic 1D topological entity
//
//  The vtol_edge class is used to represent a topological edge. For convenience
//  in working with linear edges, pointers to the two endpoint vertices
//  are maintained. The direction of an edge is the vector from v1_ to v2_.
//  A 1-chain is the superior of the edge in the topological
//  hierarchy, and a 0-chain is the inferior of the edge in the
//  topological hierarchy.  In rare cases, an edge will be used to represent
//  a ray.  In this case, only v1_ will be valid and v2_ will be NULL.
//
// \verbatim
//  Modifications:
//   JLM December 1995, Added timeStamp (Touch) to
//       operations which affect bounds.
//   JLM December 1995 Added method for ComputeBoundingBox
//       (Need to decide proper policy for curved edges
//       and possibly inconsistent linear edge geometry)
//
//   Samer Abdallah - 21/06/1996
//    Robotics Research Group, Oxford
//    Changed the constructor vtol_edge(vtol_edge &) to vtol_edge(const vtol_edge &)
//
//   JLM September 1996 - Added default curve argument to two vertex
//     constructors.  This addition is necessary because it is not
//     always the case that one wants to construct an ImplicitLine from
//     two vertices.  The curve might be a DigitalCurve, for example.
//     On the other hand in grouping or similar applications, the
//     curve endpoints can be different from the topological connections.
//     So, it is necessary to pass in the vertices as well as the curve.
//
//   02-26-97 - Peter Vanroose - Added implementation for virtual Transform()
//   May 2000, PTU - ported to vxl
//   Dec. 2002,  Peter Vanroose -interface change: vtol objects -> smart pointers
//   9 Jan. 2003, Peter Vanroose - added pure virtual "copy_geometry()"
// \endverbatim

#include <vcl_iosfwd.h>
#include <vcl_vector.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_zero_chain.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_vertex.h>
class vtol_edge_2d;

//: topological edge

class vtol_edge : public vtol_topology_object
{
 protected:
  //***************************************************************************
  // Data members
  //***************************************************************************

  // Keeping vertex pointers inside of edge
  // for convenience...for now.

  vtol_vertex_sptr v1_;
  vtol_vertex_sptr v2_;

 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default constructor. Empty edge. Not a valid edge.
  //---------------------------------------------------------------------------
  vtol_edge() : v1_(0), v2_(0){ link_inferior(new vtol_zero_chain); }

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_edge();

 private: // has been superceeded by is_a()
  //: Return the topology type
  virtual vtol_topology_object_type topology_type() const { return EDGE; }

 public:
  //---------------------------------------------------------------------------
  //: Return the first endpoint
  //---------------------------------------------------------------------------
  vtol_vertex_sptr v1() const { return v1_; }

  //---------------------------------------------------------------------------
  //: Return the second endpoint
  //---------------------------------------------------------------------------
  vtol_vertex_sptr v2() const { return v2_; }

  //---------------------------------------------------------------------------
  //: Return the first non-empty zero-chain of `this'
  //---------------------------------------------------------------------------
  virtual vtol_zero_chain_sptr zero_chain() const;

  //---------------------------------------------------------------------------
  //: Set the first endpoint.
  //---------------------------------------------------------------------------
  virtual void set_v1(vtol_vertex_sptr new_v1);

  //---------------------------------------------------------------------------
  //: Set the last endpoint
  //---------------------------------------------------------------------------
  virtual void set_v2(vtol_vertex_sptr new_v2);

  //---------------------------------------------------------------------------
  //: Determine the endpoints of an edge from its inferiors
  //---------------------------------------------------------------------------
  virtual void set_vertices_from_zero_chains();

  //---------------------------------------------------------------------------
  //: replace the current end point
  //---------------------------------------------------------------------------
  virtual void replace_end_point(vtol_vertex &current_end_point,
                                 vtol_vertex &new_end_point);

  virtual bool operator==(const vtol_edge &other) const;
  inline bool operator!=(const vtol_edge &other)const{return !operator==(other);}
  bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

  virtual void add_edge_loop(vtol_one_chain_sptr const&);
  virtual void remove_edge_loop(vtol_one_chain_sptr const&);
 private:
  // Deprecated:
  virtual void add_edge_loop(vtol_one_chain &);
  virtual void remove_edge_loop(vtol_one_chain &);
 public:
  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an edge, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_edge *cast_to_edge() const { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an edge, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_edge *cast_to_edge() { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an edge, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_edge_2d *cast_to_edge_2d() const {return 0;}

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an edge, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_edge_2d *cast_to_edge_2d() {return 0;}

  //***************************************************************************
  // Status report
  //***************************************************************************

  void link_inferior(vtol_zero_chain_sptr inf);
  void unlink_inferior(vtol_zero_chain_sptr inf);

  //---------------------------------------------------------------------------
  //: Is `inferior' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool valid_inferior_type(vtol_topology_object const* inferior) const
  { return inferior->cast_to_zero_chain() != 0; }
  bool valid_inferior_type(vtol_zero_chain_sptr const& ) const { return true; }
  bool valid_superior_type(vtol_one_chain_sptr const& ) const { return true; }

  //:
  // Inferior/Superior Accessor Methods
 protected:
  // \warning should not be used by clients
  virtual vcl_vector<vtol_vertex*> *compute_vertices();
  virtual vcl_vector<vtol_edge*> *compute_edges();
  virtual vcl_vector<vtol_zero_chain*> *compute_zero_chains();
  virtual vcl_vector<vtol_one_chain*> *compute_one_chains();
  virtual vcl_vector<vtol_face*> *compute_faces();
  virtual vcl_vector<vtol_two_chain*> *compute_two_chains();
  virtual vcl_vector<vtol_block*> *compute_blocks();
 public:

  //: get a list of endpoints
  virtual vertex_list *endpoints();

  // Utility Functions

  virtual bool share_vertex_with(vtol_edge &other);

  virtual bool add_vertex(vtol_vertex_sptr const&);
  virtual bool remove_vertex(vtol_vertex_sptr const&);
 private:
  // Deprecated:
  virtual bool add_vertex(vtol_vertex &);
  virtual bool remove_vertex(vtol_vertex &);
 public:
  virtual bool is_endpoint(const vtol_vertex &) const;
  virtual bool is_endpoint1(const vtol_vertex &) const;
  virtual bool is_endpoint2(const vtol_vertex &) const;

  virtual vtol_vertex_sptr other_endpoint(const vtol_vertex &) const;

  virtual void print(vcl_ostream &strm=vcl_cout) const;
  virtual void describe(vcl_ostream &strm=vcl_cout,
                        int blanking=0) const;

  //: have the inherited classes copy the geometry
  virtual void copy_geometry(const vtol_edge &other)=0;

  //: compare the geometry
  virtual bool compare_geometry(const vtol_edge &other) const =0;

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return vcl_string("vtol_edge"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(const vcl_string& cls) const { return cls==is_a(); }
};

#endif // vtol_edge_h_
