// This is gel/vtol/vtol_face.h
#ifndef vtol_face_h_
#define vtol_face_h_
//:
// \file
// \brief Represents the basic 2D topological entity
//
//  The vtol_face class is used to represent a topological face.
//  The connectivity between faces may be obtained from the superior 2-chains
//  of the face. The boundary of the face may be obtained from the inferior
//  1-chains of the face.
//
// \verbatim
//  Modifications:
//   JLM Dec 1995: Added timeStamp (Touch) to
//       operations which affect bounds.
//   JLM Dec 1995: Added method for ComputeBoundingBox
//       (Need to decide proper policy for curved surfaces
//       and possibly inconsistent planar surface geometry)
//   JSL Computed Area()
//   JLM Sep 1996: Fixed the face copy constructor which
//       did not handle the construction of new vtol_edge(s) properly.
//       The old implementation always constructed ImplicitLine(s)
//       for the curve of each new edge.  See vtol_edge.h for the required
//       alterations of the vtol_edge constructors.  There is still an
//       issue with proper copying of the vtol_face's Surface.  It isn't
//       done correctly.
//   PVR Aug 97: is_within_projection() implementation restored.
//   AWF Jul 1998: General topology speedup by replacing calls to
//       vertices() et al with iterators.  Benchmark: constructing
//       40K triangles, old: 37 sec, new: 9 sec.
//   PTU may-2000 ported to vxl
//   Dec. 2002, Peter Vanroose -interface change: vtol objects -> smart pointers
//   9 Jan. 2003, Peter Vanroose - added pure virtual "copy_geometry()"
//   5 Feb. 2003, Peter Vanroose - moved IsHoleP() here from vtol_intensity_face
// \endverbatim

#include <vcl_iosfwd.h>
#include <vcl_vector.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_two_chain.h>
class vtol_vertex;
class vtol_edge;
class vtol_face_2d;
class vtol_one_chain;
class vtol_two_chain;

class vtol_face : public vtol_topology_object
{
 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default constructor
  //---------------------------------------------------------------------------
  vtol_face(void) {}

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_face();

  // Accessors

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_type topology_type(void) const { return FACE; }

  virtual vtol_one_chain_sptr get_one_chain(int which = 0);
  virtual vtol_one_chain_sptr get_boundary_cycle(void);
  virtual bool add_hole_cycle(vtol_one_chain_sptr new_hole);
  virtual one_chain_list *get_hole_cycles(void);

  // Methods that are here for now in transition.. :x

  //: Inferior/Superior Accessor Methods
  virtual vtol_face * copy_with_arrays(topology_list &verts, topology_list &edges) const =0;

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a face, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_face *cast_to_face(void) const { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a face, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_face *cast_to_face(void) { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a face, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_face_2d *cast_to_face_2d(void) const {return 0;}

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a face, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_face_2d *cast_to_face_2d(void) {return 0;}

  //***************************************************************************
  // Status report
  //***************************************************************************

  void link_inferior(vtol_one_chain_sptr inf);
  void unlink_inferior(vtol_one_chain_sptr inf);

  //---------------------------------------------------------------------------
  //: Is `inferior' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool valid_inferior_type(vtol_topology_object const* inferior) const
  { return inferior->cast_to_one_chain()!=0; }
  bool valid_inferior_type(vtol_one_chain_sptr const& ) const { return true; }
  bool valid_superior_type(vtol_two_chain_sptr const& ) const { return true; }

  //: accessors for boundary elements
  virtual vertex_list *outside_boundary_vertices(void);
  virtual zero_chain_list *outside_boundary_zero_chains(void);
  virtual edge_list *outside_boundary_edges(void);
  virtual one_chain_list *outside_boundary_one_chains(void);
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

  virtual vcl_vector<vtol_vertex*> *outside_boundary_compute_vertices(void);
  virtual vcl_vector<vtol_zero_chain*> *outside_boundary_compute_zero_chains(void);
  virtual vcl_vector<vtol_edge*> *outside_boundary_compute_edges(void);
  virtual vcl_vector<vtol_one_chain*> *outside_boundary_compute_one_chains(void);
 public:

  // Editing Functions

  virtual void add_one_chain(vtol_one_chain_sptr const&);
 private:
  // Deprecated:
  virtual void add_one_chain(vtol_one_chain &);
 public:
  //: Utility Functions and overloaded operators

  virtual void reverse_normal(void);
  virtual int get_num_edges(void) const;

  //: This method determines if a vtol_face is a hole of another vtol_face.
  bool IsHoleP() const;

  //---------------------------------------------------------------------------
  //: Copy with no links. Only copy the surface if it exists
  //---------------------------------------------------------------------------
  virtual vtol_face *shallow_copy_with_no_links(void) const =0;

  virtual bool operator==(const vtol_face &other)const;
  inline bool operator!=(const vtol_face &other)const{return !operator==(other);}
  bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

  //: determine bounding box from bounding boxes of underlying edges
  virtual void compute_bounding_box(void) const;

  virtual void print(vcl_ostream &strm=vcl_cout) const;

  virtual void describe(vcl_ostream &strm=vcl_cout,
                        int blanking=0) const;
  //---------------------------------------------------------------------------
  //: Does `this' share an edge with `f' ?
  // Comparison of edge pointers, not geometric values
  //---------------------------------------------------------------------------
  virtual bool shares_edge_with(vtol_face &f);

  //: have the inherited classes copy the geometry
  virtual void copy_geometry(const vtol_face &other)=0;

  //: compare the geometry
  virtual bool compare_geometry(const vtol_face &other) const =0;
};

#endif // vtol_face_h_
