// This is gel/vtol/vtol_vertex.h
#ifndef vtol_vertex_h_
#define vtol_vertex_h_
//:
// \file
// \brief Topological container for a spatial point, without geometry info
//
//  The vtol_vertex class is used to represent either a 2D or 3D point on
//  a topological structure.  A vtol_vertex does not actually maintain a pointer
//  to the vsol_point which is the actual spatial point, since this could either
//  be a vsol_point_2d or a vsol_point_3d.  See vtol_vertex_2d for this purpose.
//
// \verbatim
//  Modifications:
//   JLM December 1995, Added timeStamp (touch) to
//                      operations which affect bounds.
//
//   JLM October 1996,  Added the method EuclideanDistance(vtol_vertex &)
//      to permit Charlie Rothwell's Polyhedra code to be more
//      generic.  Note this is distance, NOT squared distance.
//   LEG May 2000. ported to vxl
//   Dec. 2002, Peter Vanroose -interface change: vtol objects -> smart pointers
// \endverbatim

#include "vtol_topology_object.h"
#include <vcl_iosfwd.h>

class vtol_vertex_2d;
class vtol_edge;
class vtol_zero_chain;
class vtol_one_chain;
class vtol_face;
class vtol_two_chain;
class vtol_block;

class vtol_vertex : public vtol_topology_object
{
 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default constructor
  //---------------------------------------------------------------------------
  vtol_vertex(void) {}

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_vertex();

  // Accessors

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_type topology_type(void) const { return VERTEX; }

  //---------------------------------------------------------------------------
  //: create a list of all connected vertices
  //---------------------------------------------------------------------------
  virtual void explore_vertex(vertex_list &);

  //---------------------------------------------------------------------------
  //: Is `this' has the same coordinates for its point than `other' ?
  //---------------------------------------------------------------------------
  virtual bool operator==(const vtol_vertex &other) const;
  inline bool operator!=(const vtol_vertex &other)const{return !operator==(other);}
  bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

  //---------------------------------------------------------------------------
  //: Assignment of `this' with `other' (copy the point not the links)
  //---------------------------------------------------------------------------
  virtual vtol_vertex& operator=(const vtol_vertex &other);

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a vertex, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_vertex *cast_to_vertex(void) const { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a vertex, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_vertex *cast_to_vertex(void) { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a 2D vertex, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_vertex_2d *cast_to_vertex_2d(void) const {return 0;}

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a 2D vertex, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_vertex_2d *cast_to_vertex_2d(void) {return 0;}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `inferior' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool valid_inferior_type(vtol_topology_object const* /*inf*/) const
  { return false; } // a vertex can never have an inferior
  bool valid_superior_type(vtol_zero_chain_sptr const& ) const { return true; }

  //---------------------------------------------------------------------------
  //: Is `this' connected with `v2' ?
  //  ie has a superior of `this' `v2' as inferior ?
  //---------------------------------------------------------------------------
  virtual bool is_connected(const vtol_vertex &v2);

  // methods that will be defined by inherited classes

  //---------------------------------------------------------------------------
  //: Create a line edge from `this' and `other' only if this edge does not exist.
  //  Otherwise it just returns the existing edge
  //  REQUIRE: other!=*this
  //---------------------------------------------------------------------------

  virtual vtol_edge_sptr new_edge(vtol_vertex_sptr const& other)=0;
 private: // deprecated:
  vtol_edge_sptr new_edge(vtol_vertex const& other);
 public:

  //: check to see if the vertex is part of the edge
  bool is_endpoint (const vtol_edge &);

  void print(vcl_ostream &strm=vcl_cout) const;
  void describe(vcl_ostream &strm=vcl_cout, int blanking=0) const;

  //: have the inherited classes copy the geometry
  virtual void copy_geometry(const vtol_vertex &other)=0;
  virtual bool compare_geometry(const vtol_vertex &other) const =0;

 protected:
  // \warning these should not be used by clients

  virtual vcl_vector<vtol_vertex*> *compute_vertices(void);
  virtual vcl_vector<vtol_edge*> *compute_edges(void);
  virtual vcl_vector<vtol_zero_chain*> *compute_zero_chains(void);
  virtual vcl_vector<vtol_one_chain*> *compute_one_chains(void);
  virtual vcl_vector<vtol_face*> *compute_faces(void);
  virtual vcl_vector<vtol_two_chain*> *compute_two_chains(void);
  virtual vcl_vector<vtol_block*> *compute_blocks(void);
};

#endif // vtol_vertex_h_
