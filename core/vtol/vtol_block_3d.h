#ifndef vtol_block_3d_H
#define vtol_block_3d_H
//-----------------------------------------------------------------------------
//
// .NAME        vtol_block_3d - Highest node in a 3D topological structure
// .LIBRARY     vtol
// .HEADER      vxl package
// .INCLUDE     vtol/vtol_block_3d.h
// .FILE        vtol_block_3d.h
// .FILE        vtol_block_3d.cxx
//
// .SECTION Description
//   The vtol_block_3d class is used to represent the highest node in a 3D topological
//   structure. A vtol_block_3d consists of its inferior vtol_two_chain_3ds which must be
//   cyclic, and no topological superiors.
//
// .SECTION Author
//     Patricia A. Vrobel
//
// .SECTION Modifications:
//     JLM Dec 1995, Added timeStamp (Touch) to operations which affect bounds.
//     JLM Dec 1995  (Need to decide proper policy for curved volumes
//                   and possibly inconsistent polyhedra volume geometry)
//     JNS Dec 1998, Added is_within functions (used in 3D display of a block,
//                   these functions must be wrong for 3D use)
//-----------------------------------------------------------------------------



#include <vtol/vtol_topology_object_3d.h>

class vtol_two_chain_3d;
class vsol_point_3d;
class vtol_vertex_3d;
class vsol_box_3d;

class vtol_block_3d : public vtol_topology_object_3d
{
public:

  //: Constructors and Destructors
  vtol_block_3d();
  vtol_block_3d(vtol_block_3d const& );
  vtol_block_3d(vsol_box_3d&); // from bound of a box
  vtol_block_3d(vtol_two_chain_3d*);
  vtol_block_3d(vcl_vector<vtol_face_3d*>&);
  vtol_block_3d(vcl_vector<vtol_two_chain_3d*>&);
  ~vtol_block_3d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //: See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d_ref clone(void) const;

  //: Accessors

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_3d_type topology_type(void) const;
  
  vtol_two_chain_3d *get_boundary_cycle() { if (_inferiors.size() > 0)
                                   return (vtol_two_chain_3d*) _inferiors[0];
                                  else return NULL;
                               }
  vtol_block_3d * cast_to_block_3d() { return this; }
  vcl_vector<vtol_vertex_3d*>* outside_boundary_vertices ();
  vcl_vector<vtol_vertex_3d*>* vertices ();
  vcl_vector<vtol_zero_chain_3d*>* outside_boundary_zero_chains();
  vcl_vector<vtol_zero_chain_3d*>* zero_chains();
  vcl_vector<vtol_edge_3d*>* outside_boundary_edges();
  vcl_vector<vtol_edge_3d*>* edges();
  vcl_vector<vtol_one_chain_3d*>* outside_boundary_one_chains();
  vcl_vector<vtol_one_chain_3d*>* one_chains();
  vcl_vector<vtol_face_3d*>* outside_boundary_faces();
  vcl_vector<vtol_face_3d*>* faces();
  vcl_vector<vtol_two_chain_3d*>* outside_boundary_two_chains();
  vcl_vector<vtol_two_chain_3d*>* two_chains();
  vcl_vector<vtol_block_3d*>* blocks();
  vcl_vector< vtol_two_chain_3d* > * get_hole_cycles();
  
  //: Utility and Editing Functions
  bool add_hole_cycle( vtol_two_chain_3d * new_hole );
  bool add_face_loop(vtol_two_chain_3d*) ;
  bool add_two_chain(vtol_two_chain_3d*) ;
  bool remove_face_loop(vtol_two_chain_3d*) ;
  bool remove_two_chain(vtol_two_chain_3d*) ;
  // void correct_face_normals(vcl_vector<float> &);
  // void correct_chain_directions();

  void update();
  vtol_block_3d* copy(); // deep copy
  virtual vtol_topology_object_3d * shallow_copy_with_no_links( void );
  virtual vsol_spatial_object_3d* spatial_copy() { return this->copy(); }

  bool operator==(const vsol_spatial_object_3d&) const;
  bool operator== (const vtol_block_3d& obj) const;

  //: Printing Functions
  void print (ostream& strm =cout);
  void describe_directions(ostream& strm = cout, int blanking = 0);
  void describe(ostream& strm = cout, int blanking = 0);

  bool remove( vtol_two_chain_3d * two_chain,
               vcl_vector< vtol_topology_object_3d * > & changes,
               vcl_vector< vtol_topology_object_3d * > & deleted );
  void deep_remove( vcl_vector< vtol_topology_object_3d * > & removed );

  virtual bool disconnect( vcl_vector< vtol_topology_object_3d * > & changes,
                           vcl_vector< vtol_topology_object_3d * > & deleted );
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vtol_block_3d.

