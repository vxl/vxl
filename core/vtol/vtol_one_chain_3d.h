
#ifndef vtol_one_chain_3d_h_
#define vtol_one_chain_3d_h_
//-----------------------------------------------------------------------------
//
// .NAME        vtol_one_chain_3d - Represents a set of edges
// .LIBRARY     vtol
// .HEADER      vxl package
// .INCLUDE     vtol/vtol_one_chain_3d.h
// .FILE        vtol_one_chain_3d.h
// .FILE        vtol_one_chain_3d.cxx
//
// .SECTION Description
//  The vtol_one_chain_3d class is used to represent a set of edges on a topological
//  structure.  A vtol_one_chain_3d consists of its inferior edges and the superiors
//  on which it lies.  A vtol_one_chain_3d may or may not be an ordered cycle.  If
//  the chain of edges encloses an area, then the vtol_one_chain_3d may be used as
//  the boundary of a topological Face in a 3D structure.
//
// .SECTION Author
//     Patricia A. Vrobel
//
// .SECTION Modifications:
//     JLM Dec 1995, Added timeStamp (Touch) to operations which affect bounds.
//     JLM Dec 1995, no local method for ComputeBoundingBox
//                   Should use edge geometry recursively to be proper.
//                   Currently reverts to bounds on vertices from
//                   TopologyObject::ComputeBoundingBox()
//     JLM Jan 1998  Added method to get direction of an edge
//     JLM Feb 1999  Added correct method for ComputeBoundingBox()
//     PTU May 2000  ported to vxl
//-----------------------------------------------------------------------------



#include <vtol/vtol_topology_object_3d.h>
#include <vtol/vtol_hierarchy_node_3d.h>
#include <vcl/vcl_vector.h>

class vtol_edge_3d;
class vtol_vertex_3d;
class vtol_face_3d;
class vtol_block_3d;
class vtol_zero_chain_3d;
class vtol_two_chain_3d;

//: 
// \verbatim
// The class represents a collection of edges and orientations
// \endverbatim


class vtol_one_chain_3d : public vtol_topology_object_3d, public vtol_hierarchy_node_3d 
{
protected:

  bool _cycle_p;
  vcl_vector<signed char> _directions;

public:

  // Constructors and Destructors...
  
  //: \brief Contstructors

  vtol_one_chain_3d();
  vtol_one_chain_3d(vcl_vector<vtol_edge_3d*>&, bool iscycle = false);
  vtol_one_chain_3d(vcl_vector<vtol_edge_3d*>&, vcl_vector<signed char>&, bool iscycle = false);
  vtol_one_chain_3d(vtol_one_chain_3d const&);

  ~vtol_one_chain_3d();
  
  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //: See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d_ref clone(void) const;

  // Easy access methods


  //: \brief Access methods

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_3d_type topology_type(void) const;

  inline bool get_cycle_p()  { return _cycle_p; }
  inline vcl_vector<signed char> *get_directions()  { return &_directions; }
  inline const vcl_vector<signed char> *get_directions() const  { return &_directions; }
  signed char get_direction(vtol_edge_3d* e);
  inline void set_cycle_p(bool cyc)  { _cycle_p = cyc; }

  vtol_one_chain_3d * cast_to_one_chain_3d() { return this; }
  
  vcl_vector<vtol_vertex_3d*>* outside_boundary_vertices();
  vcl_vector<vtol_vertex_3d*>* vertices();
  
  vcl_vector<vtol_zero_chain_3d*>* outside_boundary_zero_chains();
  vcl_vector<vtol_zero_chain_3d*>* zero_chains();
  vcl_vector<vtol_edge_3d*>* outside_boundary_edges();
  vcl_vector<vtol_edge_3d*>* edges();
  vcl_vector<vtol_one_chain_3d*>* one_chains();
  vcl_vector<vtol_one_chain_3d*>* inferior_one_chains();
  vcl_vector<vtol_one_chain_3d*>* superior_one_chains();
  vcl_vector<vtol_one_chain_3d*>* outside_boundary_one_chains();
  vcl_vector<vtol_face_3d*>* faces();
  vcl_vector<vtol_two_chain_3d*>* two_chains();
  vcl_vector<vtol_block_3d*>* blocks();

  // Utilities
  //: \brief Utilitites

  void reverse_directions();

  vtol_one_chain_3d* copy_with_arrays(vcl_vector<vtol_topology_object_3d*>& verts,
				    vcl_vector<vtol_topology_object_3d*>& edges);
  
  virtual void compute_bounding_box(); //A local implementation
  void clear();

  inline bool cycle() const  { return _cycle_p; }
  signed char dir(int i) { return _directions[i]; }
  vtol_edge_3d *edge(int i) { return (vtol_edge_3d*)_inferiors[i]; }
  int num_edges(){ return _inferiors.size(); }

  inline bool contains_sub_chains() { return ( _hierarchy_inferiors.size() > 0); }
  inline bool is_sub_chain() { return ( _hierarchy_superiors.size() > 0); }

  vtol_one_chain_3d *copy() { return new vtol_one_chain_3d(*this); }
 
  void determine_edge_directions();
  void add_inferior_one_chain( vtol_one_chain_3d*);
  void remove_inferior_one_chain( vtol_one_chain_3d*);
  bool add_edge(vtol_edge_3d*, bool);
  bool remove_edge(vtol_edge_3d*, bool);


  void deep_remove( vcl_vector< vtol_topology_object_3d * > & removed );

  // Operators
  //: \brief Operators

  bool operator==(const vsol_spatial_object_3d& obj) const ;
  bool operator==(const vtol_one_chain_3d&) const;

  void print(ostream& strm = cout);
  void describe_directions(ostream& strm = cout, int blanking = 0);
  void describe(ostream& strm = cout, int blanking = 0);


};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vtol_one_chain_3d.
