#ifndef VTOL_TOPOLOGY_HIERARCHY_NODE_3D_H
#define VTOL_TOPOLOGY_HIERARCHY_NODE_3D_H


//
// .SECTION Description
//  The vtol_topology_hierarchy_node_3d is a base class inherited by vtol_topology_object_3d.
//  It provides the data and methods for creating the doubly linked structure
//  of inferiors and superiors.
//
// .NAME        vtol_topology_hierarchy_node_3d - Base class of vtol_topology_object_3d (inferior/superior mechanics)
// .LIBRARY     vtol
// .INCLUDE     Topology/vtol_topology_hierarchy_node_3d.h
// .FILE        vtol_topology_hierarchy_node_3d.h
// .FILE        vtol_topology_hierarchy_node_3d.cxx
// .SECTION Author
//     Patricia A. Vrobel - ported Peter Tu
//
//-----------------------------------------------------------------------------

// #include <cool/ListP.h>
// #include <cool/ArrayP.h>

#include <vcl/vcl_vector.h>


class vtol_topology_object_3d;
class vtol_vertex_3d;
class vtol_edge_3d;
class vtol_one_chain_3d;
class vtol_zero_chain_3d;
class vtol_face_3d;
class vtol_block_3d;
class vtol_two_chain_3d;


//useful typedefs!!
typedef vcl_vector<vtol_topology_object_3d*> topology_list_3d;
typedef vcl_vector<vtol_vertex_3d*> vertex_list_3d;
typedef vcl_vector<vtol_edge_3d*> edge_list_3d;
typedef vcl_vector<vtol_one_chain_3d*> one_chain_list_3d;
typedef vcl_vector<vtol_zero_chain_3d*> zero_chain_list_3d;
typedef vcl_vector<vtol_face_3d*> face_list_3d;
typedef vcl_vector<vtol_two_chain_3d*> two_chain_list_3d;
typedef vcl_vector<vtol_block_3d*> block_list_3d;


//: \brief Class for creating doubly linked lists of topology objects
class vtol_topology_hierarchy_node_3d
{
  // public:
  // TOPOLOGY_DLL_DATA static bool DEFER_SUPERIORS;

protected:

  topology_list_3d _inferiors;
  topology_list_3d _superiors;
  topology_list_3d * inf() { return &_inferiors; }
  topology_list_3d * sup() { return &_superiors; }

  // classes used to simplify the find and remove processes

  bool find(topology_list_3d &list, vtol_topology_object_3d *object);
  void remove(topology_list_3d &list, vtol_topology_object_3d *object);
    

public:
  //: \brief Constructors and Destructors


 vtol_topology_hierarchy_node_3d();

 vtol_topology_hierarchy_node_3d(int num_inferiors, int num_superiors);
 ~vtol_topology_hierarchy_node_3d();

  //: \brief   Accessors

  int numinf() const { return _inferiors.size(); }
  int numsup() const { return _superiors.size(); }
 
  void describe_inferiors(ostream& strm=cout,int blanking=0) const;
  void describe_superiors(ostream& strm=cout,int blanking=0) const;
  void print(ostream& strm=cout) const;
  void describe(ostream& strm=cout,int blanking=0) const;

  inline topology_list_3d * get_inferiors() { return &_inferiors; }
  inline topology_list_3d * get_superiors() { return &_superiors; }

  inline const topology_list_3d * get_inferiors() const { return &_inferiors; }
  inline const topology_list_3d * get_superiors() const { return &_superiors; }
  
  void set_inferiors(topology_list_3d& newinf) { _inferiors = newinf; }
  void set_superiors(topology_list_3d& newsup) { _superiors = newsup; }
  topology_list_3d * inferiors() { return &_inferiors;}
  topology_list_3d * superiors() { return &_superiors;}


  bool link_inferior(vtol_topology_object_3d*, vtol_topology_object_3d*);
  bool link_superior(vtol_topology_object_3d*, vtol_topology_object_3d*);
  bool link_inferior_oneway(vtol_topology_object_3d*);
  bool link_superior_oneway(vtol_topology_object_3d*);
  bool unlink_inferior(vtol_topology_object_3d*, vtol_topology_object_3d*);
  bool unlink_superior(vtol_topology_object_3d*, vtol_topology_object_3d*);

  void unlink_all_superiors(vtol_topology_object_3d*);
  void unlink_all_inferiors(vtol_topology_object_3d*);
  bool unlink_superior_simple(vtol_topology_object_3d*);
  bool unlink_inferior_simple(vtol_topology_object_3d*);
  void unlink_all_superiors_twoway( vtol_topology_object_3d * );
  void unlink_all_inferiors_twoway( vtol_topology_object_3d * );
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vtol_topology_hierarchy_node_3d.




