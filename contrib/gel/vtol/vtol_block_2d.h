#ifndef vtol_block_2d_H
#define vtol_block_2d_H
//-----------------------------------------------------------------------------
//
// .NAME        vtol_block_2d - Highest node in a 3D topological structure
// .LIBRARY     vtol
// .HEADER      vxl package
// .INCLUDE     vtol/vtol_block_2d.h
// .FILE        vtol_block_2d.cxx
//
// .SECTION Description
//   The vtol_block_2d class is used to represent the highest node in a 3D topological
//   structure. A vtol_block_2d consists of its inferior vtol_two_chain_2ds which must be
//   cyclic, and no topological superiors.
//
// .SECTION Author
//     Patricia A. Vrobel
//
// .SECTION Modifications:
//     JLM Dec 1995, Added timeStamp (Touch) to operations which affect bounds.
//     JLM Dec 1995  (Need to decide proper policy for curved volumes
//                   and possibly inconsistent polyhedra volume geometry)
//     JNS Dec 1998, Added is_within functions (used in 2D display of a block,
//                   these functions must be wrong for 3D use)
//-----------------------------------------------------------------------------
#include <vtol/vtol_block_2d_ref.h>

#include <vtol/vtol_topology_object_2d.h>

class vtol_two_chain_2d;
class vsol_point_2d;
class vtol_vertex_2d;
class vsol_box_2d;

class vtol_block_2d
  : public vtol_topology_object_2d
{
public:
  //***************************************************************************
  // Initialization
  //***************************************************************************
  
  //---------------------------------------------------------------------------
  //: Default constructor
  //---------------------------------------------------------------------------
  explicit vtol_block_2d(void);

  //---------------------------------------------------------------------------
  //: Constructor from a two-chain (inferior)
  //---------------------------------------------------------------------------
  explicit vtol_block_2d(vtol_two_chain_2d &faceloop);

  //---------------------------------------------------------------------------
  //: Constructor from a list of two-chains. The first two-chain is the
  //: outside boundary. The remaining two-chains are considered holes
  //: inside the the outside volume.
  //---------------------------------------------------------------------------
  explicit vtol_block_2d(two_chain_list_2d &faceloops);

  //---------------------------------------------------------------------------
  //: Constructor from a list of faces
  //---------------------------------------------------------------------------
  explicit vtol_block_2d(face_list_2d &new_face_list);

  //---------------------------------------------------------------------------
  //: Copy constructor. Deep copy.
  //---------------------------------------------------------------------------
  vtol_block_2d(const vtol_block_2d &other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_block_2d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //: See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_ref clone(void) const;

  //: Accessors

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_2d_type topology_type(void) const;
  
  virtual vtol_two_chain_2d *get_boundary_cycle(void)
  {
    return
      (_inferiors.size() > 0)
      ? (vtol_two_chain_2d*)(_inferiors[0].ptr())
      : 0;
  }

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a block, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_block_2d *cast_to_block(void) const;
  
  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a block, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_block_2d *cast_to_block(void);

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

  virtual vertex_list_2d *outside_boundary_vertices(void);
  virtual zero_chain_list_2d *outside_boundary_zero_chains(void);
  virtual edge_list_2d *outside_boundary_edges(void);
  virtual one_chain_list_2d *outside_boundary_one_chains(void);
  virtual face_list_2d *outside_boundary_faces(void);
  virtual two_chain_list_2d *outside_boundary_two_chains(void);

 //: Warning these methods should not be used by clients 

  virtual vcl_vector<vtol_vertex_2d *> *compute_vertices(void);
  virtual vcl_vector<vtol_edge_2d *> *compute_edges(void);
  virtual vcl_vector<vtol_zero_chain_2d *> *compute_zero_chains(void);
  virtual vcl_vector<vtol_one_chain_2d *> *compute_one_chains(void);
  virtual vcl_vector<vtol_face_2d *> *compute_faces(void);
  virtual vcl_vector<vtol_two_chain_2d *> *compute_two_chains(void);
  virtual vcl_vector<vtol_block_2d *> *compute_blocks(void);

  virtual
  vcl_vector<vtol_vertex_2d *> *outside_boundary_compute_vertices(void);
  virtual
  vcl_vector<vtol_zero_chain_2d *> *outside_boundary_compute_zero_chains(void);
  virtual vcl_vector<vtol_edge_2d *> *outside_boundary_compute_edges(void);
  virtual
  vcl_vector<vtol_one_chain_2d *> *outside_boundary_compute_one_chains(void);
  virtual vcl_vector<vtol_face_2d *> *outside_boundary_compute_faces(void);
  virtual
  vcl_vector<vtol_two_chain_2d *> *outside_boundary_compute_two_chains(void);

  virtual two_chain_list_2d *hole_cycles(void) const;
  
  //: Utility and Editing Functions
  virtual bool add_hole_cycle(vtol_two_chain_2d &new_hole);

  //  virtual void update(void);
  virtual bool operator==(const vtol_block_2d &other) const;
  virtual bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

  //: Printing Functions
  virtual void print(ostream &strm=cout) const;
  virtual void describe(ostream &strm=cout,
                        int blanking=0) const;
};
#endif
