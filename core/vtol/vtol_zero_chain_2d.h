#ifndef vtol_zero_chain_2d_H
#define vtol_zero_chain_2d_H
//-----------------------------------------------------------------------------
//
// Class : vtol_zero_chain_2d
//
// .SECTION Description
//  The vtol_zero_chain_2d class is used to represent a set of Vertices on
//  a topological structure. A vtol_zero_chain_2d maintains only the inferiors and
//  superiors. It is the topological inferior of an Edge.
//
// .NAME        vtol_zero_chain_2d - Represents a set of Vertices
// .LIBRARY     Topology
// .HEADER SpatialObjects package
// .INCLUDE     Topology/vtol_zero_chain_2d.h
// .FILE        vtol_zero_chain_2d.h
// .FILE        vtol_zero_chain_2d.cxx
// .SECTION Author
//     Patricia A. Vrobel
//     PTU - ported may 2000
//
// .SECTION Modifications:
//   JLM Dec 1995, Added timeStamp (Touch) to operations which affect bounds.
//   02-26-97 Added implementation for virtual Transform() - Peter Vanroose
//
//-----------------------------------------------------------------------------

#include <vtol/vtol_topology_object_2d.h>
#include <vcl/vcl_vector.h>
#include <vtol/vtol_vertex_2d.h>


class vtol_zero_chain_2d
  : public vtol_topology_object_2d
{
public:

  //: \brief  Constructors and Destructors...
  vtol_zero_chain_2d();
  vtol_zero_chain_2d(vtol_vertex_2d *v1, vtol_vertex_2d *v2);    // special for two vertex edge.
  vtol_zero_chain_2d(vcl_vector<vtol_vertex_2d*>&); // special for two vertex edge.
  vtol_zero_chain_2d(vtol_zero_chain_2d const&);   
  ~vtol_zero_chain_2d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //: See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_ref clone(void) const;

  //: \brief  Easy access methods

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_2d_type topology_type(void) const;

  inline vtol_vertex_2d* v0() { if (_inferiors.size() > 0)
                             return (vtol_vertex_2d *)_inferiors[0];
                             else return (vtol_vertex_2d *)0;
                         }

  vtol_zero_chain_2d * cast_to_zero_chain_2d() { return this; }
  void inlink_delete();
  int length() const { return _inferiors.size(); }

  vcl_vector<vtol_vertex_2d*>* vertices();
  vcl_vector<vtol_zero_chain_2d*>* zero_chains();
  vcl_vector<vtol_edge_2d*>* edges();
  vcl_vector<vtol_one_chain_2d*>* one_chains();
  vcl_vector<vtol_face_2d*>* faces();
  vcl_vector<vtol_two_chain_2d*>* two_chains();
  vcl_vector<vtol_block_2d*>* blocks();

  vtol_zero_chain_2d * copy();
  vtol_topology_object_2d * shallow_copy_with_no_links ( void );
  virtual vsol_spatial_object_2d * spatial_copy() { return this->copy(); }

  inline bool add_vertex(vtol_vertex_2d *v) { this->touch(); return link_inferior(v);}

  inline bool remove_vertex(vtol_vertex_2d *v){this->touch(); return unlink_inferior(v);}

  
  void print (ostream& strm =cout);
  void describe (ostream& strm =cout, int blanking = 0);


  bool operator==(vsol_spatial_object_2d& );
  bool operator== (vtol_zero_chain_2d&);

  bool remove( vtol_vertex_2d * vertex,
               vcl_vector< vtol_topology_object_2d * > & changes,
               vcl_vector< vtol_topology_object_2d * > & deleted );
  void deep_remove( vcl_vector< vtol_topology_object_2d * > & removed );

  virtual bool disconnect( vcl_vector< vtol_topology_object_2d * > & changes,
                           vcl_vector< vtol_topology_object_2d * > & deleted );
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vtol_zero_chain_2d.
