#ifndef vtol_two_chain_3d_H
#define vtol_two_chain_3d_H
//-----------------------------------------------------------------------------
//
// .NAME        vtol_two_chain_3d - Represents a set of vtol_face_3d
// .LIBRARY     vtol
// .HEADER      vxl  package
// .INCLUDE     vtol/vtol_two_chain_3d.h
// .FILE        vtol_two_chain_3d.cxx
//
// .SECTION Description
//  The vtol_two_chain_3d class is used to represent a set of vtol_face_3ds on a topological
//  structure.  A vtol_two_chain_3d consists of its inferior onechains and the superiors
//  on which it lies.  A vtol_two_chain_3d may or may not be an ordered cycle.  If
//  the chain of vtol_face_3d encloses a volume, then the vtol_two_chain_3d may be used as
//  the boundary of a topological vtol_block_3d in a 3D structure.
//
// .SECTION Author
//     Patricia A. Vrobel
//
// .SECTION Modifications:
//               JLM December 1995, Added timeStamp (Touch) to
//                   operations which affect bounds.
//               JLM December 1995, no local method for ComputeBoundingBox
//                   Should use vtol_face_3d geometry recursively to be proper.
//                   Currently reverts to bounds on vertices from
//                   vtol_topology_object_3d::ComputeBoundingBox()
//               PTU ported to vxl May 2000  
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vtol/vtol_topology_object_3d.h>
#include <vtol/vtol_hierarchy_node_3d.h>


class vtol_two_chain_3d : public vtol_topology_object_3d , public vtol_hierarchy_node_3d
{
protected:

  bool _cycle_p;
  vcl_vector<signed char> _directions;

public:

  // Constructors and Destructors

  vtol_two_chain_3d();
  vtol_two_chain_3d(int num_face);
  vtol_two_chain_3d (vcl_vector<vtol_face_3d*>&, bool cyc = false);
  vtol_two_chain_3d (vcl_vector<vtol_face_3d*>&, vcl_vector<signed char>&, bool cyc = false);
  vtol_two_chain_3d (vtol_two_chain_3d const&);
  ~vtol_two_chain_3d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //: See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d_ref clone(void) const;

  vtol_two_chain_3d* copy_with_arrays(vcl_vector<vtol_topology_object_3d*>& verts,
                           vcl_vector<vtol_topology_object_3d*>& edges);
  // Accessors

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_3d_type topology_type(void) const;

  bool cycle() const { return _cycle_p;}
  void set_cycle(bool val) { _cycle_p = val; }
  vcl_vector<signed char>* dirs() {return &_directions;}
  int dir(int i) const {return (int)_directions[i];}

  bool get_cycle_p() const { return _cycle_p;}
  void set_cycle_p(bool val) { _cycle_p = val; }
  vcl_vector<signed char>* get_directions() {return &_directions;}
  
  vtol_face_3d* face(int i) { return (vtol_face_3d *)(_inferiors[i]);}
  vtol_face_3d* get_face(int i) { return (vtol_face_3d *)(_inferiors[i]);}

  // Editing Functions
  vtol_two_chain_3d *copy() { return new vtol_two_chain_3d(*this); }
  virtual vtol_topology_object_3d * shallow_copy_with_no_links( void );
  virtual vsol_spatial_object_3d *spatial_copy() { return this->copy(); }

  virtual void add_superiors_from_parent(vcl_vector<vtol_topology_object_3d*>&);
  virtual void remove_superiors_of_parent(vcl_vector<vtol_topology_object_3d*>&);
  virtual void remove_superiors();
  virtual void update_superior_list_p_from_hierarchy_parent();
  virtual void add_inferior_two_chain(vtol_two_chain_3d*);
  virtual void remove_inferior_two_chain(vtol_two_chain_3d*);

  virtual bool add_face(vtol_face_3d*, signed char) ;
  virtual bool remove_face(vtol_face_3d*) ;
  virtual bool add_part(vtol_block_3d*) ;
  virtual bool add_block(vtol_block_3d*) ;
  virtual bool remove_part(vtol_block_3d*) ;
  virtual bool remove_block(vtol_block_3d*) ;
  inline bool contains_sub_chains() { return (_hierarchy_inferiors.size()>0); }
  inline bool is_sub_chain() { return ( _hierarchy_superiors.size() > 0 ); }

  vtol_two_chain_3d * cast_to_two_chain_3d() { return this; }
  vcl_vector<vtol_vertex_3d*>* outside_boundary_vertices();
  vcl_vector<vtol_vertex_3d*>* vertices ();
  vcl_vector<vtol_zero_chain_3d*>* outside_boundary_zero_chains();
  vcl_vector<vtol_zero_chain_3d*>* zero_chains();
  vcl_vector<vtol_edge_3d*>* outside_boundary_edges();
  vcl_vector<vtol_edge_3d*>* edges();
  vcl_vector<vtol_one_chain_3d*>* outside_boundary_one_chains();
  vcl_vector<vtol_one_chain_3d*>* one_chains();
  vcl_vector<vtol_face_3d*>* outside_boundary_faces();
  vcl_vector<vtol_face_3d*>* faces();
  vcl_vector<vtol_two_chain_3d*>* two_chains();
  vcl_vector<vtol_two_chain_3d*>* inferior_two_chains();
  vcl_vector<vtol_two_chain_3d*>* superior_two_chains();
  vcl_vector<vtol_two_chain_3d*>* outside_boundary_two_chains();
  vcl_vector<vtol_block_3d*>* blocks();
  virtual int num_faces() { return _inferiors.size();}
  virtual void clear();

  // virtual void Correctvtol_face_3dNormals(CoolVector<float> &pt, bool outer = true);
  virtual void correct_chain_directions();
  // virtual vtol_face_3d * FindClosestvtol_face_3d(CoolVector<float> &);

  bool operator==(const vtol_two_chain_3d&) const;
  bool operator==(const vsol_spatial_object_3d& obj) const; // virtual of vsol_spatial_object_3d

  void print (vcl_ostream& strm =vcl_cout) const;
  void describe_directions(vcl_ostream& strm = vcl_cout, int blanking = 0) const;
  void describe(vcl_ostream& strm = vcl_cout, int blanking = 0) const;

  bool remove( vtol_face_3d * vtol_face_3d,
               vcl_vector< vtol_topology_object_3d * > & changes,
               vcl_vector< vtol_topology_object_3d * > & deleted );
  void deep_remove( vcl_vector< vtol_topology_object_3d * > & removed );

  virtual bool disconnect( vcl_vector< vtol_topology_object_3d * > & changes,
                           vcl_vector< vtol_topology_object_3d * > & deleted );
  bool  break_into_connected_components( vcl_vector<vtol_topology_object_3d*> & components );

};

#endif   // vtol_two_chain_3d.h
