#ifndef vtol_two_chain_2d_H
#define vtol_two_chain_2d_H
//-----------------------------------------------------------------------------
//
// .NAME        vtol_two_chain_2d - Represents a set of vtol_face_2d
// .LIBRARY     vtol
// .HEADER      vxl  package
// .INCLUDE     vtol/vtol_two_chain_2d.h
// .FILE        vtol_two_chain_2d.cxx
//
// .SECTION Description
//  The vtol_two_chain_2d class is used to represent a set of vtol_face_2ds on a topological
//  structure.  A vtol_two_chain_2d consists of its inferior onechains and the superiors
//  on which it lies.  A vtol_two_chain_2d may or may not be an ordered cycle.  If
//  the chain of vtol_face_2d encloses a volume, then the vtol_two_chain_2d may be used as
//  the boundary of a topological vtol_block_2d in a 3D structure.
//
// .SECTION Author
//     Patricia A. Vrobel
//
// .SECTION Modifications:
//               JLM December 1995, Added timeStamp (Touch) to
//                   operations which affect bounds.
//               JLM December 1995, no local method for ComputeBoundingBox
//                   Should use vtol_face_2d geometry recursively to be proper.
//                   Currently reverts to bounds on vertices from
//                   vtol_topology_object_2d::ComputeBoundingBox()
//               PTU ported to vxl May 2000  
//-----------------------------------------------------------------------------
#include <vtol/vtol_two_chain_2d_ref.h>

#include <vcl_vector.h>
//#include <vtol/vtol_topology_object_2d.h>
//#include <vtol/vtol_hierarchy_node_2d.h>
#include <vtol/vtol_chain_2d.h>

class vtol_two_chain_2d
//: public vtol_topology_object_2d,
  : public vtol_chain_2d
{
public:
  //***************************************************************************
  // Initialization
  //***************************************************************************
  
  //---------------------------------------------------------------------------
  //: Default constructor
  //---------------------------------------------------------------------------
  explicit vtol_two_chain_2d(void);

  //---------------------------------------------------------------------------
  //: Constructor
  //---------------------------------------------------------------------------
  explicit vtol_two_chain_2d(int num_face);

  //---------------------------------------------------------------------------
  //: Constructor
  //---------------------------------------------------------------------------
  explicit vtol_two_chain_2d(face_list_2d &,
                             bool new_is_cycle=false);

  //---------------------------------------------------------------------------
  //: Constructor
  //---------------------------------------------------------------------------
  explicit vtol_two_chain_2d(face_list_2d &,
                             vcl_vector<signed char> &,
                             bool new_is_cycle=false);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vtol_two_chain_2d(const vtol_two_chain_2d &other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_two_chain_2d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //: See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_ref clone(void) const;

  virtual vtol_two_chain_2d *
  copy_with_arrays(vcl_vector<vtol_topology_object_2d_ref> &verts,
                   vcl_vector<vtol_topology_object_2d_ref> &edges) const;
  // Accessors

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_2d_type topology_type(void) const;

  //  vcl_vector<signed char>* dirs() {return &_directions;}

  //  bool get_cycle_p() const { return _cycle_p;}
  //  void set_cycle_p(bool val) { _cycle_p = val; }
  //vcl_vector<signed char>* get_directions() {return &_directions;}
  
  virtual vtol_face_2d *face(int i)
  {
    return (vtol_face_2d *)(_inferiors[i].ptr());
  }
  //  vtol_face_2d* get_face(int i) { return (vtol_face_2d *)(_inferiors[i]);}

  //---------------------------------------------------------------------------
  //: Shallow copy with no links
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_2d *shallow_copy_with_no_links(void) const;

  virtual void add_superiors_from_parent(topology_list_2d &);
  virtual void remove_superiors_of_parent(topology_list_2d &);
  virtual void remove_superiors(void);
  virtual void update_superior_list_p_from_hierarchy_parent(void);

  virtual void add_face(vtol_face_2d &,signed char);
  virtual void remove_face(vtol_face_2d &);
  virtual void add_block(vtol_block_2d &);
  virtual void remove_block(vtol_block_2d &);

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a two_chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_two_chain_2d *cast_to_two_chain(void) const;

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a two_chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_two_chain_2d *cast_to_two_chain(void);

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
  //: Is `chain_inf_sup' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool
  valid_chain_type(const vtol_chain_2d &chain_inf_sup) const;

  //: network access methods

  virtual vertex_list_2d *outside_boundary_vertices(void);
  virtual zero_chain_list_2d *outside_boundary_zero_chains(void);
  virtual edge_list_2d *outside_boundary_edges(void);
  virtual one_chain_list_2d *outside_boundary_one_chains(void);
  virtual face_list_2d *outside_boundary_faces(void);
  virtual two_chain_list_2d *outside_boundary_two_chains(void);

  virtual two_chain_list_2d *inferior_two_chains(void);
  virtual two_chain_list_2d *superior_two_chains(void);

  //: Warning these methods should not be used by clients 

  virtual vcl_vector<vtol_vertex_2d*> *compute_vertices(void);
  virtual vcl_vector<vtol_edge_2d*> *compute_edges(void);
  virtual vcl_vector<vtol_zero_chain_2d*> *compute_zero_chains(void);
  virtual vcl_vector<vtol_one_chain_2d*> *compute_one_chains(void);
  virtual vcl_vector<vtol_face_2d*> *compute_faces(void);
  virtual vcl_vector<vtol_two_chain_2d*> *compute_two_chains(void);
  virtual vcl_vector<vtol_block_2d*> *compute_blocks(void);

  virtual vcl_vector<vtol_vertex_2d*> *outside_boundary_compute_vertices(void);
  virtual vcl_vector<vtol_zero_chain_2d*> *outside_boundary_compute_zero_chains(void);
  virtual vcl_vector<vtol_edge_2d*> *outside_boundary_compute_edges(void);
  virtual vcl_vector<vtol_one_chain_2d*> *outside_boundary_compute_one_chains(void);
  virtual vcl_vector<vtol_face_2d*> *outside_boundary_compute_faces(void);
  virtual vcl_vector<vtol_two_chain_2d*> *outside_boundary_compute_two_chains(void);
  


  




  virtual int num_faces(void) const { return numinf();}
  //virtual void clear(void);

  // virtual void Correctvtol_face_2dNormals(CoolVector<float> &pt, bool outer = true);
  virtual void correct_chain_directions(void);
  // virtual vtol_face_2d * FindClosestvtol_face_2d(CoolVector<float> &);

  virtual bool operator==(const vtol_two_chain_2d &other) const;
  bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

  virtual void print(vcl_ostream &strm=vcl_cout) const;
  virtual void describe_directions(vcl_ostream &strm=vcl_cout,
                                   int blanking=0) const;
  virtual void describe(vcl_ostream &strm=vcl_cout,
                        int blanking=0) const;

  virtual bool break_into_connected_components(vcl_vector<vtol_topology_object_2d *> &components);

};

#endif   // vtol_two_chain_2d.h
