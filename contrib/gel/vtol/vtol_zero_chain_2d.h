#ifndef vtol_zero_chain_2d_H
#define vtol_zero_chain_2d_H
//-----------------------------------------------------------------------------
//
// .NAME    vtol_zero_chain_2d - Represents a set of Vertices
// .LIBRARY vtol
// .HEADER  gel package
// .INCLUDE vtol/vtol_zero_chain_2d.h
// .FILE    vtol_zero_chain_2d.cxx
//
// .SECTION Description
//  The vtol_zero_chain_2d class is used to represent a set of Vertices on
//  a topological structure. A vtol_zero_chain_2d maintains only the inferiors and
//  superiors. It is the topological inferior of an Edge.
//
// .SECTION Author
//     Patricia A. Vrobel
//     PTU - ported may 2000
//
// .SECTION Modifications:
//   JLM Dec 1995, Added timeStamp (Touch) to operations which affect bounds.
//   02-26-97 Added implementation for virtual Transform() - Peter Vanroose
//
//-----------------------------------------------------------------------------

#include <vtol/vtol_topology_object_2d.h>
#include <vcl_vector.h>
#include <vtol/vtol_vertex_2d.h>


class vtol_zero_chain_2d
  : public vtol_topology_object_2d
{
public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default constructor. Empty zero-chain
  //---------------------------------------------------------------------------
  explicit vtol_zero_chain_2d(void);

  //---------------------------------------------------------------------------
  //: Constructor from two vertices (to make an edge creation easier)
  //: REQUIRE: v1.ptr()!=0 and v2.ptr()!=0 and v1.ptr()!=v2.ptr()
  //---------------------------------------------------------------------------
  explicit vtol_zero_chain_2d(vtol_vertex_2d &v1,
                              vtol_vertex_2d &v2);

  //---------------------------------------------------------------------------
  //: Constructor from an array of vertices
  //: REQUIRE: new_vertices.size()>0
  //---------------------------------------------------------------------------
  explicit
  vtol_zero_chain_2d(const vcl_vector<vtol_vertex_2d_sptr> &new_vertices);

  //---------------------------------------------------------------------------
  //: Copy constructor. Copy the vertices and the links
  //---------------------------------------------------------------------------
  vtol_zero_chain_2d(const vtol_zero_chain_2d &other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_zero_chain_2d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //: See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_sptr clone(void) const;

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_2d_type topology_type(void) const;

  //---------------------------------------------------------------------------
  //: Return the first vertex of `this'. If it does not exist, return 0
  //---------------------------------------------------------------------------
  virtual vtol_vertex_2d_sptr v0(void) const;

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a zero_chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_zero_chain_2d *cast_to_zero_chain(void) const;

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a zero_chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_zero_chain_2d *cast_to_zero_chain(void);

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

  //---------------------------------------------------------------------------
  //: Return the length of the zero-chain
  //---------------------------------------------------------------------------
  virtual int length(void) const;

  //---------------------------------------------------------------------------
  //: Is `this' equal to `other' ?
  //---------------------------------------------------------------------------
  virtual bool operator==(const vtol_zero_chain_2d &other) const;
  bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

  virtual void print(vcl_ostream &strm=vcl_cout) const;
  virtual void describe(vcl_ostream &strm=vcl_cout,
                        int blanking=0) const;

  // : Warning - should not be used by clients
protected:
  virtual vcl_vector<vtol_vertex_2d*> *compute_vertices(void);
  virtual vcl_vector<vtol_edge_2d*> *compute_edges(void);
  virtual vcl_vector<vtol_zero_chain_2d*> *compute_zero_chains(void);
  virtual vcl_vector<vtol_one_chain_2d*> *compute_one_chains(void);
  virtual vcl_vector<vtol_face_2d*> *compute_faces(void);
  virtual vcl_vector<vtol_two_chain_2d*> *compute_two_chains(void);
  virtual vcl_vector<vtol_block_2d*> *compute_blocks(void);


};

#endif // #ifndef vtol_zero_chain_2d_H
