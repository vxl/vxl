#ifndef vtol_zero_chain_3d_H
#define vtol_zero_chain_3d_H
//-----------------------------------------------------------------------------
//
// .NAME        vtol_zero_chain_3d - Represents a set of Vertices
// .LIBRARY     vtol
// .HEADER	vxl package
// .INCLUDE     vtol/vtol_zero_chain_3d.h
// .FILE        vtol_zero_chain_3d.cxx
//
// .SECTION Description
//  The vtol_zero_chain_3d class is used to represent a set of Vertices on
//  a topological structure. A vtol_zero_chain_3d maintains only the inferiors and
//  superiors. It is the topological inferior of an Edge.
//
// .SECTION Author
//     Patricia A. Vrobel
//     PTU - ported may 2000
//
// .SECTION Modifications:
//   JLM Dec 1995, Added timeStamp (Touch) to operations which affect bounds.
//   02-26-97 Added implementation for virtual Transform() - Peter Vanroose
//
//-----------------------------------------------------------------------------

#include <vtol/vtol_topology_object_3d.h>
#include <vcl/vcl_vector.h>
#include <vtol/vtol_vertex_3d.h>


class vtol_zero_chain_3d
  : public vtol_topology_object_3d
{
public:

  //: \brief  Constructors and Destructors...
  vtol_zero_chain_3d();
  vtol_zero_chain_3d(vtol_vertex_3d *v1, vtol_vertex_3d *v2);    // special for two vertex edge.
  vtol_zero_chain_3d(vcl_vector<vtol_vertex_3d*>&); // special for two vertex edge.
  vtol_zero_chain_3d(vtol_zero_chain_3d const&);   
  ~vtol_zero_chain_3d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //: See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d_ref clone(void) const;

  //: \brief  Easy access methods

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_3d_type topology_type(void) const;

  inline vtol_vertex_3d* v0() { if (_inferiors.size() > 0)
                             return (vtol_vertex_3d *)_inferiors[0];
                             else return (vtol_vertex_3d *)0;
                         }

  vtol_zero_chain_3d * cast_to_zero_chain_3d() { return this; }
  void inlink_delete();
  int length() const { return _inferiors.size(); }

  vcl_vector<vtol_vertex_3d*>* vertices();
  vcl_vector<vtol_zero_chain_3d*>* zero_chains();
  vcl_vector<vtol_edge_3d*>* edges();
  vcl_vector<vtol_one_chain_3d*>* one_chains();
  vcl_vector<vtol_face_3d*>* faces();
  vcl_vector<vtol_two_chain_3d*>* two_chains();
  vcl_vector<vtol_block_3d*>* blocks();

  vtol_zero_chain_3d * copy();
  vtol_topology_object_3d * shallow_copy_with_no_links ( void );
  virtual vsol_spatial_object_3d * spatial_copy() { return this->copy(); }

  inline bool add_vertex(vtol_vertex_3d *v) { this->touch(); return link_inferior(v);}

  inline bool remove_vertex(vtol_vertex_3d *v){this->touch(); return unlink_inferior(v);}

  
  void print (ostream& strm =cout) const;
  void describe (ostream& strm =cout, int blanking = 0) const;


  bool operator== (const vtol_zero_chain_3d&) const;
  bool operator==(const vsol_spatial_object_3d& obj) const; // virtual of vsol_spatial_object_3d

  bool remove( vtol_vertex_3d * vertex,
               vcl_vector< vtol_topology_object_3d * > & changes,
               vcl_vector< vtol_topology_object_3d * > & deleted );
  void deep_remove( vcl_vector< vtol_topology_object_3d * > & removed );

  virtual bool disconnect( vcl_vector< vtol_topology_object_3d * > & changes,
                           vcl_vector< vtol_topology_object_3d * > & deleted );
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vtol_zero_chain_3d.
