#ifndef VTOL_TOPOLOGY_HIERARCHY_NODE_2D_H
#define VTOL_TOPOLOGY_HIERARCHY_NODE_2D_H


//
// .SECTION Description
//  The vtol_topology_hierarchy_node_2d is a base class inherited by vtol_topology_object_2d.
//  It provides the data and methods for creating the doubly linked structure
//  of inferiors and superiors.
//
// .NAME        vtol_topology_hierarchy_node_2d - Base class of vtol_topology_object_2d (inferior/superior mechanics)
// .LIBRARY     vtol
// .INCLUDE     Topology/vtol_topology_hierarchy_node_2d.h
// .FILE        vtol_topology_hierarchy_node_2d.h
// .FILE        vtol_topology_hierarchy_node_2d.cxx
// .SECTION Author
//     Patricia A. Vrobel - ported Peter Tu
//
//-----------------------------------------------------------------------------

// #include <cool/ListP.h>
// #include <cool/ArrayP.h>

#include <vcl/vcl_vector.h>


class vtol_topology_object_2d;
class vtol_vertex_2d;
class vtol_edge_2d;
class vtol_one_chain_2d;
class vtol_zero_chain_2d;
class vtol_face_2d;
class vtol_block_2d;
class vtol_two_chain_2d;


//useful typedefs!!
typedef vcl_vector<vtol_topology_object_2d*> topology_list_2d;
typedef vcl_vector<vtol_vertex_2d*> vertex_list_2d;
typedef vcl_vector<vtol_edge_2d*> edge_list_2d;
typedef vcl_vector<vtol_one_chain_2d*> one_chain_list_2d;
typedef vcl_vector<vtol_zero_chain_2d*> zero_chain_list_2d;
typedef vcl_vector<vtol_face_2d*> face_list_2d;
typedef vcl_vector<vtol_two_chain_2d*> two_chain_list_2d;
typedef vcl_vector<vtol_block_2d*> block_list_2d;


//: \brief Class for creating doubly linked lists of topology objects
class vtol_topology_hierarchy_node_2d
{
  // public:
  // TOPOLOGY_DLL_DATA static bool DEFER_SUPERIORS;

protected:

  topology_list_2d _inferiors;
  topology_list_2d _superiors;
  topology_list_2d * inf() { return &_inferiors; }
  topology_list_2d * sup() { return &_superiors; }

  // classes used to simplify the find and remove processes

  bool find(topology_list_2d &list, vtol_topology_object_2d *object);
  void remove(topology_list_2d &list, vtol_topology_object_2d *object);
    

public:
  //: \brief Constructors and Destructors


 vtol_topology_hierarchy_node_2d();

 vtol_topology_hierarchy_node_2d(int num_inferiors, int num_superiors);
 ~vtol_topology_hierarchy_node_2d();

  //: \brief   Accessors

  int numinf() const { return _inferiors.size(); }
  int numsup() const { return _superiors.size(); }
 
  void describe_inferiors(ostream& strm=cout,int blanking=0);
  void describe_superiors(ostream& strm=cout,int blanking=0);
  void print(ostream& strm=cout);
  void describe(ostream& strm=cout,int blanking=0);

  inline topology_list_2d * get_inferiors() { return &_inferiors; }
  inline topology_list_2d * get_superiors() { return &_superiors; }

  void set_inferiors(topology_list_2d& newinf) { _inferiors = newinf; }
  void set_superiors(topology_list_2d& newsup) { _superiors = newsup; }
  topology_list_2d * inferiors() { return &_inferiors;}
  topology_list_2d * superiors() { return &_superiors;}


  bool link_inferior(vtol_topology_object_2d*, vtol_topology_object_2d*);
  bool link_superior(vtol_topology_object_2d*, vtol_topology_object_2d*);
  bool link_inferior_oneway(vtol_topology_object_2d*);
  bool link_superior_oneway(vtol_topology_object_2d*);
  bool unlink_inferior(vtol_topology_object_2d*, vtol_topology_object_2d*);
  bool unlink_superior(vtol_topology_object_2d*, vtol_topology_object_2d*);

  void unlink_all_superiors(vtol_topology_object_2d*);
  void unlink_all_inferiors(vtol_topology_object_2d*);
  bool unlink_superior_simple(vtol_topology_object_2d*);
  bool unlink_inferior_simple(vtol_topology_object_2d*);
  void unlink_all_superiors_twoway( vtol_topology_object_2d * );
  void unlink_all_inferiors_twoway( vtol_topology_object_2d * );
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vtol_topology_hierarchy_node_2d.




