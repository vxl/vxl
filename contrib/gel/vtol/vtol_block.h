#ifndef vtol_block_h
#define vtol_block_h
//-----------------------------------------------------------------------------
//:
// \file
// \brief Highest node in a 3D topological structure
//
//   The vtol_block class is used to represent the highest node in a 3D topological
//   structure. A vtol_block consists of its inferior vtol_two_chains which must be
//   cyclic, and no topological superiors.
//
// \author
//     Patricia A. Vrobel
//
// \verbatim
// Modifications:
//     JLM Dec 1995, Added timeStamp (Touch) to operations which affect bounds.
//     JLM Dec 1995  (Need to decide proper policy for curved volumes
//                   and possibly inconsistent polyhedra volume geometry)
//     JNS Dec 1998, Added is_within functions (used in 2D display of a block,
//                   these functions must be wrong for 3D use)
//     \endverbatim
//-----------------------------------------------------------------------------
#include <vtol/vtol_block_sptr.h>

#include <vtol/vtol_topology_object.h>

class vtol_vertex;
class vtol_edge;
class vtol_zero_chain;
class vtol_one_chain;
class vtol_face;
class vtol_two_chain;

class vtol_block
  : public vtol_topology_object
{
public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default constructor
  //---------------------------------------------------------------------------
  explicit vtol_block(void);

  //---------------------------------------------------------------------------
  //: Constructor from a two-chain (inferior)
  //---------------------------------------------------------------------------
  explicit vtol_block(vtol_two_chain &faceloop);

  //---------------------------------------------------------------------------
  //: Constructor from a list of two-chains.
  //  The first two-chain is the outside boundary.
  //  The remaining two-chains are considered holes inside the outside volume.
  //---------------------------------------------------------------------------
  explicit vtol_block(two_chain_list &faceloops);

  //---------------------------------------------------------------------------
  //: Constructor from a list of faces
  //---------------------------------------------------------------------------
  explicit vtol_block(face_list &new_face_list);

  //---------------------------------------------------------------------------
  //: Copy constructor. Deep copy.
  //---------------------------------------------------------------------------
  vtol_block(const vtol_block &other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_block();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_sptr clone(void) const;

  // Accessors

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_type topology_type(void) const;

  virtual vtol_two_chain *get_boundary_cycle(void)
  {
    return
      (_inferiors.size() > 0)
      ? (vtol_two_chain*)(_inferiors[0].ptr())
      : 0;
  }

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a block, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_block *cast_to_block(void) const { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a block, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_block *cast_to_block(void) { return this; }

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `inferior' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool
  valid_inferior_type(const vtol_topology_object &inferior) const;

  //---------------------------------------------------------------------------
  //: Is `superior' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool
  valid_superior_type(const vtol_topology_object &superior) const;

  virtual vertex_list *outside_boundary_vertices(void);
  virtual zero_chain_list *outside_boundary_zero_chains(void);
  virtual edge_list *outside_boundary_edges(void);
  virtual one_chain_list *outside_boundary_one_chains(void);
  virtual face_list *outside_boundary_faces(void);
  virtual two_chain_list *outside_boundary_two_chains(void);

 // WARNING these methods should not be used by clients

  virtual vcl_vector<vtol_vertex *> *compute_vertices(void);
  virtual vcl_vector<vtol_edge *> *compute_edges(void);
  virtual vcl_vector<vtol_zero_chain *> *compute_zero_chains(void);
  virtual vcl_vector<vtol_one_chain *> *compute_one_chains(void);
  virtual vcl_vector<vtol_face *> *compute_faces(void);
  virtual vcl_vector<vtol_two_chain *> *compute_two_chains(void);
  virtual vcl_vector<vtol_block *> *compute_blocks(void);

  virtual
  vcl_vector<vtol_vertex *> *outside_boundary_compute_vertices(void);
  virtual
  vcl_vector<vtol_zero_chain *> *outside_boundary_compute_zero_chains(void);
  virtual vcl_vector<vtol_edge *> *outside_boundary_compute_edges(void);
  virtual
  vcl_vector<vtol_one_chain *> *outside_boundary_compute_one_chains(void);
  virtual vcl_vector<vtol_face *> *outside_boundary_compute_faces(void);
  virtual
  vcl_vector<vtol_two_chain *> *outside_boundary_compute_two_chains(void);

  virtual two_chain_list *hole_cycles(void) const;

  //: Utility and Editing Functions
  virtual bool add_hole_cycle(vtol_two_chain &new_hole);

  //  virtual void update(void);
  virtual bool operator==(const vtol_block &other) const;
  inline bool operator!=(const vtol_block &other)const{return !operator==(other);}
  virtual bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

  //: Printing Functions
  virtual void print(vcl_ostream &strm=vcl_cout) const;
  virtual void describe(vcl_ostream &strm=vcl_cout,
                        int blanking=0) const;
};

#endif // vtol_block_h
