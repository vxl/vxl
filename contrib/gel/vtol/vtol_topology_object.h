// This is gel/vtol/vtol_topology_object.h
#ifndef topology_object_h_
#define topology_object_h_
//:
// \file
// \brief base class for topology objects
// \author Patricia A. Vrobel.

#include <vtol/vtol_topology_object_sptr.h>
#include <vsol/vsol_spatial_object_2d.h> // parent class
#include <vcl_vector.h>
#include <vcl_list.h>
#include <vcl_iosfwd.h>
#include <vtol/vtol_vertex_sptr.h>
#include <vtol/vtol_zero_chain_sptr.h>
#include <vtol/vtol_edge_sptr.h>
#include <vtol/vtol_one_chain_sptr.h>
#include <vtol/vtol_face_sptr.h>
#include <vtol/vtol_two_chain_sptr.h>
#include <vtol/vtol_block_sptr.h>
#include <vtol/vtol_chain_sptr.h>
class vtol_topology_cache;

// Useful typedefs
typedef vcl_vector<vtol_topology_object_sptr> topology_list;
typedef vcl_vector<vtol_vertex_sptr>          vertex_list;
typedef vcl_vector<vtol_zero_chain_sptr>      zero_chain_list;
typedef vcl_vector<vtol_edge_sptr>            edge_list;
typedef vcl_vector<vtol_one_chain_sptr>       one_chain_list;
typedef vcl_vector<vtol_face_sptr>            face_list;
typedef vcl_vector<vtol_two_chain_sptr>       two_chain_list;
typedef vcl_vector<vtol_block_sptr>           block_list;
typedef vcl_vector<vtol_chain_sptr>           chain_list;

//*****************************************************************************
// ALL THE DERIVED AND NON-ABSTRACT CLASSES OF THIS CLASS MUST CALL
// unlink_all_inferiors() IN THEIR DESTRUCTOR
// unlink_all_inferiors() CANT BE CALLED DIRECTLY IN THIS CLASS, OTHERWISE
// BAD VERSIONS OF METHODS SHOULD BE CALLED (C++ IS STUPID !)
//*****************************************************************************

//: Base class for topology objects
// The vtol_topology_object class is the interface base class for all
// topological entities. There are only access methods in this class.
// vtol_topology_object inherits from vtol_spatial_object, which is
// the base class for all topology and geometry.
//
// The topology class hierarchy:
// \verbatim
//                             vtol_topology_object
//                                   |
//             ---------------------------------------------------
//             |        |       |       |       |       |        |
//         vertex  zero_chain  edge  one_chain  face  two_chain  block
//             |                |               |
//        vertex_2d           edge_2d          face_2d
// \endverbatim
//
// (Actually, one_chain and two_chain share a common parent class vtol_chain,
//  but that's just a matter of convenience; conceptually, the scheme is this.)
//
// \verbatim
// Incidence of:
//       (vsol_point)      (vsol_curve)     (vsol_surface)    (vsol_volume)
// Directed
// Sequence of:       vertex           edge             face
//
// \endverbatim
//
// The vertex, edge and face entities account for incidence between
// points, curves and surfaces, respectively. For example, two vtol_edge(s)
// are incident at exactly one vtol_vertex.  That is, an edge does not
// intersect itself or another edge, except possibly at a vertex.
// The chain entities are directed sequences which define composite
// structures. Each element of the chain has a sign associated with
// with it denoting the "direction" of use in a boundary traversal.
// For example, a closed vtol_one_chain (a 1-cycle) forms the boundary of
// a surface region. Two adjacent surface regions (vtol_face(s)) use an
// edge in the opposite sense in their bounding 1-chains.
//
// The data member "inferiors_" stores a list of all topology objects of
// the type "below" the current one, which form the boundary of this one.
// All these objects should have the current object listed in their "superiors_"
// list.  It is the responsibility of the superior to break the link with its
// inferior, not vice versa, hence conceptually the "superiors_" entries are
// under control of the referred superior object.  Otherwise said, an object
// cannot live without its inferiors (and will disappear when its inferiors
// disappear), while an object can live without its superiors (and is just
// notified of their appearance and disappearance by the entries listed in
// its superiors_ list).
//
// As a consequence, the "inferior" does not obtain a new time stamp when its
// superiors change (hence its "superiors_" list is mutable).
// This also explains why the "superiors_" list consists of ordinary pointers,
// not smart pointers:  the refcount toggling is done from superior to
// inferior (which requires its inferiors to be kept alive), not from inferior
// to superior (which need not bother about its superiors' existence).
//
// \author
//    Patricia A. Vrobel.
// \verbatim
//  Modifications
//   ported to vxl by Luis E. Galup
//   JLM November 2002 - added a local bounding box method
//   dec.2002 -Peter Vanroose- added chain_list (typedef) and cast_to_chain()
//   dec.2002 -Peter Vanroose- link_inferior() now takes smart pointer argument
//   sep.2004 -Peter Vanroose- made methods returning the inf_sup cache "const"
// \endverbatim

class vtol_topology_object : public vsol_spatial_object_2d
{
  //***************************************************************************
  // Data members
  //***************************************************************************

  //---------------------------------------------------------------------------
  // Description: cache system
  //---------------------------------------------------------------------------
  mutable vtol_topology_cache *inf_sup_cache_;

 protected:

  //---------------------------------------------------------------------------
  // Description: array of superiors
  //---------------------------------------------------------------------------
  vcl_list<vtol_topology_object*> superiors_;

  //---------------------------------------------------------------------------
  // Description: array of inferiors
  //---------------------------------------------------------------------------
  topology_list inferiors_;

  enum vtol_topology_object_type
  { TOPOLOGY_NO_TYPE=0,
    VERTEX,
    ZEROCHAIN,
    EDGE,
    ONECHAIN,
    FACE,
    TRIFACE,
    INTENSITYFACE,
    INTENSITYFACE3D,
    DDBINTENSITYFACE,
    TWOCHAIN,
    TRIMESHTWOCHAIN,
    BLOCK,
    NUM_TOPOLOGYOBJECT_TYPES
  };
 private: // has been superceeded by is_a()
  //: Return the topology type
  // To be overridden by all subclasses
  virtual vtol_topology_object_type topology_type() const { return TOPOLOGY_NO_TYPE; }

 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default constructor
  //---------------------------------------------------------------------------
  vtol_topology_object();

  //---------------------------------------------------------------------------
  //: Constructor with given sizes for arrays of inferiors and superiors
  //---------------------------------------------------------------------------
  vtol_topology_object(int num_inferiors, int num_superiors);

 protected:
  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_topology_object();

 public:
  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************
  virtual vtol_topology_object *cast_to_topology_object() { return this; }
  virtual const vtol_topology_object*cast_to_topology_object()const{return this;}

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a vertex, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_vertex *cast_to_vertex() const { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a vertex, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_vertex *cast_to_vertex() { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a zero_chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_zero_chain *cast_to_zero_chain() const { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a zero_chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_zero_chain *cast_to_zero_chain() { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an edge, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_edge *cast_to_edge() const { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an edge, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_edge *cast_to_edge() { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_chain *cast_to_chain() const { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_chain *cast_to_chain() { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an one_chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_one_chain *cast_to_one_chain() const { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an one_chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_one_chain *cast_to_one_chain() { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a face, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_face *cast_to_face() const { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a face, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_face *cast_to_face() { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a two_chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_two_chain *cast_to_two_chain() const { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a two_chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_two_chain *cast_to_two_chain() { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a block, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_block *cast_to_block() const { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a block, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_block *cast_to_block() { return 0; }

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `inferior' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool valid_inferior_type(vtol_topology_object const* inf) const = 0;

  //---------------------------------------------------------------------------
  //: Is `superior' type valid for `this' ?
  //---------------------------------------------------------------------------
  inline bool valid_superior_type(vtol_topology_object const* sup) const
  { return sup->valid_inferior_type(this); }

  //---------------------------------------------------------------------------
  //: Is `inferior' already an inferior of `this' ?
  //---------------------------------------------------------------------------
  bool is_inferior(vtol_topology_object_sptr inferior) const;

  //---------------------------------------------------------------------------
  //: Is `superior' already a superior of `this' ?
  //---------------------------------------------------------------------------
  bool is_superior(vtol_topology_object* const& superior) const;

  //---------------------------------------------------------------------------
  //: Number of inferiors
  //---------------------------------------------------------------------------
  int numinf() const { return inferiors()->size(); }

  //---------------------------------------------------------------------------
  //: Number of superiors
  //---------------------------------------------------------------------------
  int numsup() const { return superiors_.size(); }

  //---------------------------------------------------------------------------
  //: Return the superiors list (must be deallocated after use)
  //---------------------------------------------------------------------------
 private:
  const topology_list *superiors() const;
 public:
  const vcl_list<vtol_topology_object*> *superiors_list() const {return &superiors_;}

  //---------------------------------------------------------------------------
  //: Return the inferiors list
  //---------------------------------------------------------------------------
        topology_list *inferiors() { return &inferiors_; }
  const topology_list *inferiors() const { return &inferiors_; }

  //---------------------------------------------------------------------------
  //: Return the spatial type
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_type spatial_type()const{return TOPOLOGYOBJECT;}

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Link `this' with an inferior `inferior'
  //  REQUIRE: valid_inferior_type(inferior) and !is_inferior(inferior)
  //---------------------------------------------------------------------------
  void link_inferior(vtol_topology_object_sptr inferior);

  //---------------------------------------------------------------------------
  //: Unlink `this' from the inferior `inferior'
  //  REQUIRE: valid_inferior_type(inferior) and is_inferior(inferior)
  //---------------------------------------------------------------------------
  void unlink_inferior(vtol_topology_object_sptr inferior);

  //---------------------------------------------------------------------------
  //: Unlink `this' from all its inferiors
  //---------------------------------------------------------------------------
  void unlink_all_inferiors();

  //---------------------------------------------------------------------------
  //: Unlink `this' of the network
  //---------------------------------------------------------------------------
  void unlink();

  //: Get list of vertices
  void vertices(vertex_list &list) const;
  //: Get list of zero chains
  void zero_chains(zero_chain_list &list) const;
  //: Get list of edges
  void edges(edge_list &list) const;
  //: Get list of one chains
  void one_chains(one_chain_list &list) const;
  //: Get list of faces
  void faces(face_list &list) const;
  //: Get list of two chains
  void two_chains(two_chain_list &list) const;
  //: Get list of blocks
  void blocks(block_list &list) const;
 private:
  //---------------------------------------------------------------------------
  //: Get list of vertices
  // returned list must be deleted after use.
  // \deprecated - use vertices(list) instead
  //---------------------------------------------------------------------------
  vertex_list *vertices() const;

  //---------------------------------------------------------------------------
  //: Get list of zero chains
  // returned list must be deleted after use.
  // \deprecated - use zero_chains(list) instead
  //---------------------------------------------------------------------------
  zero_chain_list *zero_chains() const;

  //---------------------------------------------------------------------------
  //: Get list of edges
  // returned list must be deleted after use.
  // \deprecated - use edges(list) instead
  //---------------------------------------------------------------------------
  edge_list *edges() const;

  //---------------------------------------------------------------------------
  //: Get list of one chains
  // returned list must be deleted after use.
  // \deprecated - use one_chains(list) instead
  //---------------------------------------------------------------------------
  one_chain_list *one_chains() const;

  //---------------------------------------------------------------------------
  //: Get list of faces
  // returned list must be deleted after use.
  // \deprecated - use faces(list) instead
  //---------------------------------------------------------------------------
  face_list *faces() const;

  //---------------------------------------------------------------------------
  //: Get list of two chains
  // returned list must be deleted after use.
  // \deprecated - use two_chains(list) instead
  //---------------------------------------------------------------------------
  two_chain_list *two_chains() const;

  //---------------------------------------------------------------------------
  //: Get list of blocks
  // returned list must be deleted after use.
  // \deprecated - use blocks(list) instead
  //---------------------------------------------------------------------------
  block_list *blocks() const;
 public:
  //---------------------------------------------------------------------------
  //: print and describe the objects
  //---------------------------------------------------------------------------
  virtual void print(vcl_ostream &strm=vcl_cout) const;
  void describe_inferiors(vcl_ostream &strm=vcl_cout, int blanking=0) const;
  void describe_superiors(vcl_ostream &strm=vcl_cout, int blanking=0) const;
  virtual void describe(vcl_ostream &strm=vcl_cout, int blanking=0) const;

  virtual void compute_bounding_box() const; //A local implementation

  //---------------------------------------------------------------------------
  //: compute lists of vertices
  // \warning should not be used by clients
  //---------------------------------------------------------------------------
  virtual vcl_vector<vtol_vertex*> *compute_vertices();

  //---------------------------------------------------------------------------
  //: compute lists of zero chains
  // \warning should not be used by clients
  //---------------------------------------------------------------------------
  virtual vcl_vector<vtol_zero_chain*> *compute_zero_chains();

  //---------------------------------------------------------------------------
  //: compute lists of edges
  // \warning should not be used by clients
  //---------------------------------------------------------------------------
  virtual vcl_vector<vtol_edge*> *compute_edges();

  //---------------------------------------------------------------------------
  //: compute lists of one chains
  // \warning should not be used by clients
  //---------------------------------------------------------------------------
  virtual vcl_vector<vtol_one_chain*> *compute_one_chains();

  //---------------------------------------------------------------------------
  //: compute lists of faces
  // \warning should not be used by clients
  //---------------------------------------------------------------------------
  virtual vcl_vector<vtol_face*> *compute_faces();

  //---------------------------------------------------------------------------
  //: compute lists of two chains
  // \warning should not be used by clients
  //---------------------------------------------------------------------------
  virtual vcl_vector<vtol_two_chain*> *compute_two_chains();

  //---------------------------------------------------------------------------
  //: compute lists of blocks
  // \warning should not be used by clients
  //---------------------------------------------------------------------------
  virtual vcl_vector<vtol_block*> *compute_blocks();

 private:
  // declare a friend class
  friend class vtol_topology_cache;
};

#endif // topology_object_h_
