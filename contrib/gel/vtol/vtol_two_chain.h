// This is gel/vtol/vtol_two_chain.h
#ifndef vtol_two_chain_h_
#define vtol_two_chain_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief Represents a connected chain of faces
//
// The vtol_two_chain class is used to represent a set of faces on a topological
//  structure. A vtol_two_chain consists of its inferior faces and the superiors
//  on which it lies.  A vtol_two_chain may or may not be an ordered cycle.  If
//  the chain of faces encloses a volume, then the vtol_two_chain may be used as
//  the boundary of a topological vtol_block in a 3D structure.
//
// \author
//     Patricia A. Vrobel
//
// \verbatim
//  Modifications:
//   JLM December 1995, Added timeStamp (Touch) to
//       operations which affect bounds.
//   JLM December 1995, no local method for ComputeBoundingBox
//       Should use vtol_face geometry recursively to be proper.
//       Currently reverts to bounds on vertices from
//       vtol_topology_object::ComputeBoundingBox()
//   05/13/98  RIH replaced append by insert_after to avoid n^2 behavior
//   PTU May 2000 ported to vxl
//   Dec. 2002, Peter Vanroose -interface change: vtol objects -> smart pointers
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_iosfwd.h>
#include <vcl_vector.h>
#include <vtol/vtol_chain.h>
#include <vtol/vtol_face_2d_sptr.h>
class vtol_vertex;
class vtol_edge;
class vtol_zero_chain;
class vtol_one_chain;
class vtol_face;
class vtol_block;

class vtol_two_chain : public vtol_chain
{
 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  void link_chain_inferior(vtol_two_chain_sptr chain_inferior);
  void unlink_chain_inferior(vtol_two_chain_sptr chain_inferior);

  //---------------------------------------------------------------------------
  //: Default constructor
  //---------------------------------------------------------------------------
  vtol_two_chain(void) { is_cycle_=false; }

  //---------------------------------------------------------------------------
  //: Constructor
  //---------------------------------------------------------------------------
  explicit vtol_two_chain(int /*num_faces*/) { is_cycle_=false; }

  //---------------------------------------------------------------------------
  //: Constructor
  //---------------------------------------------------------------------------
  explicit vtol_two_chain(face_list const&, bool new_is_cycle=false);

  //---------------------------------------------------------------------------
  //: Constructor
  //---------------------------------------------------------------------------
  vtol_two_chain(face_list const&,
                 vcl_vector<signed char> const&,
                 bool new_is_cycle=false);

  //---------------------------------------------------------------------------
  //: Pseudo copy constructor.  Deep copy.
  //---------------------------------------------------------------------------
  vtol_two_chain(vtol_two_chain_sptr const& other);
 private:
  //---------------------------------------------------------------------------
  //: Copy constructor.  Deep copy.  Deprecated.
  //---------------------------------------------------------------------------
  vtol_two_chain(vtol_two_chain const& other);
 public:
  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_two_chain();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_sptr clone(void) const;

  virtual vtol_two_chain * copy_with_arrays(topology_list &verts,
                                            topology_list &edges) const;
  // Accessors

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_type topology_type(void) const {return TWOCHAIN;}

  //: get the direction of the face
  signed char direction(vtol_face const& f) const;

  virtual vtol_face_sptr face(int i);

  //---------------------------------------------------------------------------
  //: Shallow copy with no links
  //---------------------------------------------------------------------------
  virtual vtol_topology_object *shallow_copy_with_no_links(void) const;

  virtual void add_superiors_from_parent(topology_list &);
  virtual void remove_superiors_of_parent(topology_list &);
  virtual void remove_superiors(void);
  virtual void update_superior_list_p_from_hierarchy_parent(void);

  virtual void add_face(vtol_face_sptr const&, signed char);
  virtual void remove_face(vtol_face_sptr const&);
 private:
  // Deprecated:
  virtual void add_face(vtol_face &,signed char);
  virtual void remove_face(vtol_face &);
 public:
  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a two_chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_two_chain *cast_to_two_chain(void) const { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a two_chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_two_chain *cast_to_two_chain(void) { return this; }

  //***************************************************************************
  // Status report
  //***************************************************************************

  void link_inferior(vtol_face_sptr inf);
  void unlink_inferior(vtol_face_sptr inf);

  //---------------------------------------------------------------------------
  //: Is `inferior' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool valid_inferior_type(vtol_topology_object const* inferior) const
  { return inferior->cast_to_face()!=0; }
  bool valid_inferior_type(vtol_face_sptr const& )    const { return true; }
  bool valid_inferior_type(vtol_face_2d_sptr const& ) const { return true; }
  bool valid_superior_type(vtol_block_sptr const& )   const { return true; }

  //---------------------------------------------------------------------------
  //: Is `chain_inf_sup' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool valid_chain_type(vtol_chain_sptr chain_inf_sup) const
  { return chain_inf_sup->cast_to_two_chain()!=0; }
  bool valid_chain_type(vtol_two_chain_sptr const& ) const { return true; }

  // network access methods

  virtual vertex_list *outside_boundary_vertices(void);
  virtual zero_chain_list *outside_boundary_zero_chains(void);
  virtual edge_list *outside_boundary_edges(void);
  virtual one_chain_list *outside_boundary_one_chains(void);
  virtual face_list *outside_boundary_faces(void);
  virtual two_chain_list *outside_boundary_two_chains(void);

  // The returned pointers must be deleted after use.
  virtual two_chain_list *inferior_two_chains(void);
  // The returned pointers must be deleted after use.
  virtual two_chain_list *superior_two_chains(void);

 protected:
  // \warning these methods should not be used by clients
  // The returned pointers must be deleted after use.

  virtual vcl_vector<vtol_vertex*> *compute_vertices(void);
  virtual vcl_vector<vtol_edge*> *compute_edges(void);
  virtual vcl_vector<vtol_zero_chain*> *compute_zero_chains(void);
  virtual vcl_vector<vtol_one_chain*> *compute_one_chains(void);
  virtual vcl_vector<vtol_face*> *compute_faces(void);
  virtual vcl_vector<vtol_two_chain*> *compute_two_chains(void);
  virtual vcl_vector<vtol_block*> *compute_blocks(void);

 public:
  virtual vcl_vector<vtol_vertex*> *outside_boundary_compute_vertices(void);
  virtual vcl_vector<vtol_zero_chain*> *outside_boundary_compute_zero_chains(void);
  virtual vcl_vector<vtol_edge*> *outside_boundary_compute_edges(void);
  virtual vcl_vector<vtol_one_chain*> *outside_boundary_compute_one_chains(void);
  virtual vcl_vector<vtol_face*> *outside_boundary_compute_faces(void);
  virtual vcl_vector<vtol_two_chain*> *outside_boundary_compute_two_chains(void);

  int num_faces(void) const { return numinf(); }

  virtual void correct_chain_directions(void);

  virtual bool operator==(vtol_two_chain const& other) const;
  inline bool operator!=(const vtol_two_chain &other)const{return !operator==(other);}
  bool operator==(vsol_spatial_object_2d const& obj) const; // virtual of vsol_spatial_object_2d

  virtual void print(vcl_ostream &strm=vcl_cout) const;
  virtual void describe_directions(vcl_ostream &strm=vcl_cout, int blanking=0) const;
  virtual void describe(vcl_ostream &strm=vcl_cout, int blanking=0) const;

  virtual bool break_into_connected_components(topology_list &components);
};

#endif // vtol_two_chain_h_
